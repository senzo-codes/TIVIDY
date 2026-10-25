#ifndef TIVIDY_CANCELLEDSTATE_H
#define TIVIDY_CANCELLEDSTATE_H

#include "TaskStates.h"
#include <string>

class HumanTask;

class CancelledState : public TaskStates {
public:
    static CancelledState& instance();
    void handle(HumanTask& task, const std::string& operation) override;
    std::string getName() const override;
    bool isClosed() const override;
private:
    CancelledState() = default;
    CancelledState(const CancelledState&) = delete;
    CancelledState& operator=(const CancelledState&) = delete;
};

#endif // TIVIDY_CANCELLEDSTATE_H
