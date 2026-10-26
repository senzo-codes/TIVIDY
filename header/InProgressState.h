#ifndef TIVIDY_INPROGRESSSTATE_H
#define TIVIDY_INPROGRESSSTATE_H

#include "TaskStates.h"
#include <string>

class HumanTask;

class InProgressState : public TaskStates {
public:
    static InProgressState& instance();
    void handle(HumanTask& task, const std::string& operation) override;
    std::string getName() const override;
    bool isClosed() const override;
private:
    InProgressState() = default;
    InProgressState(const InProgressState&) = delete;
    InProgressState& operator=(const InProgressState&) = delete;
};

#endif // TIVIDY_INPROGRESSSTATE_H
