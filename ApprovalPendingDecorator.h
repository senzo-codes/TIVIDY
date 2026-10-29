#ifndef TIVIDY_APPROVALPENDINGDECORATOR_H
#define TIVIDY_APPROVALPENDINGDECORATOR_H

#include "Decorator.h"

class ApprovalRequest;

class ApprovalPendingDecorator : public Decorator {
public:
    ApprovalPendingDecorator(TaskComponent* wrapped, const ApprovalRequest* request);
    ~ApprovalPendingDecorator() override;

    bool isLocked() const;
    std::string decorationLabel() const override;
private:
    const ApprovalRequest* request;
};

#endif // TIVIDY_APPROVALPENDINGDECORATOR_H
