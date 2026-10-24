#ifndef TIVIDY_PRIORITYDECORATOR_H
#define TIVIDY_PRIORITYDECORATOR_H

#include "Decorator.h"

class PriorityDecorator : public Decorator {
public:
    PriorityDecorator(TaskComponent* wrapped, bool priority);
    ~PriorityDecorator() override;

    std::string decorationLabel() const override;
private:
    bool priority;
};

#endif // TIVIDY_PRIORITYDECORATOR_H
