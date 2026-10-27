#ifndef TIVIDY_DUEDATEITERATOR_H
#define TIVIDY_DUEDATEITERATOR_H

#include "Iterator.h"
#include <vector>

class TaskComponent;

class DueDateIterator : public Iterator {
public:
    explicit DueDateIterator(const std::vector<TaskComponent*>& tasks);
    ~DueDateIterator() override;

    void sortByDate();
    bool hasNext() const override;
    TaskComponent* next() override;
private:
    std::vector<TaskComponent*> items;
    std::size_t index;
};

#endif // TIVIDY_DUEDATEITERATOR_H
