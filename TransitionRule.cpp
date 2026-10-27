#include "TransitionRule.h"

/**
 * @brief Creates one row of a workflow definition.
 * @param fromStatus The status this row applies from.
 * @param action The action that triggers the row. Also used as the task's name.
 * @param toStatus The status the instance moves to.
 * @param requiredRole The level needed for the task this row creates.
 * @param guardKey The instance data key this row is guarded by.
 * @param guardValue The value the key must have for this row to apply.
 * @param dueDays Days until the task is due. Zero means no due date. Negative means already overdue.
 * @param parallelSubtasks Names of the subtasks. Empty means one single task.
 */
TransitionRule::TransitionRule(const std::string &fromStatus, const std::string &action,
                               const std::string &toStatus, const Role &requiredRole,
                               const std::string &guardKey, const std::string &guardValue,
                               int dueDays, const std::vector<std::string> &parallelSubtasks)
    : fromStatus(fromStatus), action(action), toStatus(toStatus), requiredRole(requiredRole), guardKey(guardKey), guardValue(guardValue), dueDays(dueDays), parallelSubtasks(parallelSubtasks) {}

/**
 * @brief Returns the action that triggers this row.
 * @return The action name.
 */
std::string TransitionRule::getAction() const
{
    return action;
}

/**
 * @brief Returns the status the instance moves to.
 * @return The target status.
 */
std::string TransitionRule::getToStatus() const
{
    return toStatus;
}

/**
 * @brief Returns the status the instance moves from.
 * @return The previous status.
 */
std::string TransitionRule::getFromStatus() const
{
    return fromStatus;
}

/**
 * @brief Returns the level needed for the task this row creates.
 * @return The required role.
 */
Role TransitionRule::getRequiredRole() const
{
    return requiredRole;
}

/**
 * @brief Returns the days until the created task is due.
 * @return The number of days. Zero means no due date, negative means already overdue.
 */
int TransitionRule::getDueDays() const
{
    return dueDays;
}

/**
 * @brief Returns the names of the parallel subtasks.
 * @return A read-only reference to the names. Empty means one single task.
 */
const std::vector<std::string> &TransitionRule::getParallelSubtasks() const
{
    return parallelSubtasks;
}

bool TransitionRule::hasGuard() const
{
    return !guardKey.empty();
}

const std::string &TransitionRule::getGuardKey() const
{
    return guardKey;
}

const std::string &TransitionRule::getGuardValue() const
{
    return guardValue;
}