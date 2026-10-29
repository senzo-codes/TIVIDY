#ifndef TIVIDY_COMPLETIONOBSERVER_H
#define TIVIDY_COMPLETIONOBSERVER_H

#include "TaskObserver.h"

class WorkflowRunner;

class CompletionObserver : public TaskObserver
{
public:
    CompletionObserver(HumanTask *subject, WorkflowInstance *instance, WorkflowRunner *runner);
    ~CompletionObserver() override;

    void update(HumanTask &task) override;

private:
    WorkflowRunner *runner;
};

#endif // TIVIDY_COMPLETIONOBSERVER_H
