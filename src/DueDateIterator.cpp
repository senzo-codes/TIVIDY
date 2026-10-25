#include "DueDateIterator.h"
using namespace std;

DueDateIterator::DueDateIterator(const std::vector<TaskComponent*>& tasks) : items(tasks), index(0) {

    sortByDate();
}

void DueDateIterator::sortByDate(){
    for(size_t i= 0; i < items.size(); i++){
        for(size_t j = 0; j + 1 < items.size(); j++){
            if(items[j]->getDueDate() > items[j + 1]->getDueDate()){
                TaskComponent* temp = items[j];
                items[j] = items[j + 1];
                items[j + 1] = temp;
            }
        }
    }
}
DueDateIterator::~DueDateIterator() {
   
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

