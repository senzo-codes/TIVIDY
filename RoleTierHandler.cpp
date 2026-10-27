#include "RoleTierHandler.h"
#include "AssignmentRule.h"
#include "ApprovalRequest.h"
#include "Participant.h"

RoleTierHandler::RoleTierHandler(const Role& tierRole, AssignmentRule* rule)
    : RequestHandler(), tierRole(tierRole), rule(rule) {}

RoleTierHandler::~RoleTierHandler() {}

void RoleTierHandler::handle(ApprovalRequest& request) {
    // If the request is open, this level is strictly higher than the current holder,
    // and someone exists at this level, offer it to them.
    if (request.isOpen() && tierRole.getLevel() > request.getHolderLevel()) {
        Participant* candidate = rule->assign(tierRole);
        if (candidate != nullptr) {
            request.offerTo(candidate->getId(), tierRole.getLevel());
            return; // We offered it, stop passing up for now.
        }
    }

    // Otherwise, pass it to the next handler
    if (successor != nullptr) {
        successor->handle(request);
    }
}