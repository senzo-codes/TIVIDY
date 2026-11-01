#ifndef TIVIDY_ASSIGNMENTRULE_H
#define TIVIDY_ASSIGNMENTRULE_H

#include "Role.h"

class Participant;

/**
 * @class AssignmentRule
 * @brief Strategy interface for choosing which participant receives a task.
 *
 * Each subclass implements one assignment policy. Rules can be combined, for
 * example RoleBasedAssignment hands the final choice to an inner rule.
 *
 * @see RoleBasedAssignment
 * @see LeastLoadedAssignment
 */
class AssignmentRule {
public:
    /**
     * @brief Destroys the rule through a base-class pointer.
     */
    virtual ~AssignmentRule();

    /**
     * @brief Chooses a participant for a role.
     * @param role The role (authority level) the task needs.
     * @return The chosen participant, or nullptr if nobody qualifies. The participant
     *         is owned by the ParticipantRegistry.
     */
    virtual Participant* assign(const Role& role) const = 0;
};

#endif // TIVIDY_ASSIGNMENTRULE_H
