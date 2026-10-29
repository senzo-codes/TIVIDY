#include "CompletionObserver.h"
#include "HumanTask.h"
#include "WorkflowInstance.h"
#include "WorkflowRunner.h"
#include "TaskComponent.h"
#include "TaskComposite.h"
#include <stdexcept>

CompletionObserver::CompletionObserver(HumanTask *subject, WorkflowInstance *instance, WorkflowRunner *runner)
    : TaskObserver(subject, instance), runner(runner)
{
    if (this->runner == nullptr)
    {
        throw std::invalid_argument("Workflow Runner cannot be null");
    }
}

CompletionObserver::~CompletionObserver() = default;

void CompletionObserver::update(HumanTask &task)
{
    if (task.getStatus() != "Completed")
    {
        return;
    }

    TaskComponent *topTask = &task;

    if (task.getParent() != nullptr)
    {
        topTask = task.getParent();
    }

    runner->advance(*instance, topTask);
}
