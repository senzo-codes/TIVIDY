#ifndef TIVIDY_PENDINGTASKITERATOR_H
#define TIVIDY_PENDINGTASKITERATOR_H

#include "Iterator.h"
#include <vector>
#include <cstddef>

class WorkflowInstance;
class TaskComponent;

class PendingTaskIterator : public Iterator {
public:
    PendingTaskIterator(const std::vector<WorkflowInstance*>& instances, int participantId);
    ~PendingTaskIterator() override;

    bool hasNext() const override;
    TaskComponent* next() override;
private:
    std::vector<TaskComponent*> items;
    std::size_t index;
};

#endif // TIVIDY_PENDINGTASKITERATOR_H
