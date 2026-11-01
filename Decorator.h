#ifndef TIVIDY_DECORATOR_H
#define TIVIDY_DECORATOR_H

#include "TaskComponent.h"

/**
 * @class Decorator
 * @brief Base class of the task decorators (Decorator pattern).
 *
 * A decorator wraps another TaskComponent and forwards every operation to it.
 * Subclasses add a short label through decorationLabel(), which is printed after
 * the wrapped task's information. The decorator does not own the wrapped task.
 *
 * @see ApprovalPendingDecorator
 * @see DueSoonDecorator
 * @see PriorityDecorator
 */
class Decorator : public TaskComponent {
public:
    /**
     * @brief Destroys the decorator. The wrapped task is not deleted.
     */
    ~Decorator() override;

    /**
     * @brief Gets the decorated task.
     * @return The wrapped task.
     */
    TaskComponent* getWrapped() const;

    /**
     * @brief Gets the label this decoration adds.
     * @return A short label such as "priority". An empty string means no label.
     */
    virtual std::string decorationLabel() const = 0;

    /**
     * @brief Forwards to the wrapped task. @return The task id.
     */
    int getId() const override;

    /**
     * @brief Forwards to the wrapped task. @return The task name.
     */
    std::string getName() const override;

    /**
     * @brief Forwards to the wrapped task. @return The role the task needs.
     */
    Role getRequiredRole() const override;

    /**
     * @brief Forwards to the wrapped task. @return The parent composite, or nullptr.
     */
    TaskComposite* getParent() const override;

    /**
     * @brief Forwards to the wrapped task.
     * @param parent The new parent composite.
     */
    void setParent(TaskComposite* parent) override;

    /**
     * @brief Forwards to the wrapped task. @return The assignee id, or -1 if unassigned.
     */
    int getAssignee() const override;

    /**
     * @brief Forwards to the wrapped task.
     * @param participantId The new assignee.
     */
    void assign(int participantId) override;

    /**
     * @brief Forwards to the wrapped task. @return The due date, or an empty string.
     */
    std::string getDueDate() const override;

    /**
     * @brief Forwards to the wrapped task. @return The status name.
     */
    std::string getStatus() const override;

    /**
     * @brief Forwards to the wrapped task.
     * @param operation The operation to apply.
     */
    void setStatus(const std::string& operation) override;

    /**
     * @brief Forwards to the wrapped task. @return true if the task is closed.
     */
    bool isClosed() const override;

    /**
     * @brief Forwards to the wrapped task.
     * @param child The child to add.
     */
    void add(TaskComponent* child) override;

    /**
     * @brief Forwards to the wrapped task.
     * @param child The child to remove.
     */
    void remove(TaskComponent* child) override;

    /**
     * @brief Forwards to the wrapped task.
     * @param index The position of the child.
     * @return The child at that position.
     */
    TaskComponent* getChild(int index) const override;

    /**
     * @brief Prints the wrapped task's information, followed by the label in brackets if there is one.
     */
    void printTaskInfo() const override;
protected:
    /**
     * @brief Creates a decorator around a task.
     * @param wrapped The task to decorate. Not owned.
     */
    explicit Decorator(TaskComponent* wrapped);
    TaskComponent* wrapped;
};

#endif // TIVIDY_DECORATOR_H
