#include "PendingState.h"
#include "InProgressState.h"
#include "CancelledState.h"
#include "HumanTask.h"
#include "errors.h"
#include <iostream>

void PendingState::handle(HumanTask& task, const std::string& operation) {
    if (operation == "check" || operation == "start") {
        std::cout << "[Transition] Task checked. Moving from Pending to InProgress.\n";
        task.setState(new InProgressState());
    } else if (operation == "cancel") {
        std::cout << "[Transition] Task cancelled from Pending state.\n";
        task.setState(new CancelledState());
    } else {
        std::cout << "[Error] Invalid operation '" << operation << "' in Pending state.\n";
    }
}

std::string PendingState::getName() const { return "Pending"; }
bool PendingState::isClosed() const { return false; }