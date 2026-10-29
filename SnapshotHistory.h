#ifndef TIVIDY_SNAPSHOTHISTORY_H
#define TIVIDY_SNAPSHOTHISTORY_H

#include <vector>

class WorkflowSnapshot;

class SnapshotHistory {
public:
    SnapshotHistory();
    ~SnapshotHistory();
    SnapshotHistory(const SnapshotHistory&) = delete;
    SnapshotHistory& operator=(const SnapshotHistory&) = delete;

    void saveSnapshot(WorkflowSnapshot* snapshot);
    const WorkflowSnapshot& restoreSnapshot(int index) const;
    int count() const;
private:
    std::vector<WorkflowSnapshot*> snapshots;
};

#endif // TIVIDY_SNAPSHOTHISTORY_H
