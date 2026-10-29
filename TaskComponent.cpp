#include "TaskComponent.h"
#include <stdexcept>

using namespace std;

TaskComponent::TaskComponent(int id, const string &name, const Role &requiredRole)
    : id(id), name(name), requiredRole(requiredRole)
{
    parent = nullptr;
}

TaskComponent::~TaskComponent() {}

int TaskComponent::getId() const
{
    return id;
}

string TaskComponent::getName() const
{
    return name;
}

Role TaskComponent::getRequiredRole() const
{
    return requiredRole;
}

TaskComposite *TaskComponent::getParent() const
{
    return parent;
}

void TaskComponent::setParent(TaskComposite *parent)
{
    if (this->parent != nullptr)
    {
        this->parent = parent;
    }
    else
    {
        this->parent = parent;
    }
}

string TaskComponent::getDueDate() const
{
    return "";
}

int TaskComponent::getAssignee() const
{
    return -1;
}

void TaskComponent::assign(int)
{
    throw std::logic_error("Cannot assign a non-human task");
}

void TaskComponent::setStatus(const std::string &)
{
    throw std::logic_error("Cannot change status of this task type directly");
}

void TaskComponent::add(TaskComponent *)
{
    throw std::logic_error("Cannot add children to a leaf task");
}

void TaskComponent::remove(TaskComponent *)
{
    throw std::logic_error("Cannot remove children from a leaf task");
}

TaskComponent *TaskComponent::getChild(int) const
{
    return nullptr;
}
