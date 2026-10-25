#ifndef TIVIDY_APPROVALSERVICE_H
#define TIVIDY_APPROVALSERVICE_H

#include <vector>

class ParticipantRegistry;
class AssignmentRule;
class RequestHandler;
class ApprovalRequest;
class WorkflowInstance;
class TaskComponent;
class Participant;

class ApprovalService {
public:
    ApprovalService(ParticipantRegistry* registry, AssignmentRule* rule);
    ~ApprovalService();
    ApprovalService(const ApprovalService&) = delete;
    ApprovalService& operator=(const ApprovalService&) = delete;

    void rebuildChain();
    ApprovalRequest* request(WorkflowInstance& instance, TaskComponent* task, Participant& requester);
    void accept(WorkflowInstance& instance, ApprovalRequest* request, Participant& participant);
    void passUp(WorkflowInstance& instance, ApprovalRequest* request, Participant& participant);
    ApprovalRequest* openRequestFor(int taskId) const;
private:
    ParticipantRegistry* registry;
    AssignmentRule* rule;
    RequestHandler* chainHead;
    std::vector<ApprovalRequest*> requests;
};

#endif // TIVIDY_APPROVALSERVICE_H
