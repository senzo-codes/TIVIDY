#ifndef TIVIDY_ROLE_H
#define TIVIDY_ROLE_H

#include <string>

/**
 * @class Role
 * @brief An authority level and a display label.
 *
 * The level is what the engine compares. The name is only shown to the user.
 * Level 0 is the automated role. Levels 1 and up are human, and a higher
 * number means more authority.
 */
class Role {
public:
    /**
     * @brief Creates a role.
     * @param level The authority number. 0 is automated; higher means more authority.
     * @param name  The label shown to the user, such as "Claims Handler". Not used in comparisons.
     */
    Role(int level, const std::string& name);

    /**
     * @brief Returns the authority number.
     * @return The level. All comparisons in the engine use this value.
     */
    int getLevel() const;

    /**
     * @brief Returns the display label.
     * @return The name of the role.
     */
    std::string getName() const;

    /**
     * @brief Checks whether this is the automated role.
     * @return true if the level is 0.
     */
    bool isAutomated() const;

private:
    int level;          ///< Authority number. 0 means automated.
    std::string name;   ///< Display label. Not used in comparisons.
};

#endif // TIVIDY_ROLE_H
