#include "HumanTask.h"
#include "PendingState.h"
#include "InProgressState.h"
#include "CompletedState.h"
#include "CancelledState.h"
#include "TaskObserver.h"
#include "TaskStates.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>



HumanTask::HumanTask(int id, const std::string& name, const Role& requiredRole, const std::string& dueDate, int assigneeId) : TaskComponent(id, name, requiredRole),
    assigneeId(assigneeId),
    dueDate(dueDate),
    currentState(&PendingState::instance()),
    priority(false) {
}


HumanTask::~HumanTask() = default;

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

void HumanTask::assign(int participantId) {
    assigneeId = participantId;
}


int HumanTask::getAssignee() const {
    return assigneeId;
}


std::string HumanTask::getDueDate() const {
    return dueDate;
}


std::string HumanTask::getStatus() const {
    return currentState->getName();
}

bool HumanTask::isClosed() const {
    return currentState->isClosed();
}


void HumanTask::setState(TaskStates* state) {
    if (state == nullptr) {
        throw std::invalid_argument("State cannot be null");
    }
    currentState = state;
}


bool HumanTask::isPriority() const {
    return priority;
}

void HumanTask::setPriority(bool priority) {
    this->priority = priority;
}


void HumanTask::printTaskInfo() const {
    std::cout << "Human Task ID: " << id << ", Name: " << name
              << ", Status: " << getStatus()
              << ", Assignee: ";


    if (assigneeId == -1) {
        std::cout << "Unassigned";
    } else {
        std::cout << assigneeId;
    }

    std::cout << ", Due Date: "
              << (dueDate.empty() ? "None" : dueDate)
              << '\n';
}
