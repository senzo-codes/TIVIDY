#ifndef TIVIDY_TASKOBSERVER_H
#define TIVIDY_TASKOBSERVER_H

class HumanTask;
class WorkflowInstance;

/**
 * @class TaskObserver
 * @brief Observer interface for tasks (Observer pattern).
 *
 * An observer attaches itself to a HumanTask when it is created and detaches
 * itself when it is destroyed. It is told about every status change.
 *
 * @see HumanTask
 * @see AuditObserver
 * @see CompletionObserver
 */
class TaskObserver {
public:
    /**
     * @brief Creates the observer and attaches it to the task.
     * @param subject The task to watch.
     * @param instance The instance the task belongs to.
     * @throws std::invalid_argument If either pointer is null.
     */
    TaskObserver(HumanTask* subject, WorkflowInstance* instance);

    /**
     * @brief Destroys the observer and detaches it from its task.
     */
    virtual ~TaskObserver();

    /**
     * @brief Called after the watched task changes.
     * @param task The task that changed.
     */
    virtual void update(HumanTask& task) = 0;
protected:
    HumanTask* subject;
    WorkflowInstance* instance;
};

#endif // TIVIDY_TASKOBSERVER_H
