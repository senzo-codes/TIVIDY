#include "CompletedState.h"

#include <iostream>

void CompletedState::handle(HumanTask&, const std::string&) {
    std::cout << "[Error] Task is already completed. No further operations allowed.\n";
}

std::string CompletedState::getName() const {
    return "Completed";
}

bool CompletedState::isClosed() const {
    return true;
}
