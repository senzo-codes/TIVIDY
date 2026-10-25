#ifndef TIVIDY_PENDINGSTATE_H
#define TIVIDY_PENDINGSTATE_H

#include "TaskStates.h"
#include <string>

class HumanTask;

class PendingState : public TaskStates {
public:
    void handle(HumanTask& task, const std::string& operation) override;
    std::string getName() const override;
    bool isClosed() const override;
};

#endif // TIVIDY_PENDINGSTATE_H