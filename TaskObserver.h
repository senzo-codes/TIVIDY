#ifndef TIVIDY_TASKOBSERVER_H
#define TIVIDY_TASKOBSERVER_H

class HumanTask;
class WorkflowInstance;

class TaskObserver
{
public:
    TaskObserver(HumanTask *subject, WorkflowInstance *instance);
    virtual ~TaskObserver();
    virtual void update(HumanTask &task) = 0;

protected:
    HumanTask *subject;
    WorkflowInstance *instance;
};

#endif // TIVIDY_TASKOBSERVER_H
