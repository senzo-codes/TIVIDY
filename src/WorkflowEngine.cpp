#include "WorkflowEngine.h"
#include "ParticipantRegistry.h"
#include "LeastLoadedAssignment.h"
#include "RoleBasedAssignment.h"
#include "ApprovalService.h"
#include "WorkflowRunner.h"
#include "SnapshotHistory.h"
#include "WorkflowDefinition.h"
#include "AutomatedTask.h"
#include "WorkflowInstance.h"
#include "WorkflowSnapshot.h"
#include "CompletedState.h"
#include "CancelledState.h"
#include "InProgressState.h"
#include "CompletionObserver.h"
#include "AuditObserver.h"
#include "TaskComposite.h"
#include "Participant.h"
#include "HumanTask.h"
#include "Errors.h"

/// @brief Creates all the subsystems
WorkflowEngine::WorkflowEngine() : nextInstanceId(1), today("")
{
    registry = new ParticipantRegistry();
    leastLoaded = new LeastLoadedAssignment(registry);
    assignment = new RoleBasedAssignment(registry, leastLoaded);
    approvals = new ApprovalService(registry, leastLoaded);
    runner = new WorkflowRunner(registry, assignment, approvals);
    history = new SnapshotHistory();
}

/// @brief Deletes the instances before the subsystems
WorkflowEngine::~WorkflowEngine()
{
    for (auto i = 0; i < instances.size(); ++i) {
        delete instances[i];
    }
    instances.clear();

    for (std::map<std::string, WorkflowDefinition*>::iterator it = definitions.begin(); it != definitions.end(); ++it) {
        delete it->second;
    }
    definitions.clear();
    delete registry;
    delete leastLoaded;
    delete assignment;
    delete approvals;
    delete runner;
    delete history;

}

/**
 * @brief Used to add a participant to the system
 * @param id Unique id of the participant
 * @param name Name of the participant
 * @param level Authority level of the participant
 * @param roleName Label of the participant's role
 * @note Refused if the id is already taken
 */
void WorkflowEngine::addParticipant(int id, const std::string &name, int level, const std::string &roleName)
{
    if (registry->find(id)) {
        return;
    }
    Role role(level, roleName);
    Participant* part = new Participant(id, name, role);
    registry->add(part);
}

/**
 * @brief Used to set the current date for the runner and deadline checks
 * @param today The current date
 */
void WorkflowEngine::setToday(const std::string &today)
{
    this->today = today;
} 

/**
 * @brief Used to store a process map under its name
 * @param definition The definition to register
 * @note Refused if the name is taken. The caller keeps ownership of a refused definition
 */
void WorkflowEngine::registerDefinition(WorkflowDefinition *definition)
{
    for (std::map<std::string, WorkflowDefinition*>::iterator it = definitions.begin();it != definitions.end(); ++it) {
        if (it->first == definition->getName()) {
            std::cout << "Definition name already registered";
            return;
        }
    }
    this->definitions.insert({definition->getName(), definition});
}

/**
 * @brief Used to create an instance and ask the runner to create its first tasks
 * @param definitionName Name of the registered definition to follow
 * @param name Name shown to the user for this instance
 * @return It either returns the new instance
 * @return Or, it returns nullptr if the definition name is unknown
 */
WorkflowInstance *WorkflowEngine::startInstance(const std::string &definitionName, const std::string &name)
{
    std::map<std::string, WorkflowDefinition*>::iterator it = definitions.find(definitionName);

    if (it == definitions.end()) {
        return nullptr;
    }

    WorkflowInstance* instance = new WorkflowInstance(nextInstanceId++, name, it->second);

    instances.push_back(instance);
    runner->start(*instance);

    return instance;
}

/**
 * @brief Used to find the instance with the passed in ID
 * @param id Instance id number
 * @return It either returns the instance, if a matching ID is found
 * @return Or, it returns nullptr
 */
WorkflowInstance *WorkflowEngine::findInstance(int id) const
{
    for (auto i = 0; i < instances.size(); ++i) {
        if (instances[i]->getId() == id) {
            return instances[i];
        }
    }
    return nullptr;
}

/**
 * @brief Used to start a task by finding the instance, task and participant, then asking the runner
 * @param instanceId Id of the instance holding the task
 * @param taskId Id of the task to start
 * @param participantId Id of the participant starting it
 * @throws std::invalid_argument If the instance, task or participant is unknown
 */
void WorkflowEngine::startTask(int instanceId, int taskId, int participantId)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if (instance == nullptr) {
        throw std::invalid_argument("Unknown instance");
    }

    TaskComponent* task = instance->findTask(taskId);
    if (task == nullptr) {
        throw std::invalid_argument("Unknown task");
    }

    Participant* participant = registry->find(participantId);
    if (participant == nullptr) {
        throw std::invalid_argument("Unknown participant");
    }

    runner->startTask(*instance, task, *participant);
}

/**
 * @brief Used to complete a task by finding the instance, task and participant, then asking the runner
 * @param instanceId Id of the instance holding the task
 * @param taskId Id of the task to complete
 * @param participantId Id of the participant completing it
 * @throws std::invalid_argument If the instance, task or participant is unknown
 */
void WorkflowEngine::completeTask(int instanceId, int taskId, int participantId)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if (instance == nullptr) {
        throw std::invalid_argument("Unknown instance");
    }

    TaskComponent* task = instance->findTask(taskId);
    if (task == nullptr) {
        throw std::invalid_argument("Unknown task");
    }

    Participant* participant = registry->find(participantId);
    if (participant == nullptr) {
        throw std::invalid_argument("Unknown participant");
    }

    runner->completeTask(*instance, task, *participant);
}

/**
 * @brief Used to cancel an instance by finding it, then asking the runner
 * @param instanceId Id of the instance to cancel
 * @throws std::invalid_argument If the instance is unknown
 */
void WorkflowEngine::cancelInstance(int instanceId)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if (instance == nullptr) {
        throw std::invalid_argument("Unknown instance");
    }

    runner->cancelInstance(*instance);
}
/**
 * @brief Used to request approval for a task by finding the three objects, then asking the approval service
 * @param instanceId Id of the instance holding the task
 * @param taskId Id of the task needing approval
 * @param participantId Id of the participant asking
 * @return The approval request that was created
 * @throws std::invalid_argument If the instance, task or participant is unknown
 */
ApprovalRequest *WorkflowEngine::requestApproval(int instanceId, int taskId, int participantId)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if (instance == nullptr) {
        throw std::invalid_argument("Unknown instance");
    }

    TaskComponent* task = instance->findTask(taskId);
    if (task == nullptr) {
        throw std::invalid_argument("Unknown task");
    }

    Participant* participant = registry->find(participantId);
    if (participant == nullptr) {
        throw std::invalid_argument("Unknown participant");
    }

    return approvals->request(*instance, task, *participant);
}

/**
 * @brief Used to accept an approval request by finding the instance and person, then asking the approval service
 * @param instanceId Id of the instance the request belongs to
 * @param request The request being accepted
 * @param participantId Id of the participant accepting it
 * @throws std::invalid_argument If the instance or participant is unknown
 */
void WorkflowEngine::acceptRequest(int instanceId, ApprovalRequest *request, int participantId)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if (instance == nullptr) {
        throw std::invalid_argument("Unknown instance");
    }

    Participant* participant = registry->find(participantId);
    if (participant == nullptr) {
        throw std::invalid_argument("Unknown participant");
    }

    approvals->accept(*instance, request, *participant);
}

/**
 * @brief Used to pass an approval request up a level by finding the instance and person, then asking the approval service
 * @param instanceId Id of the instance the request belongs to
 * @param request The request being passed up
 * @param participantId Id of the participant passing it up
 * @throws std::invalid_argument If the instance or participant is unknown
 */
void WorkflowEngine::passUpRequest(int instanceId, ApprovalRequest *request, int participantId)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if (instance == nullptr) {
        throw std::invalid_argument("Unknown instance");
    }

    Participant* participant = registry->find(participantId);
    if (participant == nullptr) {
        throw std::invalid_argument("Unknown participant");
    }

    approvals->passUp(*instance, request, *participant);
}

/**
 * @brief Used to save every instance and task as a text block, stored with today's date
 */
void WorkflowEngine::saveSnapshot()
{
    history->saveSnapshot(new WorkflowSnapshot(today, serializeInstances()));
}

/**
 * @brief Used to replace the current instances with the ones in a saved snapshot, then recalculate workloads
 * @param index Position of the snapshot in the history
 * @throws std::out_of_range If the index does not exist
 * @throws std::runtime_error If a definition named in the snapshot is not registered
 */
void WorkflowEngine::restoreSnapshot(int index)
{
    if (index < 0 || index >= history->count()) {
        throw std::out_of_range("No such snapshot");
    }

    std::string data = history->restoreSnapshot(index).getTasksData();

    for (auto i = 0; i < instances.size(); ++i) {
        delete instances[i];
    }
    instances.clear();

    std::istringstream in(data);
    std::string line;
    WorkflowInstance* current = nullptr;
    TaskComposite* currentComposite = nullptr;

    while (std::getline(in, line)) {
        std::vector<std::string> f;
        std::istringstream lineStream(line);
        std::string piece;
        while (std::getline(lineStream, piece, '|')) {
            f.push_back(piece);
        }
        if (f.empty()) continue;

        if (f[0] == "I") {
            std::map<std::string, WorkflowDefinition*>::iterator it = definitions.find(f[3]);
            if (it == definitions.end()) {
                throw std::runtime_error("Definition missing for snapshot");
            }

            current = new WorkflowInstance(std::stoi(f[1]), f[2], it->second);
            current->setStatus(f[4]);
            current->setFlagged(f[5] == "1");
            instances.push_back(current);
            currentComposite = nullptr;
        }
        else if (f[0] == "T") {
            char type = f[1][0];
            int id = std::stoi(f[2]);
            int parentId = std::stoi(f[4]);
            int assignee = std::stoi(f[6]);
            Role role(std::stoi(f[8]), "");

            TaskComponent* task = nullptr;

            if (type == 'H') {
                HumanTask* human = new HumanTask(id, f[3], role, f[7], assignee);

                if (f[5] == "InProgress") {
                    human->setState(&InProgressState::instance());
                } else if (f[5] == "Completed") {
                    human->setState(&CompletedState::instance());
                } else if (f[5] == "Cancelled") {
                    human->setState(&CancelledState::instance());
                }

                current->addObserver(new CompletionObserver(human, current, runner));
                current->addObserver(new AuditObserver(human, current, registry));

                task = human;
            }
            else if (type == 'A') {
                AutomatedTask* automated = new AutomatedTask(id, f[3]);
                if (f[5] == "Completed") {
                    automated->execute();
                }
                task = automated;
            }
            else {
                currentComposite = new TaskComposite(id, f[3], role);
                task = currentComposite;
            }

            if (parentId == -1) {
                current->addTask(task);
            } else {
                currentComposite->add(task);
            }
        }
    }

    std::vector<TaskComponent*> openTasks;
    for (auto i = 0; i < instances.size(); ++i) {
        std::vector<TaskComponent*> open = instances[i]->openLeafTasks();
        openTasks.insert(openTasks.end(), open.begin(), open.end());
    }
    registry->recountLoad(openTasks);
}

/**
 * @brief Used to walk the open tasks held by one participant across all instances
 * @param participantId Id of the participant
 * @return An iterator over that participant's open tasks
 */
PendingTaskIterator WorkflowEngine::dashboardFor(int participantId) const
{
    return PendingTaskIterator(instances, participantId);
}

/**
 * @brief Used to write every instance and task into one text block
 * @return One line per instance and one line per task
 */
std::string WorkflowEngine::serializeInstances() const
{
    std::ostringstream out;

    for (auto i = 0; i < instances.size(); ++i) {
        WorkflowInstance* instance = instances[i];

        out << "I|" << instance->getId()
            << "|" << instance->getName()
            << "|" << instance->getDefinition()->getName()
            << "|" << instance->getStatus()
            << "|" << (instance->isFlagged() ? 1 : 0)
            << "\n";

        const std::vector<TaskComponent*>& tasks = instance->getTasks();

        for (auto j = 0; j < tasks.size(); ++j) {
            TaskComponent* task = tasks[j];
            TaskComposite* composite = dynamic_cast<TaskComposite*>(task);

            char type = 'A';
            if (dynamic_cast<HumanTask*>(task) != nullptr) {
                type = 'H';
            } else if (composite != nullptr) {
                type = 'C';
            }

            out << "T|" << type
                << "|" << task->getId()
                << "|" << task->getName()
                << "|" << -1
                << "|" << task->getStatus()
                << "|" << task->getAssignee()
                << "|" << task->getDueDate()
                << "|" << task->getRequiredRole().getLevel()
                << "\n";

            if (composite != nullptr) {
                for (int k = 0; k < composite->childCount(); ++k) {
                    TaskComponent* child = composite->getChild(k);

                    char childType = 'A';
                    if (dynamic_cast<HumanTask*>(child) != nullptr) {
                        childType = 'H';
                    }

                    out << "T|" << childType
                        << "|" << child->getId()
                        << "|" << child->getName()
                        << "|" << task->getId()
                        << "|" << child->getStatus()
                        << "|" << child->getAssignee()
                        << "|" << child->getDueDate()
                        << "|" << child->getRequiredRole().getLevel()
                        << "\n";
                }
            }
        }
    }

    return out.str();
}