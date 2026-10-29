#ifndef TIVIDY_PENDINGSTATE_H
#define TIVIDY_PENDINGSTATE_H

#include "TaskStates.h"
#include <string>

class HumanTask;

/**
 * @class PendingState
 * @brief Represents a task that has not started.
 *
 * A pending task can be started to move it to the in-progress
 * state, or cancelled to move it to the cancelled state.
 */
class PendingState : public TaskStates
{
public:
    /**
     * @brief Applies an operation to a pending task and returns the next state.
     * @param task Task whose state may change.
     * @param operation Operation to apply ("start" or "cancel").
     * @return TaskStates* Pointer to the newly allocated next state object.
     */
    TaskStates *handle(HumanTask &task, const std::string &operation) override;

    /**
     * @brief Gets the name of this state.
     * @return "Pending".
     */
    std::string getName() const override;

    /**
     * @brief Checks whether this state is terminal.
     * @return false, because a pending task is not closed.
     */
    bool isClosed() const override;
};

#endif // TIVIDY_PENDINGSTATE_H
