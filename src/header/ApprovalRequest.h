#ifndef TIVIDY_APPROVALREQUEST_H
#define TIVIDY_APPROVALREQUEST_H

#include "Role.h"

class ApprovalRequest {
public:
    ApprovalRequest(int taskId, const Role& requiredRole, int requesterLevel);
    int getTaskId() const;
    int getHolderId() const;
    int getHolderLevel() const;
    void offerTo(int participantId, int level);
    void accept(int participantId);
    void markRejected();
    bool canBeAcceptedBy(int level) const;
    bool isOpen() const;
private:
    int taskId;
    Role requiredRole;
    int holderLevel;
    int holderId;
    int assignedTo;
    bool rejected;
};

#endif // TIVIDY_APPROVALREQUEST_H
