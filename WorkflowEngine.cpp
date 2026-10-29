#include "WorkflowEngine.h"
#include "ParticipantRegistry.h"
#include "Participant.h"
#include "Role.h"
#include "LeastLoadedAssignment.h"
#include "RoleBasedAssignment.h"
#include "ApprovalService.h"
#include "ApprovalRequest.h"
#include "WorkflowRunner.h"
#include "WorkflowDefinition.h"
#include "WorkflowInstance.h"
#include "SnapshotHistory.h"
#include "WorkflowSnapshot.h"
#include "TaskComponent.h"
#include "TaskComposite.h"
#include "HumanTask.h"
#include "PendingState.h"
#include "InProgressState.h"
#include "CompletedState.h"
#include "CancelledState.h"
#include "AuditType.h"
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace
{

    // Snapshot text format, one record per line, fields separated by '|':
    //   I|instanceId|name|status
    //   D|instanceId|key|value
    //   T|instanceId|taskId|parentId|name|status|assigneeId
    // parentId is -1 for a top-level task.

    std::string esc(const std::string &s)
    {
        std::string out;
        for (std::size_t i = 0; i < s.size(); ++i)
        {
            char c = s[i];
            if (c == '\\')
                out += "\\\\";
            else if (c == '|')
                out += "\\p";
            else if (c == '\n')
                out += "\\n";
            else
                out += c;
        }
        return out;
    }

    std::string unesc(const std::string &s)
    {
        std::string out;
        for (std::size_t i = 0; i < s.size(); ++i)
        {
            if (s[i] == '\\' && i + 1 < s.size())
            {
                char n = s[++i];
                out += (n == 'p') ? '|' : (n == 'n') ? '\n'
                                                     : n;
            }
            else
            {
                out += s[i];
            }
        }
        return out;
    }

    std::vector<std::string> split(const std::string &line)
    {
        std::vector<std::string> fields;
        std::string current;
        for (std::size_t i = 0; i < line.size(); ++i)
        {
            if (line[i] == '|')
            {
                fields.push_back(current);
                current.clear();
            }
            else
            {
                current += line[i];
            }
        }
        fields.push_back(current);
        return fields;
    }

    struct TaskRecord
    {
        int id;
        int parentId;
        std::string status;
        int assignee;
    };

    struct InstanceRecord
    {
        std::string status;
        DataMap data;
        std::vector<TaskRecord> tasks;
    };

    /// @brief Builds a new state object for a saved status name.
    TaskStates *stateFromName(const std::string &name)
    {
        if (name == "Pending")
            return new PendingState();
        if (name == "InProgress")
            return new InProgressState();
        if (name == "Completed")
            return new CompletedState();
        if (name == "Cancelled")
            return new CancelledState();
        throw std::invalid_argument("unknown status in snapshot: " + name);
    }

    /// @brief Writes a task, then its children if it is a composite.
    void writeTask(std::ostringstream &out, int instanceId, TaskComponent *task, int parentId)
    {
        out << "T|" << instanceId << '|' << task->getId() << '|' << parentId << '|'
            << esc(task->getName()) << '|' << esc(task->getStatus()) << '|'
            << task->getAssignee() << '\n';

        if (TaskComposite *composite = dynamic_cast<TaskComposite *>(task))
        {
            const std::vector<TaskComponent *> &children = composite->getChildren();
            for (std::size_t i = 0; i < children.size(); ++i)
            {
                writeTask(out, instanceId, children[i], task->getId());
            }
        }
    }

} // namespace

WorkflowEngine::WorkflowEngine()
    : registry(new ParticipantRegistry()),
      leastLoaded(new LeastLoadedAssignment(registry)),
      assignment(new RoleBasedAssignment(registry, leastLoaded)),
      approvals(new ApprovalService(registry, assignment)),
      runner(new WorkflowRunner(registry, assignment, approvals)),
      history(new SnapshotHistory()),
      today(""),
      nextInstanceId(1) {}

WorkflowEngine::~WorkflowEngine()
{
    // Instances go first, because they refer to definitions.
    for (WorkflowInstance *inst : instances)
        delete inst;
    instances.clear();

    for (auto &definition : definitions)
        delete definition.second;
    definitions.clear();

    delete history;
    delete runner;
    delete approvals;
    delete assignment;
    delete leastLoaded;
    delete registry;
}

void WorkflowEngine::addParticipant(int id, const std::string &name, int level, const std::string &roleName)
{
    if (registry->find(id) != nullptr)
    {
        throw std::invalid_argument("Participant id already exists");
    }
    registry->add(new Participant(id, name, Role(level, roleName)));
    approvals->rebuildChain();
}

void WorkflowEngine::setToday(const std::string &t)
{
    today = t;
    runner->setToday(t);
}

void WorkflowEngine::registerDefinition(WorkflowDefinition *definition)
{
    if (definition == nullptr)
    {
        throw std::invalid_argument("Null definition");
    }
    if (definitions.count(definition->getName()))
    {
        throw std::invalid_argument("Definition name already registered");
    }
    definitions[definition->getName()] = definition;
}

WorkflowInstance *WorkflowEngine::startInstance(const std::string &definitionName, const std::string &name, const DataMap &data)
{
    std::map<std::string, WorkflowDefinition *>::iterator it = definitions.find(definitionName);
    if (it == definitions.end())
    {
        throw std::invalid_argument("Unknown definition: " + definitionName);
    }

    WorkflowInstance *instance = new WorkflowInstance(nextInstanceId++, name, it->second);

    for (DataMap::const_iterator d = data.begin(); d != data.end(); ++d)
    {
        instance->setData(d->first, d->second);
    }

    instances.push_back(instance);
    runner->start(*instance);
    return instance;
}

WorkflowInstance *WorkflowEngine::findInstance(int id) const
{
    for (std::size_t i = 0; i < instances.size(); ++i)
    {
        if (instances[i]->getId() == id)
            return instances[i];
    }
    return nullptr;
}

PendingTaskIterator WorkflowEngine::dashboardFor(int participantId)
{
    return PendingTaskIterator(instances, participantId);
}

void WorkflowEngine::startTask(int instanceId, int taskId, int participantId)
{
    WorkflowInstance *inst = findInstance(instanceId);
    if (!inst)
        throw std::invalid_argument("No such instance");
    TaskComponent *task = inst->findTask(taskId);
    if (!task)
        throw std::invalid_argument("No such task");
    Participant *p = registry->find(participantId);
    if (!p)
        throw std::invalid_argument("No such participant");
    runner->startTask(*inst, task, *p);
}

void WorkflowEngine::completeTask(int instanceId, int taskId, int participantId)
{
    WorkflowInstance *inst = findInstance(instanceId);
    if (!inst)
        throw std::invalid_argument("No such instance");
    TaskComponent *task = inst->findTask(taskId);
    if (!task)
        throw std::invalid_argument("No such task");
    Participant *p = registry->find(participantId);
    if (!p)
        throw std::invalid_argument("No such participant");
    runner->completeTask(*inst, task, *p);
}

void WorkflowEngine::cancelInstance(int instanceId)
{
    WorkflowInstance *inst = findInstance(instanceId);
    if (!inst)
        throw std::invalid_argument("No such instance");
    runner->cancelInstance(*inst);
}

void WorkflowEngine::setPriority(int instanceId, int taskId, bool priority)
{
    WorkflowInstance *instance = findInstance(instanceId);
    if (instance == nullptr)
    {
        throw std::invalid_argument("unknown case");
    }

    TaskComponent *task = instance->findTask(taskId);
    if (task == nullptr)
    {
        throw std::invalid_argument("unknown task");
    }

    HumanTask *human = dynamic_cast<HumanTask *>(task);
    if (human == nullptr)
    {
        throw std::logic_error("only human tasks can be marked as priority");
    }

    human->setPriority(priority);
    instance->logEvent(AuditType::PriorityChanged, "Engine",
                       "'" + task->getName() + "' " + (priority ? "marked as priority" : "priority removed"));
}

ApprovalRequest *WorkflowEngine::requestApproval(int instanceId, int taskId, int participantId)
{
    WorkflowInstance *inst = findInstance(instanceId);
    if (!inst)
        throw std::invalid_argument("No such instance");
    TaskComponent *task = inst->findTask(taskId);
    if (!task)
        throw std::invalid_argument("No such task");
    Participant *p = registry->find(participantId);
    if (!p)
        throw std::invalid_argument("No such participant");
    return approvals->request(*inst, task, *p);
}

void WorkflowEngine::acceptRequest(int instanceId, ApprovalRequest *request, int participantId)
{
    WorkflowInstance *inst = findInstance(instanceId);
    if (!inst)
        throw std::invalid_argument("No such instance");
    Participant *p = registry->find(participantId);
    if (!p)
        throw std::invalid_argument("No such participant");
    approvals->accept(*inst, request, *p);
}

void WorkflowEngine::passUpRequest(int instanceId, ApprovalRequest *request, int participantId)
{
    WorkflowInstance *inst = findInstance(instanceId);
    if (!inst)
        throw std::invalid_argument("No such instance");
    Participant *p = registry->find(participantId);
    if (!p)
        throw std::invalid_argument("No such participant");
    approvals->passUp(*inst, request, *p);
}

void WorkflowEngine::saveSnapshot()
{
    history->saveSnapshot(new WorkflowSnapshot(today, serializeInstances()));
}

void WorkflowEngine::restoreSnapshot(int index)
{
    const WorkflowSnapshot &snapshot = history->restoreSnapshot(index); // throws if the index is bad

    // 1. Parse the whole snapshot before changing anything.
    std::map<int, InstanceRecord> saved;
    std::istringstream in(snapshot.getTasksData());
    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty())
            continue;
        std::vector<std::string> f = split(line);
        if (f[0] == "I" && f.size() >= 4)
        {
            saved[std::stoi(f[1])].status = unesc(f[3]);
        }
        else if (f[0] == "D" && f.size() >= 4)
        {
            saved[std::stoi(f[1])].data[unesc(f[2])] = unesc(f[3]);
        }
        else if (f[0] == "T" && f.size() >= 7)
        {
            TaskRecord t = {std::stoi(f[2]), std::stoi(f[3]), unesc(f[5]), std::stoi(f[6])};
            saved[std::stoi(f[1])].tasks.push_back(t);
        }
    }

    // 2. Apply it to each live instance.
    for (std::size_t i = 0; i < instances.size(); ++i)
    {
        WorkflowInstance *instance = instances[i];
        std::map<int, InstanceRecord>::const_iterator it = saved.find(instance->getId());
        if (it == saved.end())
            continue; // created after the snapshot
        const InstanceRecord &record = it->second;

        instance->setStatus(record.status);
        for (DataMap::const_iterator d = record.data.begin(); d != record.data.end(); ++d)
        {
            instance->setData(d->first, d->second);
        }

        // Remove top-level tasks that were created after the snapshot.
        std::set<int> savedTopLevel;
        for (std::size_t j = 0; j < record.tasks.size(); ++j)
        {
            if (record.tasks[j].parentId == -1)
                savedTopLevel.insert(record.tasks[j].id);
        }
        std::vector<int> stale;
        const std::vector<TaskComponent *> &topLevel = instance->getTasks();
        for (std::size_t j = 0; j < topLevel.size(); ++j)
        {
            if (savedTopLevel.count(topLevel[j]->getId()) == 0)
                stale.push_back(topLevel[j]->getId());
        }
        for (std::size_t j = 0; j < stale.size(); ++j)
        {
            instance->removeTask(stale[j]);
        }

        // Put each surviving human task back into its saved status and assignee.
        for (std::size_t j = 0; j < record.tasks.size(); ++j)
        {
            const TaskRecord &savedTask = record.tasks[j];
            HumanTask *human = dynamic_cast<HumanTask *>(instance->findTask(savedTask.id));
            if (human == nullptr)
                continue; // composites take their status from their children

            human->setState(stateFromName(savedTask.status)); // swaps and deletes the old state, no notify
            if (savedTask.assignee >= 0)
            {
                registry->assign(human, savedTask.assignee);
            }
            else if (human->getAssignee() != -1)
            {
                registry->release(human);
            }
        }
    }

    // 3. Rebuild the workload counts from the restored assignments.
    std::vector<TaskComponent *> open;
    for (std::size_t i = 0; i < instances.size(); ++i)
    {
        std::vector<TaskComponent *> leaves = instances[i]->openLeafTasks();
        open.insert(open.end(), leaves.begin(), leaves.end());
    }
    registry->recountLoad(open);
}

std::string WorkflowEngine::serializeInstances() const
{
    std::ostringstream out;
    for (std::size_t i = 0; i < instances.size(); ++i)
    {
        WorkflowInstance *instance = instances[i];
        out << "I|" << instance->getId() << '|' << esc(instance->getName()) << '|'
            << esc(instance->getStatus()) << '\n';

        const DataMap &data = instance->getDataMap();
        for (DataMap::const_iterator d = data.begin(); d != data.end(); ++d)
        {
            out << "D|" << instance->getId() << '|' << esc(d->first) << '|' << esc(d->second) << '\n';
        }

        const std::vector<TaskComponent *> &tasks = instance->getTasks();
        for (std::size_t j = 0; j < tasks.size(); ++j)
        {
            writeTask(out, instance->getId(), tasks[j], -1);
        }
    }
    return out.str();
}

int WorkflowEngine::snapshotCount() const
{
    return history->count();
}