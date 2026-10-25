#ifndef TIVIDY_WORKFLOWRUNNER_H
#define TIVIDY_WORKFLOWRUNNER_H

#include "HumanTaskCreator.h"
#include <string>

class ParticipantRegistry;
class RoleBasedAssignment;
class ApprovalService;
class AutomatedTaskCreator;
class WorkflowInstance;
class TaskComponent;
class Participant;

class WorkflowRunner {
public:
    WorkflowRunner(ParticipantRegistry* registry, RoleBasedAssignment* assignment,
                   ApprovalService* approvals);
    ~WorkflowRunner();
    WorkflowRunner(const WorkflowRunner&) = delete;
    WorkflowRunner& operator=(const WorkflowRunner&) = delete;

    void setToday(const std::string& today);
    void start(WorkflowInstance& instance);
    void startTask(WorkflowInstance& instance, TaskComponent* task, Participant& participant);
    void completeTask(WorkflowInstance& instance, TaskComponent* task, Participant& participant);
    void advance(WorkflowInstance& instance, TaskComponent* top);
    void cancelInstance(WorkflowInstance& instance);
private:
    void spawnTasks(WorkflowInstance& instance);

    ParticipantRegistry* registry;
    RoleBasedAssignment* assignment;
    ApprovalService* approvals;
    HumanTaskCreator humanCreator;
    AutomatedTaskCreator* automatedCreator;
    std::string today;
    int nextTaskId;
};

#endif // TIVIDY_WORKFLOWRUNNER_H
