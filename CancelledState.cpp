#include "CancelledState.h"

#include <iostream>

void CancelledState::handle(HumanTask&, const std::string&) {
    std::cout << "[Error] Task is cancelled/closed. No further operations allowed.\n";
}

std::string CancelledState::getName() const {
    return "Cancelled";
}

bool CancelledState::isClosed() const {
    return true;
}  