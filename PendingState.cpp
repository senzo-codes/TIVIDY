#include "PendingState.h"
#include "InProgressState.h"
#include "CancelledState.h"
#include "HumanTask.h"
#include <iostream>
#include <stdexcept>

TaskStates* PendingState::handle(HumanTask& task, const std::string& operation) {
    if (operation == "start") {
        std::cout << "[Transition] Task started. Moving from Pending to InProgress.\n";
        return new InProgressState();
    } else if (operation == "cancel") {
        std::cout << "[Transition] Task cancelled from Pending state.\n";
        return new CancelledState();
    } else {
        throw std::logic_error("[Error] Invalid operation '" + operation + "' in Pending state.");
    }
}

std::string PendingState::getName() const { return "Pending"; }
bool PendingState::isClosed() const { return false; }
