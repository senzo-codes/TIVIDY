#ifndef TIVIDY_COMPLETEDSTATE_H
#define TIVIDY_COMPLETEDSTATE_H

#include "TaskStates.h"
#include <string>

class HumanTask;

class CompletedState : public TaskStates {
public:
    static CompletedState& instance();
    void handle(HumanTask& task, const std::string& operation) override;
    std::string getName() const override;
    bool isClosed() const override;
private:
    CompletedState() = default;
    CompletedState(const CompletedState&) = delete;
    CompletedState& operator=(const CompletedState&) = delete;
};

#endif // TIVIDY_COMPLETEDSTATE_H
