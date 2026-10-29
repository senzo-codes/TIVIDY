#ifndef TIVIDY_HUMANTASK_H
#define TIVIDY_HUMANTASK_H

#include "TaskComponent.h"
#include <string>
#include <vector>

class TaskStates;
class TaskObserver;

class HumanTask : public TaskComponent
{
public:
    HumanTask(int id, const std::string &name, const Role &requiredRole,
              const std::string &dueDate, int assigneeId);
    ~HumanTask() override;

    void assign(int participantId) override;
    int getAssignee() const override;
    std::string getDueDate() const override;
    std::string getStatus() const override;
    void setStatus(const std::string &operation) override;
    bool isClosed() const override;
    void printTaskInfo() const override;

    void setState(TaskStates *state);
    bool isPriority() const;
    void setPriority(bool priority);
    void attach(TaskObserver *observer);
    void detach(TaskObserver *observer);

private:
    void notify();

    int assigneeId;
    std::string dueDate;
    TaskStates *currentState;
    bool priority;
    std::vector<TaskObserver *> observers;
};

#endif // TIVIDY_HUMANTASK_H
