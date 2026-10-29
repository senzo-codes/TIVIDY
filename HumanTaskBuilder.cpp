#include "HumanTaskBuilder.h"

/// @brief Constructor
HumanTaskBuilder::HumanTaskBuilder()
    : requiredRole(1, "")
{
    reset();
}

HumanTaskBuilder::~HumanTaskBuilder() {}

/**
 * @brief Sets the defaults: no assignee, no due date, level 1
 * @return Returns the reserted humanTaskBuilder
 */
HumanTaskBuilder &HumanTaskBuilder::reset()
{
    assigneeId = -1;
    dueDate = "";
    requiredRole = Role(1, "");
    return *this;
}

/**
 * @brief An ID setter
 * @param id The ID to set
 * @return Returns the updated human Task Builder
 */
HumanTaskBuilder &HumanTaskBuilder::setId(int id)
{
    this->id = id;
    return *this;
}

/**
 * @brief A name setter
 * @param name The name to set
 * @return Returns the updated human Task Builder
 */
HumanTaskBuilder &HumanTaskBuilder::setName(const std::string &name)
{
    this->name = name;
    return *this;
}

/**
 * @brief An assigneeId setter
 * @param assigneeId The assigneeId to set
 * @return Returns the updated human Task Builder
 */
HumanTaskBuilder &HumanTaskBuilder::setAssignee(int assigneeId)
{
    this->assigneeId = assigneeId;
    return *this;
}

/**
 * @brief A dueDate setter
 * @param dueDate The dueDate to set
 * @return Returns the updated human Task Builder
 */
HumanTaskBuilder &HumanTaskBuilder::setDueDate(const std::string &dueDate)
{
    this->dueDate = dueDate;
    return *this;
}

/**
 * @brief A role setter
 * @param role The Role to set
 * @return Returns the updated human Task Builder
 */
HumanTaskBuilder &HumanTaskBuilder::setRequiredRole(const Role &role)
{
    requiredRole = role;
    return *this;
}

/**
 * @brief A human task getter
 * @return returns a human task
 */
HumanTask *HumanTaskBuilder::getResult()
{
    return new HumanTask(id, name, requiredRole, dueDate, assigneeId);
}
