#include "CancelledState.h"
#include <stdexcept>

TaskStates* CancelledState::handle(HumanTask&, const std::string&) {
    throw std::logic_error("[Error] Task is cancelled/closed. No further operations allowed.");
}

std::string CancelledState::getName() const {
    return "Cancelled";
}

bool CancelledState::isClosed() const {
    return true;
}