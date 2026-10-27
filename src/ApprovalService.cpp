#include "ApprovalService.h"
#include "ParticipantRegistry.h"
#include "RoleTierHandler.h"
#include "FallbackHandler.h"
#include "ApprovalRequest.h"
#include "WorkflowInstance.h"
#include "TaskComponent.h"
#include "Participant.h"
#include "AssignmentRule.h"
#include "Errors.h"

/**
 * @brief Creates the approval service and builds the approval chain.\
 * @param registry Used to find people and to reassign tasks. Not owned.
 * @param rule     Passed to each tier handler to find someone at a level. Not owned.
 */
ApprovalService::ApprovalService(ParticipantRegistry *registry, AssignmentRule *rule)
    : registry(registry), rule(rule) {
        
}

/**
 * @brief Destroys the service.\
 */
ApprovalService::~ApprovalService()
{
}

/**
 * @brief Builds the handler chain again.
 */
void ApprovalService::rebuildChain()
{
}

/**
 * @brief Creates an approval request for a task the requester cannot act on.
 * @param instance  The instance the task belongs to, used for logging.
 * @param task      The task approval is requested for.
 * @param requester The person asking.
 * @return The new request, or nullptr if the request was refused.
 */
ApprovalRequest *ApprovalService::request(WorkflowInstance &instance, TaskComponent *task, Participant &requester)
{
    return nullptr;
}

/**
 * @brief The current holder accepts a request.
 * @param instance    The instance the task belongs to, used for logging.
 * @param request     The request being accepted.
 * @param participant The person accepting.
 */
void ApprovalService::accept(WorkflowInstance &instance, ApprovalRequest *request, Participant &participant)
{
}

/**
 * @brief The current holder passes a request up to the next level.
 * @param instance    The instance the task belongs to, used for logging.
 * @param request     The request being passed up.
 * @param participant The person passing it up.
 */
void ApprovalService::passUp(WorkflowInstance &instance, ApprovalRequest *request, Participant &participant)
{
}

/**
 * @brief Finds the open request for a task.
 * @param taskId The id of the task.
 * @return The open request for that task, or nullptr if there is none.
 */
ApprovalRequest *ApprovalService::openRequestFor(int taskId) const
{
    return nullptr;
}
