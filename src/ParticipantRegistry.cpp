#include "ParticipantRegistry.h"
#include "Participant.h"
#include "TaskComponent.h"

/// @brief Constructor 
ParticipantRegistry::ParticipantRegistry() {}

/// @brief Destructor 
ParticipantRegistry::~ParticipantRegistry()
{
    for (Participant* part: participants) {
        delete part;
    }
    participants.clear();
}

/**
 * @brief Used to add a participant to the participants registry
 * @param participant The participant to add
 */
void ParticipantRegistry::add(Participant *participant)
{
    participants.push_back(participant);
}

/**
 * @brief Used to find the participant with the passed in ID
 * @param id Id number
 * @return It either return a participant, if matching ID is found
 * @return Or, it return nullptr
 */
Participant *ParticipantRegistry::find(int id) const
{
    for (Participant* part : participants) {
        if (part->getId() == id)
            return part;
    }

    return nullptr;
}

/**
 * @brief Returns all participants
 * @return The vector that contains all participants
 */
const std::vector<Participant *> &ParticipantRegistry::all() const
{
    return participants;
}

/**
 * @brief Returns the all roles of a passed in level
 * @param level The required level
 * @return Returns a vector of participants at a the passed in level
 */
std::vector<Participant *> ParticipantRegistry::byRole(int level) const
{
    std::vector<Participant*> vec;

    for (Participant* part : participants) {
        if (part->getRole().getLevel() == level) {
            vec.push_back(part);
        }
    }
    return vec;
}

/**
 * @brief Finding the max role level
 * @return Returns the highest level in use 
 */
int ParticipantRegistry::maxRoleLevel() const
{
    if (participants.size() == 0) { return 0; }

    int max = 0;

    for (Participant* part : participants) {
        if (part->getRole().getLevel() >= max) {
            max = part->getRole().getLevel();
        }
    }
    return max;
}

/**
 * @brief Calculates how many tasks a participant has
 * @param participantId The target participant
 * @return returns the count of tasks the individual has
 */
int ParticipantRegistry::openTaskCount(int participantId) const
{
    for (const auto& [key, value] : load) {
        if (key == participantId) 
            return value;
    }
    return 0;
}

/**
 * @brief Participants changing tasks
 * @param task A pointer to the task being re-assigned
 * @param participantId Id of the new assignee
 */
void ParticipantRegistry::assign(TaskComponent *task, int participantId)
{
    int oldHolder = task->getAssignee();

    if (oldHolder != -1) {
        load[oldHolder] -= 1;
    }

    task->assign(participantId);
    load[participantId] += 1;
}

/**
 * @brief Takes the task away from its holder and reduces that holder's count
 * @param task The task that has been completed or cancelled
 */
void ParticipantRegistry::release(TaskComponent *task)
{
    int oldHolder = task->getAssignee();

    task->assign(-1);

    if (oldHolder != -1) {
        load[oldHolder] -= 1;
    }
}

/**
 * @brief Recalculates every count from the list of open tasks.
 * @param openTasks A vector of taskComponents
 */
void ParticipantRegistry::recountLoad(const std::vector<TaskComponent *> &openTasks)
{
    for (auto i = 0; i < participants.size(); ++i) {
        load[participants[i]->getId()] = 0;
    }

    for (auto i = 0; i < openTasks.size(); ++i) {
        int who = openTasks[i]->getAssignee();

        if (who != -1) {
            load[who] += 1;
        }
    }
}
