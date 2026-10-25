#include "Strategy.h"
#include <vector>

//LeastLoadedAssignment
LeastLoadedAssignment::LeastLoadedAssignment(ParticipantRegistry* registry)
    : registry(registry) {}

// Picks the qualifying participant with the fewest open tasks (ties -> first found).
Participant* LeastLoadedAssignment::assign(const Role& role) {
    Participant* best = nullptr;
    for (Participant* p : registry->byRole(role)) {
        if (p->getRole().isAutomated()) continue;
        if (best == nullptr || registry->load(p->getId()) < registry->load(best->getId()))
            best = p;
    }
    return best;
}

//RoleBasedAssignment
RoleBasedAssignment::RoleBasedAssignment(ParticipantRegistry* registry, AssignmentRule* inner)
    : registry(registry), inner(inner) {}

// Finds the lowest role tier >= minRole that has people, then lets the
// inner rule choose within it.
Participant* RoleBasedAssignment::assign(const Role& minRole) {
    bool found = false;
    Role lowest;
    for (Participant* p : registry->byRole(minRole)) {
        if (p->getRole().isAutomated()) continue;
        if (!found || p->getRole().getLevel() < lowest.getLevel()) {
            lowest = p->getRole();
            found = true;
        }
    }
    if (!found) return nullptr;
    return inner->assign(lowest);
}