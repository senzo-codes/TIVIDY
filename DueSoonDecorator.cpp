#include "DueSoonDecorator.h"
#include "HumanTask.h"

using namespace std;

DueSoonDecorator::DueSoonDecorator(TaskComponent* wrapped, const std::string& today, int daysThreshold = 2)
: Decorator(wrapped), today(today), daysThreshold(daysThreshold) { }

DueSoonDecorator::~DueSoonDecorator() { }

bool DueSoonDecorator::checkIfDueSoon() const {
  if (Decorator::isClosed()) return false;
  if (Decorator::getDueDate() == "") return false;  //if it has no due date
  if (Decorator::getDueDate() > to_string(daysThreshold)) return false;
  return true;
}

bool DueSoonDecorator::isOverdue() const {
  if (Decorator::isClosed()) return false;
  if (Decorator::getDueDate() == "") return false;  //if it has no due date
  if (today < Decorator::getDueDate()) return false;
  return true;
}

string DueSoonDecorator::decorationLabel() const {
  if (isOverdue()) return "overdue";
  if (checkIfDueSoon()) return "due soon";
  return "";
}