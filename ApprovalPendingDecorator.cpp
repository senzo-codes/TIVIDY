#include "ApprovalPendingDecorator.h"
#include "ApprovalRequest.h"

using namespace std;

ApprovalPendingDecorator::ApprovalPendingDecorator(TaskComponent* wrapped, const ApprovalRequest* request)
: Decorator(wrapped), request(request) { }

ApprovalPendingDecorator::~ApprovalPendingDecorator() { }

bool ApprovalPendingDecorator::isLocked() const {
    if (request == nullptr) return false;
    return true;
}

string ApprovalPendingDecorator::decorationLabel() const {
   if (isLocked()) { return "approval pending"; }
   return "";
}