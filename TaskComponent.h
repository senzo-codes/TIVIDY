#ifndef TIVIDY_TASKCOMPONENT_H
#define TIVIDY_TASKCOMPONENT_H

#include "Role.h"
#include <string>

class TaskComposite;

/**
 * @class TaskComponent
 * @brief Common interface of every task (Composite pattern component).
 *
 * Both single tasks (leaves) and groups of tasks (composites) are used through
 * this interface. Operations that only make sense for one kind have a default
 * that throws, so a leaf rejects add() and remove() and a group rejects assign().
 *
 * @see HumanTask
 * @see AutomatedTask
 * @see TaskComposite
 * @see Decorator
 */
class TaskComponent {
public:
    /**
     * @brief Creates a task with no parent.
     * @param id The task id.
     * @param name The task name.
     * @param requiredRole The role needed to work on the task.
     */
    TaskComponent(int id, const std::string& name, const Role& requiredRole);

    /**
     * @brief Destroys the task through a base-class pointer.
     */
    virtual ~TaskComponent();

    /**
     * @brief Gets the id. @return The task id.
     */
    virtual int getId() const;

    /**
     * @brief Gets the name. @return The task name.
     */
    virtual std::string getName() const;

    /**
     * @brief Gets the role the task needs. @return The required role.
     */
    virtual Role getRequiredRole() const;

    /**
     * @brief Gets the group this task belongs to. @return The parent, or nullptr for a top-level task.
     */
    virtual TaskComposite* getParent() const;

    /**
     * @brief Sets the group this task belongs to.
     * @param parent The parent composite, or nullptr.
     */
    virtual void setParent(TaskComposite* parent);

    /**
     * @brief Gets who holds the task. @return The participant id, or -1 if nobody.
     */
    virtual int getAssignee() const;

    /**
     * @brief Gives the task to a participant.
     * @param participantId The new holder.
     * @throws std::logic_error If the task is not a human task.
     */
    virtual void assign(int participantId);

    /**
     * @brief Gets the due date. @return The date as text, or an empty string if there is none.
     */
    virtual std::string getDueDate() const;

    /**
     * @brief Gets the status name. @return The current status.
     */
    virtual std::string getStatus() const = 0;

    /**
     * @brief Changes the status through an operation.
     * @param operation The operation to apply, such as "start", "complete" or "cancel".
     * @throws std::logic_error If the task type does not allow it.
     */
    virtual void setStatus(const std::string& operation);

    /**
     * @brief Checks whether the task is finished. @return true if Completed or Cancelled.
     */
    virtual bool isClosed() const = 0;

    /**
     * @brief Prints the task's information to standard output.
     */
    virtual void printTaskInfo() const = 0;

    /**
     * @brief Adds a child task.
     * @param child The child to add.
     * @throws std::logic_error If the task is a leaf.
     */
    virtual void add(TaskComponent* child);

    /**
     * @brief Removes a child task.
     * @param child The child to remove.
     * @throws std::logic_error If the task is a leaf.
     */
    virtual void remove(TaskComponent* child);

    /**
     * @brief Gets a child task.
     * @param index The position of the child.
     * @return The child, or nullptr for a leaf.
     */
    virtual TaskComponent* getChild(int index) const;
protected:
    int id;
    std::string name;
    Role requiredRole;
    TaskComposite* parent;
};

#endif // TIVIDY_TASKCOMPONENT_H
