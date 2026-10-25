#include "TaskComponent.h"
#include <stdexcept>

using namespace std;


TaskComponent::TaskComponent(int id, const string& name, const Role& requiredRole)
: id(id), name(name), requiredRole(requiredRole) 
{
    parent = nullptr;
}

TaskComponent::~TaskComponent() { }

int TaskComponent::getId() const { return id; }

string TaskComponent::getName() const { return name; }

Role TaskComponent::getRequiredRole() const { return requiredRole; }

TaskComposite* TaskComponent::getParent() const { return parent; }

void TaskComponent::setParent(TaskComposite* parent) {
    if (this->parent != nullptr) {
        delete this->parent;
        this->parent = parent;
    } else {
        this->parent = parent;
    }
}

string TaskComponent::getDueDate() const { return ""; }

