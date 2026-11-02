#ifndef TIVIDY_ITERATOR_H
#define TIVIDY_ITERATOR_H

class TaskComponent;

/**
 * @class Iterator
 * @brief Interface for walking through a list of tasks (Iterator pattern).
 *
 * The caller does not need to know how the tasks are stored. The iterator does
 * not own the tasks it returns.
 *
 * @see DueDateIterator
 * @see PendingTaskIterator
 */
class Iterator {
public:
    /**
     * @brief Destroys the iterator through a base-class pointer.
     */
    virtual ~Iterator();

    /**
     * @brief Checks whether more tasks remain.
     * @return true if next() will return a task.
     */
    virtual bool hasNext() const = 0;

    /**
     * @brief Returns the next task and moves forward.
     * @return The next task, or nullptr when there are no more.
     */
    virtual TaskComponent* next() = 0;
};

#endif // TIVIDY_ITERATOR_H
