#ifndef TIVIDY_APPROVALREQUEST_H
#define TIVIDY_APPROVALREQUEST_H

#include "Role.h"

/**
 * @brief A request for someone with more authority to take over a task.
 *
 * The request is offered to one person at a time. It is open until someone
 * accepts it or it is rejected.
 */

class ApprovalRequest
{
public:
    /**
     * @brief Creates an open request with nobody holding it yet.
     * @param taskId         The id of the task being handed up.
     * @param requiredRole   The authority the task needs.
     * @param requesterLevel The level of the person who asked. Starts as the holder level.
     */
    ApprovalRequest(int taskId, const Role &requiredRole, int requesterLevel);

    /**
     * @brief Returns the id of the task this request is about.
     * @return The task id.
     */
    int getTaskId() const;

    /**
     * @brief Returns the id of the person the request is currently offered to.
     * @return The holder's participant id, or -1 if nobody holds it
     */
    int getHolderId() const;

    /**
     * @brief Returns the authority level of the current holder.
     * @return The holder's level.
     */
    int getHolderLevel() const;

    /**
     * @brief Makes a person the current holder of the request.
     * @param participantId The id of the person to offer the request to.
     * @param level         That person's authority level.
     */
    void offerTo(int participantId, int level);

    /**
     * @brief Records that a person has accepted the request.
     * @param participantId The id of the person who accepted it.
     */
    void accept(int participantId);

    /**
     * @brief Marks the request as rejected, so nobody can take it.
     */
    void markRejected();

    /**
     * @brief Checks whether a person at a given level may accept the request.
     * @param level The authority level of the person who wants to accept.
     * @return True if the level is at least the required level.
     */
    bool canBeAcceptedBy(int level) const;

    /**
     * @brief Checks whether the request is still open.
     * @return True if nobody has accepted it and it has not been rejected.
     */
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
