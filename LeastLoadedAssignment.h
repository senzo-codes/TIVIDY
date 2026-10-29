#ifndef TIVIDY_LEASTLOADEDASSIGNMENT_H
#define TIVIDY_LEASTLOADEDASSIGNMENT_H

#include "AssignmentRule.h"

class ParticipantRegistry;

class LeastLoadedAssignment : public AssignmentRule
{
public:
    explicit LeastLoadedAssignment(ParticipantRegistry *registry);
    ~LeastLoadedAssignment() override;

    Participant *assign(const Role &role) const override;

private:
    ParticipantRegistry *registry;
};

#endif // TIVIDY_LEASTLOADEDASSIGNMENT_H
