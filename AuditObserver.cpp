#include "AuditObserver.h"
#include "HumanTask.h"
#include "WorkflowInstance.h"
#include "ParticipantRegistry.h"
#include "Participant.h"
#include "AuditType.h"
#include <stdexcept>

AuditObserver::AuditObserver(HumanTask* subject, WorkflowInstance* instance, ParticipantRegistry* registry)
    : TaskObserver(subject, instance), registry(registry) {
    if (this->registry == nullptr) {
        throw std::invalid_argument("Participant Registry cannot be null");
    }
}

AuditObserver::~AuditObserver() = default;


void AuditObserver::update(HumanTask& task) {
    std::string status = task.getStatus();
    AuditType eventType;
    std::string action;

    if (status == "InProgress") {
        eventType = AuditType::TaskStarted;
        action = " started";
    } else if (status == "Completed") {

        if (task.getParent() != nullptr) {
            eventType = AuditType::SubtaskDone;
        } else {
            eventType = AuditType::TaskDone;
        }
        action = " completed";

    } else if (status == "Cancelled") {
        eventType = AuditType::TaskCancelled;
        action = " cancelled";

    } else {
        return;
    }


    std::string actor;
    Participant* participant = registry->find(task.getAssignee());
    if (participant != nullptr) {
        actor = participant->getName();
    } else {
        actor = "Unassigned";
    }


    instance->logEvent(eventType, actor, "'" + task.getName() + "'" + action);
}
