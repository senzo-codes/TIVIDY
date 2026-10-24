#ifndef TIVIDY_TASKSTATES_H
#define TIVIDY_TASKSTATES_H

#include <string>

class HumanTask;

class TaskStates {
public:
    virtual ~TaskStates();
    virtual void handle(HumanTask& task, const std::string& operation) = 0;
    virtual std::string getName() const = 0;
    virtual bool isClosed() const;
};

#endif // TIVIDY_TASKSTATES_H
