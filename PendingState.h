#ifndef TIVIDY_PENDINGSTATE_H
#define TIVIDY_PENDINGSTATE_H

#include "TaskStates.h"
#include <string>

class HumanTask;

class PendingState : public TaskStates {
public:
    static PendingState& instance();
    void handle(HumanTask& task, const std::string& operation) override;
    std::string getName() const override;
    bool isClosed() const override;
private:
    PendingState() = default;
    PendingState(const PendingState&) = delete;
    PendingState& operator=(const PendingState&) = delete;
};

#endif // TIVIDY_PENDINGSTATE_H
