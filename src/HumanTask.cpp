#include "HumanTask.h"
#include "PendingState.h"
#include "InProgressState.h"
#include "CompletedState.h"
#include "CancelledState.h"
#include "TaskObserver.h"
#include "TaskStates.h"
#include <stdexcept>
#include <algorithm>


void HumanTask::attach(TaskObserver* observer) {
    if (observer == nullptr) {
        throw std::invalid_argument("Observer cannot be null");
    }

    if (std::find(observers.begin(), observers.end(), observer) != observers.end()) {
        throw std::invalid_argument("Observer is already attached");
    }


    observers.push_back(observer);
}

void HumanTask::detach(TaskObserver* observer) {
    if (observer == nullptr) {
        throw std::invalid_argument("Observer cannot be null");
    }

    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}


void HumanTask::notify() {
    std::vector<TaskObserver*> observerSnapshot = observers;

    for (TaskObserver* observer : observerSnapshot) {
        observer->update(*this);
    }
}


void HumanTask::setStatus(const std::string& operation) {
    currentState->handle(*this, operation);
    notify();
}
