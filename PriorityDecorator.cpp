#include "PriorityDecorator.h"
using namespace std;

PriorityDecorator::PriorityDecorator(TaskComponent* wrapped, bool priority)
: Decorator(wrapped), priority(priority) { }

PriorityDecorator::~PriorityDecorator() { }

string PriorityDecorator::decorationLabel() const {
   if (priority) return "priority";
   return "";
}