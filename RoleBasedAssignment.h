#ifndef TIVIDY_ROLEBASEDASSIGNMENT_H
#define TIVIDY_ROLEBASEDASSIGNMENT_H

#include "AssignmentRule.h"

class ParticipantRegistry;

class RoleBasedAssignment : public AssignmentRule {
public:
    RoleBasedAssignment(ParticipantRegistry* registry, AssignmentRule* inner);
    ~RoleBasedAssignment() override;

    Participant* assign(const Role& minRole) const override;
private:
    ParticipantRegistry* registry;
    AssignmentRule* inner;
};

#endif // TIVIDY_ROLEBASEDASSIGNMENT_H
