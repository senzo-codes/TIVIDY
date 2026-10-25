#include "Decorator.h"

#include <iostream>

using namespace std;

Decorator::Decorator(TaskComponent* wrapped) 
: TaskComponent(wrapped->getId(), wrapped->getName(), wrapped->getRequiredRole())
{ }

Decorator::~Decorator() { }

TaskComponent* Decorator::getWrapped() const { return wrapped; }

int Decorator::getId() const { wrapped->getId(); }

string Decorator::getName() const { wrapped->getName(); }

Role Decorator::getRequiredRole() const { wrapped->getRequiredRole(); }

string Decorator::getDueDate() const { wrapped->getDueDate(); }

string Decorator::getStatus() const { wrapped->getStatus(); }

void Decorator::setStatus(const string& operation) { wrapped->setStatus(operation); }

bool Decorator::isClosed() const { wrapped->isClosed(); }

void Decorator::printTaskInfo() const {
   //to do
}