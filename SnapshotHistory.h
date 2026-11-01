#ifndef TIVIDY_SNAPSHOTHISTORY_H
#define TIVIDY_SNAPSHOTHISTORY_H

#include <vector>

class WorkflowSnapshot;

/**
 * @class SnapshotHistory
 * @brief Keeps saved snapshots in the order they were taken (Memento caretaker).
 *
 * The history owns every snapshot it is given and cannot be copied.
 *
 * @see WorkflowSnapshot
 */
class SnapshotHistory {
public:
    /**
     * @brief Creates an empty history.
     */
    SnapshotHistory();

    /**
     * @brief Destroys the history and every snapshot in it.
     */
    ~SnapshotHistory();
    SnapshotHistory(const SnapshotHistory&) = delete;
    SnapshotHistory& operator=(const SnapshotHistory&) = delete;

    /**
     * @brief Adds a snapshot to the end of the history.
     * @param snapshot The snapshot. The history takes ownership. A null pointer is ignored.
     */
    void saveSnapshot(WorkflowSnapshot* snapshot);

    /**
     * @brief Gets a saved snapshot.
     * @param index The position, starting at 0.
     * @return A read-only reference to the snapshot.
     * @throws std::out_of_range If there is no snapshot at that index.
     */
    const WorkflowSnapshot& restoreSnapshot(int index) const;

    /**
     * @brief Gets the number of snapshots. @return The count.
     */
    int count() const;
private:
    std::vector<WorkflowSnapshot*> snapshots;
};

#endif // TIVIDY_SNAPSHOTHISTORY_H
