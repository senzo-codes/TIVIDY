#ifndef TIVIDY_COMPLETEDSTATE_H
#define TIVIDY_COMPLETEDSTATE_H

#include "TaskStates.h"
#include <string>

class HumanTask;

/**
 * @class CompletedState
 * @brief Represents a task that has been completed.
 *
 * Completed tasks are closed and do not accept further operations.
 */
class CompletedState : public TaskStates {
public:
    /**
     * @brief Rejects operations on a completed task.
     * @param task Completed task; its state is not changed.
     * @param operation Operation that was attempted.
     */
    void handle(HumanTask& task, const std::string& operation) override;

    /**
     * @brief Gets the name of this state.
     * @return "Completed".
     */
    std::string getName() const override;

    /**
     * @brief Checks whether this state is terminal.
     * @return true, because a completed task is closed.
     */
    bool isClosed() const override;
};

#endif // TIVIDY_COMPLETEDSTATE_H