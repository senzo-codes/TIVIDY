#include "Decorator.h"

#include <iostream>

using namespace std;

Decorator::Decorator(TaskComponent *wrapped)
    : TaskComponent(wrapped->getId(), wrapped->getName(), wrapped->getRequiredRole()), wrapped(wrapped)
{
}

Decorator::~Decorator() {}

TaskComponent *Decorator::getWrapped() const { return wrapped; }

int Decorator::getId() const
{
    return wrapped->getId();
}

string Decorator::getName() const
{
    return wrapped->getName();
}

Role Decorator::getRequiredRole() const
{
    return wrapped->getRequiredRole();
}

string Decorator::getDueDate() const
{
    return wrapped->getDueDate();
}

string Decorator::getStatus() const
{
    return wrapped->getStatus();
}

void Decorator::setStatus(const string &operation)
{
    return wrapped->setStatus(operation);
}

bool Decorator::isClosed() const
{
    return wrapped->isClosed();
}

void Decorator::printTaskInfo() const
{
    wrapped->printTaskInfo();
    string label = decorationLabel();
    if (!label.empty())
    {
        cout << " [" << label << "]";
    }
    cout << "\n";
}

TaskComposite *Decorator::getParent() const
{
    return wrapped->getParent();
}

void Decorator::setParent(TaskComposite *parent)
{
    wrapped->setParent(parent);
}

int Decorator::getAssignee() const
{
    return wrapped->getAssignee();
}

void Decorator::assign(int participantId)
{
    wrapped->assign(participantId);
}

void Decorator::add(TaskComponent *child)
{
    wrapped->add(child);
}

void Decorator::remove(TaskComponent *child)
{
    wrapped->remove(child);
}

TaskComponent *Decorator::getChild(int index) const
{
    return wrapped->getChild(index);
}