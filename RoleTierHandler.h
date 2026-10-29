#ifndef TIVIDY_ROLETIERHANDLER_H
#define TIVIDY_ROLETIERHANDLER_H

#include "RequestHandler.h"
#include "Role.h"

class AssignmentRule;

/**
 * @brief Handles one authority level in the approval chain
 *
 * If the request is still open, this level is above the current holder, and
 * someone is at this level, the request is offered to that person. Otherwise
 * the request goes to the next handler.
 */
class RoleTierHandler : public RequestHandler
{
public:
    /**
     * @brief Creates a handler for one level.
     * @param tierRole The level this handler covers.
     * @param rule     The rule used to find a person at that level. Not owned.
     */
    RoleTierHandler(const Role &tierRole, AssignmentRule *rule);

    /**
     * @brief Destroys the handler.
     */
    ~RoleTierHandler() override;

    /**
     * @brief Offers the request to someone at this level, or passes it on.
     * @param request The approval request to handle. It is changed in place.
     */
    void handle(ApprovalRequest &request) override;

private:
    Role tierRole;
    AssignmentRule *rule;
};

#endif // TIVIDY_ROLETIERHANDLER_H
