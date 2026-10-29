#ifndef TIVIDY_AUTOMATEDTASK_H
#define TIVIDY_AUTOMATEDTASK_H

#include "TaskComponent.h"
#include <string>

class AutomatedTask : public TaskComponent
{
public:
    AutomatedTask(int id, const std::string &name);
    ~AutomatedTask() override;

    void execute();
    std::string getStatus() const override;
    void setStatus(const std::string &operation) override;
    bool isClosed() const override;
    void printTaskInfo() const override;

private:
    bool executed;
};

#endif // TIVIDY_AUTOMATEDTASK_H
