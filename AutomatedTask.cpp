#include "AutomatedTask.h"
#include <stdexcept>
#include <iostream>

AutomatedTask::AutomatedTask(int id, const std::string& name)
    : TaskComponent(id, name, Role(0, "Automated")), executed(false) {}

AutomatedTask::~AutomatedTask() = default;

void AutomatedTask::execute() {
    executed = true;
}


std::string AutomatedTask::getStatus() const {
    if (executed) {
        return "Completed";
    } else {
        return "Pending";
    }
}

void AutomatedTask::setStatus(const std::string&) {
    throw std::logic_error("AutomatedTask must be executed with execute()");
}


bool AutomatedTask::isClosed() const {
    return executed;
}


void AutomatedTask::printTaskInfo() const {
    std::cout << "Automated Task ID: " << id << ", Name: " << name << ", Status: " << getStatus() << '\n';
}
