#ifndef TIVIDY_HUMANTASK_H
#define TIVIDY_HUMANTASK_H

#include "TaskComponent.h"
#include <string>
#include <vector>

class TaskStates;
class TaskObserver;

/**
 * @class HumanTask
 * @brief A leaf task that a person works on.
 *
 * The task's lifecycle is handled by a TaskStates object (State pattern): it
 * starts Pending, can be started, completed or cancelled, and ends Completed or
 * Cancelled. The task is also the subject of the Observer pattern: every call to
 * setStatus() notifies the attached observers. It owns its current state object.
 * Observers are not owned by the task.
 *
 * @see TaskComponent
 * @see TaskStates
 * @see TaskObserver
 */
class HumanTask : public TaskComponent {
public:
    /**
     * @brief Creates a pending task.
     * @param id The task id.
     * @param name The task name.
     * @param requiredRole The role needed to work on the task.
     * @param dueDate The due date as YYYY-MM-DD. An empty string means no due date.
     * @param assigneeId The participant holding the task. -1 means unassigned.
     */
    HumanTask(int id, const std::string& name, const Role& requiredRole,
              const std::string& dueDate, int assigneeId);

    /**
     * @brief Destroys the task and its current state object.
     */
    ~HumanTask() override;

    /**
     * @brief Changes who holds the task.
     * @param participantId The new assignee. -1 means unassigned.
     */
    void assign(int participantId) override;

    /**
     * @brief Gets the holder. @return The assignee id, or -1 if unassigned.
     */
    int getAssignee() const override;

    /**
     * @brief Gets the due date. @return The date as text, or an empty string.
     */
    std::string getDueDate() const override;

    /**
     * @brief Gets the current state name.
     * @return "Pending", "InProgress", "Completed" or "Cancelled".
     */
    std::string getStatus() const override;

    /**
     * @brief Applies an operation through the current state, then notifies the observers.
     * @param operation "start", "complete" or "cancel".
     * @throws std::logic_error If the operation is not allowed in the current state.
     */
    void setStatus(const std::string& operation) override;

    /**
     * @brief Checks whether the task is finished. @return true if Completed or Cancelled.
     */
    bool isClosed() const override;

    /**
     * @brief Prints the task id, name, status, assignee and due date to standard output.
     */
    void printTaskInfo() const override;

    /**
     * @brief Replaces the current state object.
     * @param state The new state. The task takes ownership of it.
     * @throws std::invalid_argument If state is null.
     */
    void setState(TaskStates* state);

    /**
     * @brief Checks the priority flag. @return true if the task is a priority.
     */
    bool isPriority() const;

    /**
     * @brief Sets the priority flag.
     * @param priority true to mark the task as a priority.
     */
    void setPriority(bool priority);

    /**
     * @brief Adds an observer that is told about every status change.
     * @param observer The observer. Not owned.
     * @throws std::invalid_argument If the observer is null or already attached.
     */
    void attach(TaskObserver* observer);

    /**
     * @brief Removes an observer.
     * @param observer The observer to remove.
     * @throws std::invalid_argument If the observer is null.
     */
    void detach(TaskObserver* observer);
private:
    /**
     * @brief Calls update() on every attached observer, using a copy of the observer list.
     */
    void notify();

    int assigneeId;
    std::string dueDate;
    TaskStates* currentState;
    bool priority;
    std::vector<TaskObserver*> observers;
};

#endif // TIVIDY_HUMANTASK_H
