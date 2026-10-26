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

/**
 * @brief Owns the approval requests and the chain of handlers that serves them.
 *
 * A person who lacks the authority for a task asks for approval. The request
 * is passed along the chain, one level at a time, until someone accepts it or
 * the fallback rejects it.
 */
class ApprovalService
{
public:
    /**
     * @brief Creates the service and builds the handler chain for the current roster.
     * @param registry The participants. Not owned.
     * @param rule     The rule used to find a person at each level. Not owned.
     */
    ApprovalService(ParticipantRegistry *registry, AssignmentRule *rule);

    /**
     * @brief Destroys the service, its handler chain and all its requests.
     */
    ~ApprovalService();
    ApprovalService(const ApprovalService &) = delete;
    ApprovalService &operator=(const ApprovalService &) = delete;

    /**
     * @brief Rebuilds the handler chain to match the current roster.
     *
     * Creates one tier handler for each level from 2 up to the highest level in
     * use, then a fallback handler at the end. Call it whenever a person is added.
     */
    void rebuildChain();

    /**
     * @brief Creates an approval request for a task the requester cannot do, and offers it up the chain.
     * @param instance  The workflow instance the task belongs to. Used for the audit log.
     * @param task      The task the requester is asking about.
     * @param requester The person asking. Must be the task's assignee and lack the required level.
     * @return The new request. It is owned by the service.
     * @throws std::runtime_error if the requester is not the assignee, or already has the level.
     * @throws std::logic_error   if an open request already exists for the task.
     */
    ApprovalRequest *request(WorkflowInstance &instance, TaskComponent *task, Participant &requester);

    /**
     * @brief Lets the current holder accept a request, and reassigns the task to them.
     * @param instance    The workflow instance the task belongs to.
     * @param request     The request being accepted.
     * @param participant The person accepting. Must be the current holder with the required level.
     * @throws std::runtime_error if the person is not the holder or is too junior.
     * @throws std::logic_error   if the task cannot be found in the instance.
     */
    void accept(WorkflowInstance &instance, ApprovalRequest *request, Participant &participant);

    /**
     * @brief Lets the current holder pass a request to the next level up.
     * @param instance    The workflow instance the task belongs to.
     * @param request     The request being passed up.
     * @param participant The current holder. Must be the person the request is offered to.
     * @throws std::runtime_error if the person is not the current holder.
     */
    void passUp(WorkflowInstance &instance, ApprovalRequest *request, Participant &participant);

    /**
     * @brief Finds the open request for a task, if there is one.
     * @param taskId The id of the task.
     * @return The open request for that task, or nullptr if there is none.
     */
    ApprovalRequest *openRequestFor(int taskId) const;

private:
    ParticipantRegistry *registry;
    AssignmentRule *rule;
    RequestHandler *chainHead;
    std::vector<ApprovalRequest *> requests;
};

#endif // TIVIDY_APPROVALSERVICE_H
