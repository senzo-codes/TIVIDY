#ifndef TIVIDY_APPROVALPENDINGDECORATOR_H
#define TIVIDY_APPROVALPENDINGDECORATOR_H

#include "Decorator.h"

class ApprovalRequest;

/**
 * @class ApprovalPendingDecorator
 * @brief Decorator that marks a task as waiting for an approval decision.
 *
 * Wraps a task together with the approval request that is open for it. While a
 * request is attached the task counts as "locked" and prints the label
 * "approval pending". The decorator does not own the wrapped task or the request.
 *
 * @see Decorator
 * @see ApprovalRequest
 */
class ApprovalPendingDecorator : public Decorator {
public:
    /**
     * @brief Wraps a task with an approval-pending marker.
     * @param wrapped The task being decorated.
     * @param request The open approval request. nullptr means nothing is pending.
     */
    ApprovalPendingDecorator(TaskComponent* wrapped, const ApprovalRequest* request);

    /**
     * @brief Destroys the decorator. The wrapped task and request are not deleted.
     */
    ~ApprovalPendingDecorator() override;

    /**
     * @brief Checks whether an approval request is pending for the task.
     * @return true if a request is attached; otherwise, false.
     */
    bool isLocked() const;

    /**
     * @brief Gets the label printed after the task information.
     * @return "approval pending" while the task is locked; otherwise an empty string.
     */
    std::string decorationLabel() const override;
private:
    const ApprovalRequest* request;
};

#endif // TIVIDY_APPROVALPENDINGDECORATOR_H
