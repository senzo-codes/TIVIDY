#ifndef TIVIDY_LEASTLOADEDASSIGNMENT_H
#define TIVIDY_LEASTLOADEDASSIGNMENT_H

#include "AssignmentRule.h"

class ParticipantRegistry;

/**
 * @class LeastLoadedAssignment
 * @brief Assignment rule that picks the participant with the fewest open tasks.
 *
 * Only participants whose level equals the requested level are considered. When
 * two have the same load, the one with the lower id is chosen.
 *
 * @implements AssignmentRule
 * @see ParticipantRegistry
 */
class LeastLoadedAssignment : public AssignmentRule {
public:
    /**
     * @brief Creates the rule.
     * @param registry The participants and their task counts. Not owned.
     */
    explicit LeastLoadedAssignment(ParticipantRegistry* registry);

    /**
     * @brief Destroys the rule.
     */
    ~LeastLoadedAssignment() override;

    /**
     * @brief Picks the least busy participant at a level.
     * @param role The role whose level is searched.
     * @return The chosen participant, or nullptr if nobody is at that level.
     */
    Participant* assign(const Role& role) const override;
private:
    ParticipantRegistry* registry;
};

#endif // TIVIDY_LEASTLOADEDASSIGNMENT_H
