#include "TaskObserver.h"
#include "HumanTask.h"
#include <stdexcept>

TaskObserver::TaskObserver(HumanTask* subject, WorkflowInstance* instance) : subject(subject), instance(instance) {
    if (this->subject == nullptr) {
        throw std::invalid_argument("Observer Subject cannot be null");
    }
    if (this->instance == nullptr) {
        throw std::invalid_argument("Observer Instance cannot be null");
    }

    this->subject->attach(this);
}

TaskObserver::~TaskObserver() {
    if (this->subject != nullptr) {
        this->subject->detach(this);
    }
}
