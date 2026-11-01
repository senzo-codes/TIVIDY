#ifndef TIVIDY_PARTICIPANT_H
#define TIVIDY_PARTICIPANT_H

#include "Role.h"
#include <string>

class TaskComponent;

/**
 * @class Participant
 * @brief A person who can hold and work on tasks.
 *
 * A participant has an id, a name and a role. The role's level decides which
 * tasks the person may handle.
 *
 * @see Role
 * @see ParticipantRegistry
 */
class Participant {
public:
    /**
     * @brief Creates a participant.
     * @param id The unique participant id.
     * @param name The name shown to the user.
     * @param role The participant's role.
     */
    Participant(int id, const std::string& name, const Role& role);

    /**
     * @brief Gets the id. @return The participant id.
     */
    int getId() const;

    /**
     * @brief Gets the name. @return The participant's name.
     */
    std::string getName() const;

    /**
     * @brief Gets the role. @return The participant's role.
     */
    Role getRole() const;

    /**
     * @brief Checks whether the participant has enough authority for a task.
     * @param task The task to check.
     * @return true if the participant's level is at least the task's required level.
     */
    bool canHandle(const TaskComponent& task) const;
private:
    int id;
    std::string name;
    Role role;
};

#endif // TIVIDY_PARTICIPANT_H
 