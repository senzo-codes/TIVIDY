#ifndef TIVIDY_WORKFLOWSNAPSHOT_H
#define TIVIDY_WORKFLOWSNAPSHOT_H

#include <string>

class WorkflowEngine;

/**
 * @brief Memento: a sealed, immutable record of the engine's state at one moment.
 *
 * Design pattern: Memento (participant role: Memento).
 *
 * The pattern's two interfaces are enforced by the compiler:
 *  - **Wide interface (originator only).** The constructor and the saved state are
 *    private, and WorkflowEngine, the originator, is the only friend. Only the
 *    engine can create a snapshot or read the state back to restore itself.
 *  - **Narrow interface (everyone else, e.g. SnapshotHistory).** The caretaker can
 *    store, order and delete snapshots and read their timestamp, but cannot read
 *    the saved state, build a snapshot, or copy one.
 *
 * Every member is const and copying is disabled, so a snapshot cannot change after
 * it is created.
 */
class WorkflowSnapshot {
public:
    /** Public so the caretaker (SnapshotHistory) can delete the snapshots it owns. */
    ~WorkflowSnapshot() = default;

    WorkflowSnapshot(const WorkflowSnapshot&) = delete;
    WorkflowSnapshot& operator=(const WorkflowSnapshot&) = delete;

    /** @return When the snapshot was taken. Metadata only: safe for the caretaker to see. */
    std::string getTimestamp() const;

private:
    friend class WorkflowEngine;   // the originator: the only class that may create and read state

    /**
     * @param timestamp The engine's "today" when the snapshot was taken.
     * @param tasksData The engine's serialized state; its format is private to WorkflowEngine.
     */
    WorkflowSnapshot(const std::string& timestamp, const std::string& tasksData);

    /** @return The saved state, for the originator to restore from. */
    std::string getTasksData() const;

    const std::string timestamp;
    const std::string tasksData;
};

#endif // TIVIDY_WORKFLOWSNAPSHOT_H