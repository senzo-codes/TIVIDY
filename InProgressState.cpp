#include "InProgressState.h"
#include "CompletedState.h"
#include "CancelledState.h"
#include "HumanTask.h"
#include "errors.h"
#include <iostream>
#include <stdexcept>

TaskStates* InProgressState::handle(HumanTask& task, const std::string& operation) {
    if (operation == "complete") {
        std::cout << "[Transition] Verification successful. Moving from InProgress to Completed.\n";
        return new CompletedState();
    } else if (operation == "cancel") {
        std::cout << "[Transition] Task cancelled from InProgress state.\n";
        return new CancelledState();
    } else {
        throw std::logic_error("[Error] Invalid operation '" + operation + "' in InProgress state.");
    }
}

std::string InProgressState::getName() const { return "InProgress"; }
bool InProgressState::isClosed() const { return false; }
