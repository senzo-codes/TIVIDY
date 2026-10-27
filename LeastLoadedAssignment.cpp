#include "LeastLoadedAssignment.h"
#include "ParticipantRegistry.h"
#include "Participant.h"
#include <vector>

namespace {
// Single place to adapt if your ParticipantRegistry names its open-task count differently.
int loadOf(ParticipantRegistry* registry, const Participant* p) {
    return registry->load(p->getId());
    }
}

LeastLoadedAssignment::LeastLoadedAssignment(ParticipantRegistry* registry)
    : registry(registry) {}

LeastLoadedAssignment::~LeastLoadedAssignment() {}

// Among everyone whose role level >= role, pick the one with the fewest open
// tasks (ties -> first found). Automated participants are skipped.
Participant* LeastLoadedAssignment::assign(const Role& role) const {
    Participant* best = nullptr;
    std::vector<Participant*> candidates = registry->byRole(role);
    for (Participant* p : candidates) {
        if (p->getRole().isAutomated()) continue;
        if (best == nullptr || loadOf(registry, p) < loadOf(registry, best))
            best = p;
    }
    return best;
}
