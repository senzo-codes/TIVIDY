#ifndef TIVIDY_PRIORITYDECORATOR_H
#define TIVIDY_PRIORITYDECORATOR_H

#include "Decorator.h"

/**
 * @class PriorityDecorator
 * @brief Decorator that labels a task as a priority.
 *
 * @see Decorator
 * @see DueDateIterator
 */
class PriorityDecorator : public Decorator {
public:
    /**
     * @brief Wraps a task with a priority marker.
     * @param wrapped The task being decorated.
     * @param priority true to mark the task as a priority.
     */
    PriorityDecorator(TaskComponent* wrapped, bool priority);

    /**
     * @brief Destroys the decorator.
     */
    ~PriorityDecorator() override;

    /**
     * @brief Gets the label for the task.
     * @return "priority" if the flag is set; otherwise an empty string.
     */
    std::string decorationLabel() const override;
private:
    bool priority;
};

#endif // TIVIDY_PRIORITYDECORATOR_H
