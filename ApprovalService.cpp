#include "ApprovalService.h"
#include "ParticipantRegistry.h"
#include "RoleTierHandler.h"
#include "FallbackHandler.h"
#include "ApprovalRequest.h"
#include "WorkflowInstance.h"
#include "TaskComponent.h"
#include "Participant.h"
#include "AssignmentRule.h"
#include "AuditType.h"

ApprovalService::ApprovalService(ParticipantRegistry *registry, AssignmentRule *rule)
    : registry(registry),
      rule(rule),
      chainHead(nullptr)
{
    rebuildChain();
}

ApprovalService::~ApprovalService()
{
    delete chainHead;

    for (ApprovalRequest *request : requests)
    {
        delete request;
    }
}

void ApprovalService::rebuildChain()
{
    delete chainHead;

    chainHead = nullptr;

    RequestHandler *head = nullptr;
    RequestHandler *tail = nullptr;

    int maxLevel = registry->maxRoleLevel();

    for (int level = 2; level <= maxLevel; ++level)
    {
        RoleTierHandler *tier = new RoleTierHandler(Role(level, ""), rule);

        if (head == nullptr)
        {
            head = tier;
        }
        else
        {
            tail->setSuccessor(tier);
        }
        tail = tier;
    }

    RequestHandler *fallback = new FallbackHandler();

    if (head == nullptr)
    {
        head = fallback;
    }
    else
    {
        tail->setSuccessor(fallback);
    }

    chainHead = head;
}

ApprovalRequest *ApprovalService::request(WorkflowInstance &instance, TaskComponent *task, Participant &requester)
{
    if (task->getAssignee() != requester.getId() || requester.canHandle(*task))
    {
        throw std::runtime_error("only the assignee who lacks the required level can ask for approval");
    }

    if (openRequestFor(task->getId()) != nullptr)
    {
        throw std::logic_error("an approval request is already open for this task");
    }

    ApprovalRequest *approval = new ApprovalRequest(task->getId(), task->getRequiredRole(),
                                                    requester.getRole().getLevel());
    chainHead->handle(*approval);
    requests.push_back(approval);

    if (approval->isOpen())
    {
        Participant *holder = registry->find(approval->getHolderId());
        instance.logEvent(AuditType::ApprovalRequested, requester.getName(), "offered to " + (holder != nullptr ? holder->getName() : std::string("nobody")));
    }
    else
    {
        instance.logEvent(AuditType::ApprovalRequested, requester.getName(), "no one above can take it");
        instance.logEvent(AuditType::ApprovalRejected, requester.getName(), "rejected by the fallback");
    }
    return approval;
}

void ApprovalService::accept(WorkflowInstance &instance, ApprovalRequest *request, Participant &participant) {
    if (request->getHolderId() != participant.getId()) {
        throw std::runtime_error("You are not the current holder of this request.");
    }
    if (!request->canBeAcceptedBy(participant.getRole().getLevel())) {
        throw std::runtime_error("You do not have the required authority to accept this task. You must pass it up.");
    }

    TaskComponent *task = instance.findTask(request->getTaskId());
    if (task == nullptr) {
        throw std::logic_error("The task for this request is not in the instance");
    }

    registry->assign(task, participant.getId());
    request->accept(participant.getId());

    instance.logEvent(AuditType::ApprovalAccepted, participant.getName(), "Accepted task and reassigned to " + participant.getName());
}

void ApprovalService::passUp(WorkflowInstance &instance, ApprovalRequest *request, Participant &participant)
{
    if (request->getHolderId() != participant.getId())
    {
        throw std::runtime_error("only the current holder can pass a request up");
    }

    instance.logEvent(AuditType::PassedUp, participant.getName(), "passed up to the next level");
    chainHead->handle(*request);

    if (!request->isOpen())
    {
        instance.logEvent(AuditType::ApprovalRejected, participant.getName(), "rejected by the fallback");
    }
}

ApprovalRequest *ApprovalService::openRequestFor(int taskId) const
{
    for (ApprovalRequest *request : requests)
    {
        if (request->getTaskId() == taskId && request->isOpen())
        {
            return request;
        }
    }
    return nullptr;
}
