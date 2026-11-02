#ifndef TIVIDY_TASKCOMPOSITE_H
#define TIVIDY_TASKCOMPOSITE_H

#include "TaskComponent.h"
#include <vector>

/**
 * @class TaskComposite
 * @brief A group of tasks that is treated as one task (Composite pattern).
 *
 * The composite owns its children and deletes them when it is destroyed. Its
 * status is worked out from the children:
 * - no children: Pending
 * - all children cancelled: Cancelled
 * - at least one completed and every child closed: Completed
 * - any child in progress, or some completed while others are still open: InProgress
 * - otherwise: Pending
 *
 * The only operation it accepts through setStatus() is "cancel", which cancels
 * every child that is still open.
 *
 * @see TaskComponent
 */
class TaskComposite : public TaskComponent {
public:
    /**
     * @brief Creates an empty group.
     * @param id The task id.
     * @param name The task name.
     * @param requiredRole The role needed to work on the group.
     */
    TaskComposite(int id, const std::string& name, const Role& requiredRole);

    /**
     * @brief Destroys the group and every child in it.
     */
    ~TaskComposite() override;

    /**
     * @brief Adds a child and makes this group its parent.
     * @param child The child. The group takes ownership.
     * @throws std::invalid_argument If the child is null.
     */
    void add(TaskComponent* child) override;

    /**
     * @brief Removes a child and deletes it. Does nothing if the child is not in the group.
     * @param child The child to remove.
     */
    void remove(TaskComponent* child) override;

    /**
     * @brief Gets a child by position.
     * @param index The position, starting at 0.
     * @return The child.
     * @throws std::out_of_range If the index is outside the list.
     */
    TaskComponent* getChild(int index) const override;

    /**
     * @brief Gets all children.
     * @return A read-only reference to the child list.
     */
    const std::vector<TaskComponent*>& getChildren() const;

    /**
     * @brief Gets the number of children. @return The count.
     */
    int childCount() const;

    /**
     * @brief Works out the group's status from its children.
     * @return "Pending", "InProgress", "Completed" or "Cancelled".
     */
    std::string getStatus() const override;

    /**
     * @brief Checks whether the group is finished. @return true if Completed or Cancelled.
     */
    bool isClosed() const override;

    /**
     * @brief Cancels every child that is still open.
     * @param operation Must be "cancel".
     * @throws std::invalid_argument If the operation is anything else.
     */
    void setStatus(const std::string& operation) override;

    /**
     * @brief Prints the group's id, name and status, followed by each child indented.
     */
    void printTaskInfo() const override;
private:
    std::vector<TaskComponent*> children;
};

#endif // TIVIDY_TASKCOMPOSITE_H
