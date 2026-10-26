#include "DueDateIterator.h"
#include "HumanTask.h"
#include <string>

using namespace std;

DueDateIterator::DueDateIterator(const std::vector<TaskComponent*>& tasks)
    : items(tasks), index(0) {
    sortByDate();
}

DueDateIterator::~DueDateIterator() {}

void DueDateIterator::sortByDate() {
    for (size_t i = 0; i < items.size(); i++) {
        for (size_t j = 0; j + 1 < items.size() - i; j++) {
            TaskComponent* a = items[j];
            TaskComponent* b = items[j + 1];

            if (a == nullptr && b == nullptr) continue;

            bool swapNeeded = false;

            if (a == nullptr) {
                swapNeeded = true;
            } else if (b != nullptr) {
                string dateA = a->getDueDate();
                string dateB = b->getDueDate();

                const HumanTask* humanA = dynamic_cast<const HumanTask*>(a);
                const HumanTask* humanB = dynamic_cast<const HumanTask*>(b);
                bool prioA = (humanA != nullptr) && humanA->isPriority();
                bool prioB = (humanB != nullptr) && humanB->isPriority();

                if (dateA.empty() != dateB.empty()) {                   
                    swapNeeded = dateA.empty();
                } else if (prioA != prioB) {                   
                    swapNeeded = (!prioA && prioB);
                } else if (!dateA.empty()) {                   
                    swapNeeded = (dateA > dateB);
                }
            }

            if (swapNeeded) {
                TaskComponent* temp = items[j];
                items[j] = items[j + 1];
                items[j + 1] = temp;
            }
        }
    }
}

bool DueDateIterator::hasNext() const {
    return index < items.size();
}

TaskComponent* DueDateIterator::next() {
    if (hasNext()) {
        return items[index++];
    }
    return nullptr;
}