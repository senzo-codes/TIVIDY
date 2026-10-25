#pragma once
#include "TaskStates.h"
#include <iostream>

class CancelledState : public TaskStates {
public:
    void handle(HumanTask& task, const std::string& operation) override {
        std::cout << "[Error] Task is cancelled/closed. No further operations allowed.\n";
    }

    std::string getName() const override { return "Cancelled"; }
    bool isClosed() const override { return true; }
};