#ifndef TIVIDY_DUESOONDECORATOR_H
#define TIVIDY_DUESOONDECORATOR_H

#include "Decorator.h"
#include <string>

/**
 * @class DueSoonDecorator
 * @brief Decorator that labels a task as "overdue" or "due soon".
 *
 * Closed tasks and tasks without a due date are never labelled.
 *
 * @todo checkIfDueSoon() currently compares the due date text with the threshold
 *       number as strings. It should compare the number of days left instead.
 * @see Decorator
 */
class DueSoonDecorator : public Decorator {
public:
    /**
     * @brief Wraps a task with a due-date label.
     * @param wrapped The task being decorated.
     * @param today The current date, written as YYYY-MM-DD.
     * @param daysThreshold How many days before the due date counts as "soon".
     */
    DueSoonDecorator(TaskComponent* wrapped, const std::string& today, int daysThreshold);

    /**
     * @brief Destroys the decorator.
     */
    ~DueSoonDecorator() override;

    /**
     * @brief Checks whether the task is due soon.
     * @return true if the task is open, has a due date and falls inside the threshold.
     */
    bool checkIfDueSoon() const;

    /**
     * @brief Checks whether the due date has been reached or passed.
     * @return true if the task is open, has a due date and today is not before it.
     */
    bool isOverdue() const;

    /**
     * @brief Gets the label for the task.
     * @return "overdue", "due soon" or an empty string.
     */
    std::string decorationLabel() const override;
private:
    std::string today;
    int daysThreshold;
};

#endif // TIVIDY_DUESOONDECORATOR_H
