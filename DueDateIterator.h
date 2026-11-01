#ifndef TIVIDY_DUEDATEITERATOR_H
#define TIVIDY_DUEDATEITERATOR_H

#include "Iterator.h"
#include <vector>

class TaskComponent;

/**
 * @class DueDateIterator
 * @brief Iterator that returns tasks in due-date order.
 *
 * Works on a copy of the task list. Tasks with a due date come before tasks
 * without one. Inside each group, priority tasks come first and then earlier due
 * dates come first. The tasks are not owned by the iterator.
 *
 * @implements Iterator
 * @see PriorityDecorator
 */
class DueDateIterator : public Iterator {
public:
    /**
     * @brief Creates the iterator and sorts its copy of the list.
     * @param tasks The tasks to walk through. The pointers are not owned.
     */
    explicit DueDateIterator(const std::vector<TaskComponent*>& tasks);

    /**
     * @brief Destroys the iterator.
     */
    ~DueDateIterator() override;

    /**
     * @brief Sorts the internal list into due-date order.
     */
    void sortByDate();

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

#endif // TIVIDY_DUEDATEITERATOR_H
