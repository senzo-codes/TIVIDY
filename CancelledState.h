#ifndef TIVIDY_CANCELLEDSTATE_H
#define TIVIDY_CANCELLEDSTATE_H

#include "TaskStates.h"
#include <string>

class HumanTask;

class CancelledState : public TaskStates {
public:
    void handle(HumanTask& task, const std::string& operation) override;
    std::string getName() const override;
    bool isClosed() const override;
};

#endif // TIVIDY_CANCELLEDSTATE_H