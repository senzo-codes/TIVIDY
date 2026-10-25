#ifndef TIVIDY_INPROGRESSSTATE_H
#define TIVIDY_INPROGRESSSTATE_H

#include "TaskStates.h"
#include <string>

class HumanTask;

class InProgressState : public TaskStates {
public:
    void handle(HumanTask& task, const std::string& operation) override;
    std::string getName() const override;
    bool isClosed() const override;
};

#endif // TIVIDY_INPROGRESSSTATE_H