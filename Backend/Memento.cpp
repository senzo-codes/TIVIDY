#include "Memento.h"
#include <stdexcept>

// WorkflowSnapshot
WorkflowSnapshot::WorkflowSnapshot(const std::string& timestamp, const std::string& tasksData)
    : timestamp(timestamp), tasksData(tasksData) {}

std::string WorkflowSnapshot::getTimestamp() const { return timestamp; }
std::string WorkflowSnapshot::getTasksData() const { return tasksData; }

// SnapshotHistory
SnapshotHistory::~SnapshotHistory() {
    for (WorkflowSnapshot* s : snapshots) delete s;
}

void SnapshotHistory::saveSnapshot(const WorkflowSnapshot& snapshot) {
    snapshots.push_back(new WorkflowSnapshot(snapshot));
}

WorkflowSnapshot SnapshotHistory::restoreSnapshot(int index) {
    if (index < 0 || index >= static_cast<int>(snapshots.size()))
        throw std::out_of_range("No snapshot at that index");
    return *snapshots[index];
}

int SnapshotHistory::count() {
    return static_cast<int>(snapshots.size());
}