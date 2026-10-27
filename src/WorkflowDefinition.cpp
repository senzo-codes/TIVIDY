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
        if (rule.getGuardKey().empty()) {
            return &rule;
        }
        DataMap::const_iterator it = data.find(rule.getGuardKey());
        if (it != data.end() && it->second == rule.getGuardValue()) {
            return &rule;
        }
    }
    return nullptr;
}


std::vector<const TransitionRule *> WorkflowDefinition::rulesFrom(const std::string &status, const DataMap &data) const
{
    std::vector<const TransitionRule *> temp;

    for (TransitionRule trans: rules) {
        if (trans.getToStatus() == status) {
           for (TransitionRule subtrans: rules) {
                temp.push_back(&subtrans);
           }
           return temp;
        }
    }
    return temp;
}
 