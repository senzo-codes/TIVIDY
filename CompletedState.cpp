#pragma once
#include "TaskStates.h"
#include <iostream>

class CompletedState : public TaskStates {
public:
    void handle(HumanTask& task, const std::string& operation) override {
        std::cout << "[Error] Task is already completed. No further operations allowed.\n";
    }

    std::string getName() const override { return "Completed"; }
    bool isClosed() const override { return true; }
};