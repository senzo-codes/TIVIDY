#include "WorkflowInstance.h"
#include "WorkflowDefinition.h"
#include "TaskComponent.h"
#include "TaskObserver.h"
#include "AuditEvent.h"
#include "TaskComposite.h"

/**
 * @brief Constructs a workflow instance
 * @param id The instance number
 * @param name The name shown to the user
 * @param definition The definition this instance follows.
 */
WorkflowInstance::WorkflowInstance(int id, const std::string &name, const WorkflowDefinition *definition)
    : id(id), name(name), status(definition->getInitialStatus()), definition(definition) {}
/**
 * @brief Destructor of the work fow instance and its variables
 */
WorkflowInstance::~WorkflowInstance()
{
    for (auto obs : observers) {
        delete obs;
    }
    observers.clear();

    for(TaskComponent* task : tasks) delete task;

    tasks.clear();
}

/**
 * @brief Returns the id.
 * @return id
 */
int WorkflowInstance::getId() const
{
    return id;
}

/**
 * @brief Returns the name.
 * @return name
 */
std::string WorkflowInstance::getName() const
{
    return name;
}

/**
 * @brief Returns the status.
 * @return status
 */
std::string WorkflowInstance::getStatus() const
{
    return status;
}

/**
 * @brief Changes the status.
 * @details Only the runner calls this. 
 * @param status The status to set
 */
void WorkflowInstance::setStatus(const std::string &status)
{
    this->status = status;
}

/**
 * @brief Returns the definition.
 * @return definition
 */
const WorkflowDefinition *WorkflowInstance::getDefinition() const
{
    return definition;
}

/**
 * @brief Return data linked to the passed in key
 * @param key
 * @return string, or empty string if key not found
 */
std::string WorkflowInstance::getData(const std::string &key) const
{
    for (const auto& entry : data) {
        if (entry.first == key) {
            return entry.second;
        }
    }
    return "";
}

/**
 * @brief Sets a data value on this instance.
 * @param key   The name of the data entry.
 * @param value The value to store for that key.
 */
void WorkflowInstance::setData(const std::string& key, const std::string& value)
{
    DataMap::iterator it = data.find(key);
    std::string oldValue = (it == data.end()) ? std::string() : it->second;
    if (it == data.end()) data[key] = value;
    else it->second = value;

    logEvent(AuditType::DataChanged, name, "Data '" + key + "' changed from '" + oldValue + "' to '" + value + "'");
}

const DataMap& WorkflowInstance::getDataMap() const 
{ 
    return data; 
}

/**
 * @brief Adds a top-level task to this instance.
 * @param task The task to add. Must not already belong to a composite.
 */
void WorkflowInstance::addTask(TaskComponent *task)
{
    tasks.push_back(task);
}
 
/**
 * @brief Keeps an observer alive for as long as this instance exists.
 * @param observer The observer to store.
 */
void WorkflowInstance::addObserver(TaskObserver *observer)
{
    observers.push_back(observer);
}

/**
 * @brief Removes a top-level task by id and deletes it.
 * @param taskId The id of the task to remove.
 */
void WorkflowInstance::removeTask(int taskId)
{
    for (size_t i = 0; i < tasks.size(); i++) {
        if (tasks[i]->getId() == taskId) {
            TaskComponent *task = tasks[i];
            tasks.erase(tasks.begin() + i);
            delete task;

            return;
        }
    }
}

/**
 * @brief Finds a task by id.
 * @param taskId The id of the task to find.
 * @return The matching task, or nullptr if no task has that id.
 */
TaskComponent *WorkflowInstance::findTask(int taskId) const
{
    for (size_t i = 0; i < tasks.size(); i++) {
        if (tasks[i]->getId() == taskId) {  
            return tasks[i];
        }
        TaskComposite *comp = dynamic_cast<TaskComposite*>(tasks[i]);
        if (comp) {
            for (auto j = 0; j < comp->childCount(); j++) {
                TaskComponent *child = comp->getChild(j);
                if (child->getId() == taskId) {
                    return child;
                }
            }
        }
    }
    return nullptr;
}

/**
 * @brief Returns the top-level tasks of this instance.
 * @return A read-only reference to the top-level task list.
 */
const std::vector<TaskComponent *> &WorkflowInstance::getTasks() const
{
    return tasks;
}

/**
 * @brief Returns every open leaf task of this instance.
 * @return A new vector of the open leaf tasks. The tasks remain owned
 *         by the instance.
 */
std::vector<TaskComponent *> WorkflowInstance::openLeafTasks() const
{
    std::vector<TaskComponent *> subTasks;

    for (size_t i = 0; i < tasks.size(); i++) {
        TaskComposite *comp = dynamic_cast<TaskComposite*>(tasks[i]);

        if (comp) {
            for (auto j = 0; j < comp->childCount(); j++) {
                TaskComponent *child = comp->getChild(j);
                if (!child->isClosed()) {
                    subTasks.push_back(child);
                }
            }
            continue;
        }
        if (!tasks[i]->isClosed()) {  
            subTasks.push_back(tasks[i]);
        }
    }
    return subTasks;
}

/**
 * @brief Adds one entry to this instance's audit log.
 * @param type   The kind of event that happened.
 * @param actor  Who caused the event.
 * @param detail A sentence describing the event.
 */
void WorkflowInstance::logEvent(AuditType type, const std::string &actor, const std::string &detail)
{
    std::time_t now = std::time(nullptr);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    auditLog.push_back(AuditEvent(buf,type, actor, detail));
}

/**
 * @brief Returns the audit log of this instance.
 * @return A read-only reference to the audit log.
 */
const std::vector<AuditEvent> &WorkflowInstance::getAuditLog() const
{
    return auditLog;
}
