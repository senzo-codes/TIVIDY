#include "ApprovalRequest.h"

ApprovalRequest::ApprovalRequest(int taskId, const Role& requiredRole, int requesterLevel)
    : taskId(taskId),
    requiredRole(requiredRole),
    holderLevel(requesterLevel),
    holderId(-1),
    assignedTo(-1),
    rejected(false)
{}