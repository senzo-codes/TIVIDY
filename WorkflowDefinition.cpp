#include "WorkflowDefinition.h"

/**
 * @brief Creates a work flow definition
 * @param name The name of the definition
 * @param initialStatus The status a new instance starts in
 */
WorkflowDefinition::WorkflowDefinition(const std::string &name, const std::string &initialStatus)
    : name(name), initialStatus(initialStatus) {}

/**
 * @brief Adds a row
 * @details This is where each industry writes its own statuses and actions
 * @param rule One of the rows
 */
void WorkflowDefinition::addRule(const TransitionRule &rule) 
{
    rules.push_back(rule); 
}

/**
 * @brief Returns the name
 * @return name
 */
std::string WorkflowDefinition::getName() const
{
    return name;
}

/**
 * @brief Returns the initial status
 * @return initialStatus
 */
std::string WorkflowDefinition::getInitialStatus() const
{
    return initialStatus;
}

/**
 * @brief Returns the first row that matches the status, action and data, or nullptr.
 * @param status The current status of the instance.
 * @param action The action that was taken.
 * @param data   The instance's data, checked against each row's guard.
 * @return The first matching row, or nullptr if none match.
 */
const TransitionRule *WorkflowDefinition::findRule(const std::string &status, const std::string &action, const DataMap &data) const
{
    for (size_t i = 0; i < rules.size(); i++) {
        const TransitionRule &rule = rules[i];

        if (rule.getFromStatus() != status || rule.getAction() != action) {
            continue;
        }
    }
    return nullptr;
}


/**
 * @brief Returns every row that starts from the given status.
 * @param status The status to look up.
 * @param data   Currently unused.
 * @return Pointers to the matching rows, owned by the definition.
 */
std::vector<const TransitionRule *> WorkflowDefinition::rulesFrom(const std::string &status, const DataMap &data) const
{
    std::vector<const TransitionRule *> temp;

    for (size_t i = 0; i < rules.size(); i++) {
        if (rules[i].getFromStatus() == status) {
            temp.push_back(&rules[i]);
        }
    }
    return temp;
}
 
