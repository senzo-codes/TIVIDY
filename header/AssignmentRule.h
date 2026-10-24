#ifndef TIVIDY_ASSIGNMENTRULE_H
#define TIVIDY_ASSIGNMENTRULE_H

#include "Role.h"

class Participant;

class AssignmentRule {
public:
    virtual ~AssignmentRule();
    virtual Participant* assign(const Role& role) const = 0;
};

#endif // TIVIDY_ASSIGNMENTRULE_H
