#include "Decorator.h"

#include <iostream>

using namespace std;

Decorator::Decorator(TaskComponent* wrapped) 
: TaskComponent(wrapped->getId(), wrapped->getName(), wrapped->getRequiredRole())
{ }

Decorator::~Decorator() { }

TaskComponent* Decorator::getWrapped() const { return wrapped; }

int Decorator::getId() const { return wrapped->getId(); }

string Decorator::getName() const { return wrapped->getName(); }

Role Decorator::getRequiredRole() const { return wrapped->getRequiredRole(); }

string Decorator::getDueDate() const { return wrapped->getDueDate(); }

string Decorator::getStatus() const { return wrapped->getStatus(); }

void Decorator::setStatus(const string& operation) { return wrapped->setStatus(operation); }

bool Decorator::isClosed() const { return wrapped->isClosed(); }

void Decorator::printTaskInfo() const {
   wrapped->printTaskInfo();
   string label = decorationLabel();
   if (!label.empty()) {
       cout << " [" << label << "]";
   }
   cout << "\n";
}
