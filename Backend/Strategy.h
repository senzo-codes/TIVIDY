#ifndef STRATEGY_H
#define STRATEGY_H

#include "Role.h"
#include "Participant.h"
#include "ParticipantRegistry.h"

//AssignmentRule (Strategy interface)
class AssignmentRule {
public:
    virtual ~AssignmentRule() = default;
    virtual Participant* assign(const Role& role) = 0;
};

//LeastLoadedAssignment (Concrete Strategy)
class LeastLoadedAssignment : public AssignmentRule {
private:
    ParticipantRegistry* registry;
public:
    LeastLoadedAssignment(ParticipantRegistry* registry);
    Participant* assign(const Role& role) override;
};

// RoleBasedAssignment (Concrete Strategy)
class RoleBasedAssignment : public AssignmentRule {
private:
    ParticipantRegistry* registry;
    AssignmentRule* inner;
public:
    RoleBasedAssignment(ParticipantRegistry* registry, AssignmentRule* inner);
    Participant* assign(const Role& minRole) override;
};

#endif