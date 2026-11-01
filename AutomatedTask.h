#ifndef TIVIDY_AUTOMATEDTASK_H
#define TIVIDY_AUTOMATEDTASK_H

#include "TaskComponent.h"
#include <string>

/**
 * @class AutomatedTask
 * @brief A leaf task that the system performs by itself, without a person.
 *
 * An automated task has the automated role (level 0) and no assignee. It is
 * Pending until execute() is called and Completed afterwards. Its status cannot
 * be changed through setStatus().
 *
 * @see TaskComponent
 * @see AutomatedTaskCreator
 */
class AutomatedTask : public TaskComponent {
public:
    /**
     * @brief Creates a pending automated task.
     * @param id The task id.
     * @param name The task name. It is also the action name of the rule that created it.
     */
    AutomatedTask(int id, const std::string& name);

    /**
     * @brief Destroys the task.
     */
    ~AutomatedTask() override;

    /**
     * @brief Runs the automated work and marks the task as completed.
     */
    void execute();

    /**
     * @brief Gets the task status.
     * @return "Completed" after execute(); otherwise "Pending".
     */
    std::string getStatus() const override;

    /**
     * @brief Always rejected, because an automated task only changes through execute().
     * @param operation Ignored.
     * @throws std::logic_error Always.
     */
    void setStatus(const std::string& operation) override;

    /**
     * @brief Checks whether the task is finished.
     * @return true once execute() has been called.
     */
    bool isClosed() const override;

    /**
     * @brief Prints the task id, name and status to standard output.
     */
    void printTaskInfo() const override;
private:
    bool executed;
};

#endif // TIVIDY_AUTOMATEDTASK_H
