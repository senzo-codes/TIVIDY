#ifndef TIVIDY_TASKSTATES_H
#define TIVIDY_TASKSTATES_H

#include <string>

class HumanTask;

/**
 * @class TaskStates
 * @brief Abstract base class for the states of a human task.
 *
 * Concrete states define which operations are valid and how a task changes
 * state in response to them.
 */
class TaskStates {
public:
    /** @brief Destroys the state object through a base-class pointer. */
    virtual ~TaskStates();

    /**
     * @brief Applies an operation to a task according to its current state and returns the next state.
     * @param task Task whose state may change.
     * @param operation Operation to apply.
     * @return TaskStates* Pointer to the newly allocated next state object.
     */
    virtual TaskStates* handle(HumanTask& task, const std::string& operation) = 0;

    /**
     * @brief Gets the name of this state.
     * @return The state's name.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Checks whether this state is terminal.
     * @return true if the task is closed; otherwise, false.
     */
    virtual bool isClosed() const;
};

#endif // TIVIDY_TASKSTATES_H
