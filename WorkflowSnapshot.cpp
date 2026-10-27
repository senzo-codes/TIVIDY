#include "WorkflowSnapshot.h"

WorkflowSnapshot::WorkflowSnapshot(const std::string& timestamp, const std::string& tasksData)
    : timestamp(timestamp), tasksData(tasksData) {}

std::string WorkflowSnapshot::getTimestamp() const { return timestamp; }
std::string WorkflowSnapshot::getTasksData() const { return tasksData; }
