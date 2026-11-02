#ifndef TIVIDY_COMPLETIONOBSERVER_H
#define TIVIDY_COMPLETIONOBSERVER_H

#include "TaskObserver.h"

class WorkflowRunner;

/**
 * @class CompletionObserver
 * @brief Observer that moves a workflow forward when a watched task completes.
 *
 * When the task it watches reaches Completed, the observer asks the
 * WorkflowRunner to advance the instance. If the task is a subtask, the advance
 * is requested for its parent, so the stage only moves on when the whole group
 * is finished.
 *
 * @implements TaskObserver
 * @see WorkflowRunner
 */
class CompletionObserver : public TaskObserver {
public:
    /**
     * @brief Creates the observer and attaches it to the task.
     * @param subject The task to watch.
     * @param instance The instance the task belongs to.
     * @param runner The runner that advances the instance. Not owned.
     * @throws std::invalid_argument If any pointer is null.
     */
    CompletionObserver(HumanTask* subject, WorkflowInstance* instance, WorkflowRunner* runner);

    /**
     * @brief Destroys the observer and detaches it from its task.
     */
    ~CompletionObserver() override;

    /**
     * @brief Reacts to a change in the watched task.
     * @param task The task that changed. Nothing happens unless it is Completed.
     */
    void update(HumanTask& task) override;
private:
    WorkflowRunner* runner;
};

#endif // TIVIDY_COMPLETIONOBSERVER_H
