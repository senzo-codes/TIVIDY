#include "TaskComponent.h"
#include <stdexcept>

TaskComponent::TaskComponent(int id, const std::string& name, const Role& requiredRole) : id(id), name(name), requiredRole(requiredRole), parent(nullptr) {}

TaskComponent::~TaskComponent() {}

int TaskComponent::getId() const {
    return id;
}

std::string TaskComponent::getName() const {
    return name;
}

Role TaskComponent::getRequiredRole() const {
    return requiredRole;
}

TaskComposite* TaskComponent::getParent() const {
    return parent;
}


//The actual hierarchy will be managed by the TaskComposite class with add and remove.
//setParent(nullptr) is allowed
void TaskComponent::setParent(TaskComposite* parent) {
    this->parent = parent;
}


int TaskComponent::getAssignee() const {
    return -1;
}

void TaskComponent::assign(int) {
    throw std::logic_error("Only human tasks can be assigned");
}


std::string TaskComponent::getDueDate() const {
    return "";
}


void TaskComponent::setStatus(const std::string&) {
    throw std::logic_error("Status cannot be changed for this task type");
}


void TaskComponent::add(TaskComponent*) {
    throw std::logic_error("Cannot add child to this task type");
}


void TaskComponent::remove(TaskComponent*) {
    throw std::logic_error("Cannot remove child from this task type");
}


TaskComponent* TaskComponent::getChild(int) const {
    return nullptr;
}
