#ifndef TIVIDY_WORKFLOWENGINE_H
#define TIVIDY_WORKFLOWENGINE_H

#include <map>
#include <string>
#include <vector>

#include "Types.h"
#include "PendingTaskIterator.h"

class ParticipantRegistry;
class LeastLoadedAssignment;
class RoleBasedAssignment;
class ApprovalService;
class WorkflowRunner;
class SnapshotHistory;
class WorkflowDefinition;
class WorkflowInstance;
class ApprovalRequest;

class WorkflowEngine
{
public:
    WorkflowEngine();
    ~WorkflowEngine();

    WorkflowEngine(const WorkflowEngine &) = delete;
    WorkflowEngine &operator=(const WorkflowEngine &) = delete;

    void addParticipant(int id, const std::string &name, int level, const std::string &roleName);
    void setToday(const std::string &today);
    void registerDefinition(WorkflowDefinition *definition);

    WorkflowInstance *startInstance(const std::string &definitionName, const std::string &name, const DataMap &data);
    WorkflowInstance *findInstance(int id) const;

    void startTask(int instanceId, int taskId, int participantId);
    void completeTask(int instanceId, int taskId, int participantId);
    void cancelInstance(int instanceId);
    void setPriority(int instanceId, int taskId, bool priority);

    ApprovalRequest *requestApproval(int instanceId, int taskId, int participantId);
    void acceptRequest(int instanceId, ApprovalRequest *request, int participantId);
    void passUpRequest(int instanceId, ApprovalRequest *request, int participantId);

    void saveSnapshot();
    void restoreSnapshot(int index);

    PendingTaskIterator dashboardFor(int participantId);

    // DEBUGGING
    int snapshotCount() const;

private:
    std::string serializeInstances() const;

    ParticipantRegistry *registry;
    LeastLoadedAssignment *leastLoaded;
    RoleBasedAssignment *assignment;
    ApprovalService *approvals;
    WorkflowRunner *runner;
    SnapshotHistory *history;
    std::map<std::string, WorkflowDefinition *> definitions;
    std::vector<WorkflowInstance *> instances;
    std::string today;
    int nextInstanceId;
};

#endif // TIVIDY_WORKFLOWENGINE_H