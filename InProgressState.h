#ifndef TIVIDY_INPROGRESSSTATE_H
#define TIVIDY_INPROGRESSSTATE_H

#include "TaskStates.h"
#include <string>

class HumanTask;

/**
 * @class InProgressState
 * @brief Represents a task that is currently being worked on.
 *
 * An in-progress task can be completed or cancelled.
 */
class InProgressState : public TaskStates
{
public:
    /**
     * @brief Applies an operation to an in-progress task and returns the next state.
     * @param task Task whose state may change.
     * @param operation Operation to apply ("complete", "cancel").
     * @return TaskStates* Pointer to the newly allocated next state object[cite: 11].
     */
    TaskStates *handle(HumanTask &task, const std::string &operation) override;

    /**
     * @brief Gets the name of this state.
     * @return "InProgress".
     */
    std::string getName() const override;

    /**
     * @brief Checks whether this state is terminal.
     * @return false, because an in-progress task is not closed.
     */
    bool isClosed() const override;
};

#endif // TIVIDY_INPROGRESSSTATE_H
