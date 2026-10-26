#include "RoleBasedAssignment.h"
#include "ParticipantRegistry.h"
#include "Participant.h"
#include <vector>

RoleBasedAssignment::RoleBasedAssignment(ParticipantRegistry* registry, AssignmentRule* inner)
    : registry(registry), inner(inner) {}

// Does not own registry or inner (WorkflowEngine owns them).
RoleBasedAssignment::~RoleBasedAssignment() {}

// Find the lowest role tier >= minRole that actually has people, then let the
// inner rule choose within it (e.g. the least loaded person).
Participant* RoleBasedAssignment::assign(const Role& minRole) const {
    Participant* lowest = nullptr;
    std::vector<Participant*> candidates = registry->byRole(minRole);
    for (Participant* p : candidates) {
        if (p->getRole().isAutomated()) continue;
        if (lowest == nullptr || p->getRole().getLevel() < lowest->getRole().getLevel())
            lowest = p;
    }
    if (lowest == nullptr) return nullptr;

    Role tier = lowest->getRole();
    return inner->assign(tier);
}