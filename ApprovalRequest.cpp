#include "ApprovalRequest.h"

ApprovalRequest::ApprovalRequest(int taskId, const Role& requiredRole, int requesterLevel)
    : taskId(taskId),
    requiredRole(requiredRole),
    holderLevel(requesterLevel),
    holderId(-1),
    assignedTo(-1),
    rejected(false)
{}

int ApprovalRequest::getTaskId() const
{
    return taskId;
}

int ApprovalRequest::getHolderId() const 
{
    return holderId;
}

int ApprovalRequest::getHolderLevel() const 
{
    return holderLevel;
}

void ApprovalRequest::offerTo(int participantId, int level)
{
    holderId = participantId;
    holderLevel = level;
}

void ApprovalRequest::accept(int participantId)
{
    assignedTo = participantId;
}

void ApprovalRequest::markRejected() 
{
    rejected = true;
}

bool ApprovalRequest::canBeAcceptedBy(int level) const 
{
    return level >= requiredRole.getLevel();
}

bool ApprovalRequest::isOpen() const 
{
    return assignedTo == -1 && !rejected;
}
