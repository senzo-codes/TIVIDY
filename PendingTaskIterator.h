#ifndef TIVIDY_PENDINGTASKITERATOR_H
#define TIVIDY_PENDINGTASKITERATOR_H

#include "Iterator.h"
#include <vector>
#include <cstddef>

class WorkflowInstance;
class TaskComponent;

/**
 * @class PendingTaskIterator
 * @brief Iterator over one participant's pending tasks, across all instances.
 *
 * The task list is collected when the iterator is created. Only open leaf tasks
 * with the status "Pending" that are assigned to the participant are included.
 *
 * @implements Iterator
 * @see WorkflowEngine
 */
class PendingTaskIterator : public Iterator {
public:
    /**
     * @brief Collects the participant's pending tasks.
     * @param instances The workflow instances to search. Not owned.
     * @param participantId The participant whose tasks are wanted.
     */
    PendingTaskIterator(const std::vector<WorkflowInstance*>& instances, int participantId);

    /**
     * @brief Destroys the iterator.
     */
    ~PendingTaskIterator() override;

    /**
     * @brief Checks whether more tasks remain.
     * @return true if next() will return a task.
     */
    bool hasNext() const override;

    /**
     * @brief Returns the next task and moves forward.
     * @return The next task, or nullptr when the list is finished.
     */
    TaskComponent* next() override;
private:
    std::vector<TaskComponent*> items;
    std::size_t index;
};

#endif // TIVIDY_PENDINGTASKITERATOR_H
