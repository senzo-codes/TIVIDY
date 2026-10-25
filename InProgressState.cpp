#include "InProgressState.h"
#include "CompletedState.h"
#include "CancelledState.h"
#include "HumanTask.h"
#include "errors.h"
#include <iostream>

void InProgressState::handle(HumanTask& task, const std::string& operation) {
    if (operation == "complete" || operation == "approve") {
        std::cout << "[Transition] Verification successful. Moving from InProgress to Completed.\n";
        task.setState(new CompletedState());
    } else if (operation == "cancel" || operation == "reject") {
        std::cout << "[Transition] Task rejected/cancelled from InProgress state.\n";
        task.setState(new CancelledState());
    } else {
        std::cout << "[Error] Invalid operation '" << operation << "' in InProgress state.\n";
    }
}

std::string InProgressState::getName() const { return "InProgress"; }
bool InProgressState::isClosed() const { return false; }