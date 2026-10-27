#include "WorkflowEngine.h"
#include "ParticipantRegistry.h"
#include "LeastLoadedAssignment.h"
#include "RoleBasedAssignment.h"
#include "ApprovalService.h"
#include "WorkflowRunner.h"
#include "SnapshotHistory.h"
#include "WorkflowDefinition.h"
#include "WorkflowInstance.h"
#include "WorkflowSnapshot.h"
#include "Participant.h"
#include "Errors.h"

/// @brief Creates all the subsystems
WorkflowEngine::WorkflowEngine()
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
    delete registry;
    delete leastLoaded;
    delete assignment;
    delete approvals;
    delete runner;
    delete history;
    for (auto i = 0; i < instances.size(); ++i) {
        delete instances[i];
    }
    instances.clear();

    for (std::map<std::string, WorkflowDefinition*>::iterator it = definitions.begin(); it != definitions.end(); ++it) {
        delete it->second;
    }
    definitions.clear();
}


void WorkflowEngine::addParticipant(int id, const std::string &name, int level, const std::string &roleName)
{
    if (registry->find(id)) {
        return;
    }
    Role* role = new Role(level, roleName);

    Participant* part = new Participant(id, name, *role);
    registry->add(part);
}

void WorkflowEngine::setToday(const std::string &today)
{
    this->today = today;
}

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

WorkflowInstance *WorkflowEngine::findInstance(int id) const
{
    for (auto i = 0; i < instances.size(); ++i) {
        if (instances[i]->getId() == id) {
            return instances[i];
        }
    }
    return nullptr;
}

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

void WorkflowEngine::cancelInstance(int instanceId)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if (instance == nullptr) {
        throw std::invalid_argument("Unknown instance");
    }

    runner->cancelInstance(*instance);
}

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

void WorkflowEngine::saveSnapshot()
{
    history->saveSnapshot(new WorkflowSnapshot(today, serializeInstances()));
}

