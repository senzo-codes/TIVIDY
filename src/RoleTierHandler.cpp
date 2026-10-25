#include "RoleTierHandler.h"
#include "AssignmentRule.h"
#include "ApprovalRequest.h"
#include "Participant.h"

RoleTierHandler::RoleTierHandler(const Role& tierRole, AssignmentRule* rule)
    : RequestHandler(), tierRole(tierRole), rule(rule)
{
}

RoleTierHandler::~RoleTierHandler(){}

void RoleTierHandler::handle(ApprovalRequest& request)
{
    if(request.isOpen() && tierRole.getLevel() > request.getHolderLevel())
    {
        Participant* canditate = rule->assign(tierRole);

        if (canditate != nullptr)
        {
            request.offerTo(canditate->getId(), tierRole.getLevel());
        }
    }

    if(successor != nullptr)
    {
        successor->handle(request);
    }
}