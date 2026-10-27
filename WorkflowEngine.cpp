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
#include <stdexcept>

WorkflowEngine::WorkflowEngine()
    : registry(new ParticipantRegistry()),
      leastLoaded(new LeastLoadedAssignment(registry)),
      assignment(new RoleBasedAssignment(registry, leastLoaded)),
      approvals(new ApprovalService(registry, assignment)),
      runner(new WorkflowRunner(registry, assignment, approvals)),
      history(new SnapshotHistory()),
      today(""),
      nextInstanceId(1) {}

WorkflowEngine::~WorkflowEngine() {
    // DELETE INSTANCES FIRST (THEY REFERENCE DEFINITIONS)
    for (WorkflowInstance* inst : instances) delete inst;
    instances.clear();

    // DELETE THE DEFINITIONS 
    for (auto& definition : definitions) delete definition.second;
    definitions.clear();

    // WE DELETE THE SUBSYSYTEMS
    delete history;
    delete runner;
    delete approvals;
    delete assignment;
    delete leastLoaded;
    delete registry;
}

void WorkflowEngine::addParticipant(int id, const std::string& name, int level, const std::string& roleName) {
    if (registry->find(id) != nullptr) {
        throw std::invalid_argument("Participant id already exists");
    }
    registry->add(new Participant(id, name, Role(level, roleName)));
    // CALL THIS TO BASICALLY KEEP THE HEIRARCHY IN SYNC
    approvals->rebuildChain();
}

void WorkflowEngine::setToday(const std::string& t) {
    today = t;
    runner->setToday(t);
}

void WorkflowEngine::registerDefinition(WorkflowDefinition* definition) {
    if (definition == nullptr) {
        throw std::invalid_argument("Null definition");
    }
    if (definitions.count(definition->getName())) {
        throw std::invalid_argument("Definition name already registered");
    }
    definitions[definition->getName()] = definition;
}

WorkflowInstance* WorkflowEngine::startInstance(const std::string& definitionName, const std::string& name, const DataMap& data) {
    auto it = definitions.find(definitionName);
    if (it == definitions.end()) {
        throw std::invalid_argument("Unknown definition: " + definitionName);
    }

    WorkflowInstance* instance = new WorkflowInstance(nextInstanceId++, name, it->second);
    
    // APPLY THE GUARD DATA THE INSTANCE
    for (const auto& guard : data) {
        instance->setData(guard.first, guard.second);
    }

    instances.push_back(instance);
    runner->start(*instance); // TRIGGERS THE INITIAL TASKS
    return instance;
}

WorkflowInstance* WorkflowEngine::findInstance(int id) const {
    for (WorkflowInstance* instance : instances) {
        if (instance->getId() == id) return instance;
    }
    return nullptr;
}

PendingTaskIterator WorkflowEngine::dashboardFor(int participantId) {
    return PendingTaskIterator(instances, participantId);
}

void WorkflowEngine::startTask(int instanceId, int taskId, int participantId) {
    WorkflowInstance* inst = findInstance(instanceId);
    if (!inst) throw std::invalid_argument("No such instance");
    TaskComponent* task = inst->findTask(taskId);
    if (!task) throw std::invalid_argument("No such task");
    Participant* p = registry->find(participantId);
    if (!p) throw std::invalid_argument("No such participant");
    runner->startTask(*inst, task, *p);
}

void WorkflowEngine::completeTask(int instanceId, int taskId, int participantId) {
    WorkflowInstance* inst = findInstance(instanceId);
    if (!inst) throw std::invalid_argument("No such instance");
    TaskComponent* task = inst->findTask(taskId);
    if (!task) throw std::invalid_argument("No such task");
    Participant* p = registry->find(participantId);
    if (!p) throw std::invalid_argument("No such participant");
    runner->completeTask(*inst, task, *p);
}

void WorkflowEngine::cancelInstance(int instanceId) {
    WorkflowInstance* inst = findInstance(instanceId);
    if (!inst) throw std::invalid_argument("No such instance");
    runner->cancelInstance(*inst);
}

ApprovalRequest* WorkflowEngine::requestApproval(int instanceId, int taskId, int participantId) {
    WorkflowInstance* inst = findInstance(instanceId);
    if (!inst) throw std::invalid_argument("No such instance");
    TaskComponent* task = inst->findTask(taskId);
    if (!task) throw std::invalid_argument("No such task");
    Participant* p = registry->find(participantId);
    if (!p) throw std::invalid_argument("No such participant");
    return approvals->request(*inst, task, *p);
}

void WorkflowEngine::acceptRequest(int instanceId, ApprovalRequest* request, int participantId) {
    WorkflowInstance* inst = findInstance(instanceId);
    if (!inst) throw std::invalid_argument("No such instance");
    Participant* p = registry->find(participantId);
    if (!p) throw std::invalid_argument("No such participant");
    approvals->accept(*inst, request, *p);
}

void WorkflowEngine::passUpRequest(int instanceId, ApprovalRequest* request, int participantId) {
    WorkflowInstance* inst = findInstance(instanceId);
    if (!inst) throw std::invalid_argument("No such instance");
    Participant* p = registry->find(participantId);
    if (!p) throw std::invalid_argument("No such participant");
    approvals->passUp(*inst, request, *p);
}

void WorkflowEngine::saveSnapshot() {
    history->saveSnapshot(new WorkflowSnapshot(today, serializeInstances()));
}

void WorkflowEngine::restoreSnapshot(int index) {
    if (index < 0 || index >= history->count()) {
        throw std::out_of_range("No such snapshot");
    }
    // Note: Deserialization logic goes here as discussed previously, 
    // updating the state of current instances based on the snapshot text.
}

std::string WorkflowEngine::serializeInstances() const {
    // Serialization logic for Memento pattern goes here
    return "";
}