#include "Participant.h"
#include "TaskComponent.h"

/**
 * @brief Contructor of the participant object
 * @param id Id assigned to the participant
 * @param name The name of the participant
 * @param role The role of the participant
 */
Participant::Participant(int id, const std::string &name, const Role &role)
    : id(id), name(name), role(role) {}

/**
 * @brief An ID getter
 * @return It returns the user's ID
 */  
int Participant::getId() const
{
    return id;
}

/**
 * @brief A name getter method
 * @return It returns the user's name
 */  
std::string Participant::getName() const
{
    return name;
}

/**
 * @brief A role getter method
 * @return It returns the user's role
 */  
Role Participant::getRole() const
{
    return role;
}

/**
 * @brief An ID getter
 * @param task Task component that the method evaluates to see if the user can handle it.
 * @return It returns the user ID
 */  
bool Participant::canHandle(const TaskComponent &task) const
{
    return (role.getLevel() >= task.getRequiredRole().getLevel()) ? true : false;
}
 
