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
    
}