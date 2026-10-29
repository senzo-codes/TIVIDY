#ifndef TIVIDY_CANCELLEDSTATE_H
#define TIVIDY_CANCELLEDSTATE_H

#include "TaskStates.h"
#include <string>

class HumanTask;

/**
 * @class CancelledState
 * @brief Represents a task that has been cancelled.
 *
 * Cancelled tasks are closed and do not accept further operations.
 */
class CancelledState : public TaskStates {
public:
    /**
     * @brief Rejects operations on a cancelled task and returns the next state.
     * @param task Cancelled task; its state is not changed.
     * @param operation Operation that was attempted.
     * @return TaskStates* Pointer to the next state (typically throws an exception since cancelled states are terminal).
     */
    TaskStates* handle(HumanTask& task, const std::string& operation) override;

    /**
     * @brief Gets the name of this state.
     * @return "Cancelled".
     */
    std::string getName() const override;

    /**
     * @brief Checks whether this state is terminal.
     * @return true, because a cancelled task is closed.
     */
    bool isClosed() const override;
};

#endif // TIVIDY_CANCELLEDSTATE_H
