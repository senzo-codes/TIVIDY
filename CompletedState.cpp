#include "CompletedState.h"
#include <stdexcept>

TaskStates* CompletedState::handle(HumanTask&, const std::string&) {
    throw std::logic_error("[Error] Task is already completed. No further operations allowed.");
}

std::string CompletedState::getName() const {
    return "Completed";
}

bool CompletedState::isClosed() const {
    return true;
}
