#ifndef TIVIDY_PARTICIPANTREGISTRY_H
#define TIVIDY_PARTICIPANTREGISTRY_H

#include <map>
#include <vector>
#include <iostream>

class Participant;
class TaskComponent;

/**
 * @class ParticipantRegistry
 * @brief Owns all participants and counts how many open tasks each one holds.
 *
 * The registry deletes its participants when it is destroyed. It cannot be
 * copied. Task counts are kept up to date by assign() and release().
 *
 * @see Participant
 * @see AssignmentRule
 */
class ParticipantRegistry {
public:
    /**
     * @brief Creates an empty registry.
     */
    ParticipantRegistry();

    /**
     * @brief Destroys the registry and every participant in it.
     */
    ~ParticipantRegistry();
    ParticipantRegistry(const ParticipantRegistry&) = delete;
    ParticipantRegistry& operator=(const ParticipantRegistry&) = delete;

    /**
     * @brief Adds a participant.
     * @param participant The participant. The registry takes ownership.
     */
    void add(Participant* participant);

    /**
     * @brief Looks up a participant by id.
     * @param id The participant id.
     * @return The participant, or nullptr if there is none with that id.
     */
    Participant* find(int id) const;

    /**
     * @brief Gets every participant.
     * @return A read-only reference to the participant list.
     */
    const std::vector<Participant*>& all() const;

    /**
     * @brief Finds the participants at one level.
     * @param level The exact role level.
     * @return The participants whose level equals the given level.
     */
    std::vector<Participant*> byRole(int level) const;

    /**
     * @brief Finds the highest level in use.
     * @return The highest level, or 0 if there are no participants.
     */
    int maxRoleLevel() const;

    /**
     * @brief Gets how many open tasks a participant holds.
     * @param participantId The participant id.
     * @return The count, or 0 if the participant has none.
     */
    int openTaskCount(int participantId) const;

    /**
     * @brief Gives a task to a participant and updates both task counts.
     * @param task The task being handed over.
     * @param participantId The new holder.
     */
    void assign(TaskComponent* task, int participantId);

    /**
     * @brief Takes a task away from its holder and lowers that holder's count.
     * @param task The task that was completed or cancelled.
     */
    void release(TaskComponent* task);

    /**
     * @brief Recalculates every count from a list of open tasks.
     * @param openTasks The currently open tasks.
     */
    void recountLoad(const std::vector<TaskComponent*>& openTasks);
private:
    std::vector<Participant*> participants;
    std::map<int, int> load;
};

#endif // TIVIDY_PARTICIPANTREGISTRY_H
 