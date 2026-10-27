#include "SnapshotHistory.h"
#include "WorkflowSnapshot.h"
#include <stdexcept>

SnapshotHistory::SnapshotHistory() {}

// The history owns every snapshot handed to saveSnapshot().
SnapshotHistory::~SnapshotHistory() {
    for (WorkflowSnapshot* s : snapshots) delete s;
}

void SnapshotHistory::saveSnapshot(WorkflowSnapshot* snapshot) {
    if (snapshot != nullptr) snapshots.push_back(snapshot);
}

const WorkflowSnapshot& SnapshotHistory::restoreSnapshot(int index) const {
    if (index < 0 || index >= static_cast<int>(snapshots.size()))
        throw std::out_of_range("No snapshot at that index");
    return *snapshots[index];
}

int SnapshotHistory::count() const {
    return static_cast<int>(snapshots.size());
}
