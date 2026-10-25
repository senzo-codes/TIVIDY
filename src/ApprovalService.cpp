#include "ApprovalService.h"
#include "ParticipantRegistry.h"
#include "RoleTierHandler.h"
#include "FallbackHandler.h"
#include "ApprovalRequest.h"
#include "WorkflowInstance.h"
#include "TaskComponent.h"
#include "Participant.h"
#include "AssignmentRule.h"
#include "AuditType.h"
#include "Errors.h"

ApprovalService::ApprovalService(ParticipantRegistry *registry, AssignmentRule *rule)
    : registry(registry),
      rule(rule),
      chainHead(nullptr)
{
    rebuildChain();
}

ApprovalService::~ApprovalService()
{
    delete chainHead;

    for (ApprovalRequest *request : requests)
    {
        delete request;
    }
}

void ApprovalService::rebuildChain()
{
    delete chainHead;

    chainHead = nullptr;

    RequestHandler *head = nullptr;
    RequestHandler *tail = nullptr;

    int maxLevel = registry->maxRoleLevel();

    for (int level = 2; level <= maxLevel; ++level)
    {
        RoleTierHandler *tier = new RoleTierHandler(Role(level, ""), rule);

        if (head == nullptr)
        {
            head = tier;
        }
        else
        {
            tail->setSuccessor(tier);
        }
        tail = tier;
    }

    RequestHandler *fallback = new FallbackHandler();

    if (head == nullptr)
    {
        head = fallback;
    }
    else
    {
        tail->setSuccessor(fallback);
    }

    chainHead = head;
}