#ifndef TIVIDY_ROLEBASEDASSIGNMENT_H
#define TIVIDY_ROLEBASEDASSIGNMENT_H

#include "AssignmentRule.h"

class ParticipantRegistry;

/**
 * @class RoleBasedAssignment
 * @brief Assignment rule that finds the people at a level and lets an inner rule choose.
 *
 * Automated participants are skipped. If nobody is at the requested level the
 * result is nullptr. It does not own the registry or the inner rule.
 *
 * @note The implementation looks for participants at exactly the requested level,
 *       using ParticipantRegistry::byRole.
 * @implements AssignmentRule
 * @see LeastLoadedAssignment
 */
class RoleBasedAssignment : public AssignmentRule {
public:
    /**
     * @brief Creates the rule.
     * @param registry The participants. Not owned.
     * @param inner The rule that picks among the people found. Not owned.
     */
    RoleBasedAssignment(ParticipantRegistry* registry, AssignmentRule* inner);

    /**
     * @brief Destroys the rule.
     */
    ~RoleBasedAssignment() override;

    /**
     * @brief Picks a participant at the requested level.
     * @param minRole The role whose level is searched.
     * @return The participant chosen by the inner rule, or nullptr if nobody is at that level.
     */
    Participant* assign(const Role& minRole) const override;
private:
    ParticipantRegistry* registry;
    AssignmentRule* inner;
};

#endif // TIVIDY_ROLEBASEDASSIGNMENT_H
