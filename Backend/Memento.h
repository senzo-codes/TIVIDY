#ifndef MEMENTO_H
#define MEMENTO_H

#include <string>
#include <vector>

// WorkflowSnapshot (Memento)
class WorkflowSnapshot {
private:
    std::string timestamp;
    std::string tasksData;
public:
    WorkflowSnapshot(const std::string& timestamp, const std::string& tasksData);
    std::string getTimestamp() const;
    std::string getTasksData() const;
};

// SnapshotHistory (Caretaker)
class SnapshotHistory {
private:
    std::vector<WorkflowSnapshot*> snapshots;
public:
    SnapshotHistory() = default;
    ~SnapshotHistory();
    SnapshotHistory(const SnapshotHistory&) = delete;
    SnapshotHistory& operator=(const SnapshotHistory&) = delete;

    void saveSnapshot(const WorkflowSnapshot& snapshot);
    WorkflowSnapshot restoreSnapshot(int index);   // throws std::out_of_range on bad index
    int count();
};

#endif