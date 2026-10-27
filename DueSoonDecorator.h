#ifndef TIVIDY_DUESOONDECORATOR_H
#define TIVIDY_DUESOONDECORATOR_H

#include "Decorator.h"
#include <string>

class DueSoonDecorator : public Decorator {
public:
    DueSoonDecorator(TaskComponent* wrapped, const std::string& today, int daysThreshold);
    ~DueSoonDecorator() override;

    bool checkIfDueSoon() const;
    bool isOverdue() const;
    std::string decorationLabel() const override;
private:
    std::string today;
    int daysThreshold;
};

#endif // TIVIDY_DUESOONDECORATOR_H
