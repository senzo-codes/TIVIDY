#include "WorkflowRunner.h"
#include "WorkflowDefinition.h"
#include "src/header/WorkflowInstance.h" // For testing - can remove
#include "WorkflowInstance.h"
#include "ApprovalService.h"
#include "TaskComposite.h"
#include "HumanTask.h"
#include "AutomatedTask.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "RoleBasedAssignment.h"
#include "CompletionObserver.h"
#include "AuditObserver.h"
#include "Errors.h"

WorkflowRunner::WorkflowRunner(ParticipantRegistry *registry, RoleBasedAssignment *assignment, ApprovalService *approvals)
    : registry(registry), assignment(assignment), approvals(approvals) {
    nextTaskId = 1;
}

WorkflowRunner::~WorkflowRunner()
{
    delete automatedCreator;
}

void WorkflowRunner::setToday(const std::string &today)
{
    this->today = today;
}

void WorkflowRunner::start(WorkflowInstance &instance)
{
    // Logs of a task
    instance.logEvent(AuditType::InstanceCreated, instance.getName(), "Instance '" + instance.getName() + "' created at " + instance.getStatus());
    // Creates tasks
    
    instance.getDefinition()->rulesFrom(instance.getStatus());
}

void WorkflowRunner::startTask(WorkflowInstance &instance, TaskComponent *task, Participant &participant)
{
}

void WorkflowRunner::completeTask(WorkflowInstance &instance, TaskComponent *task, Participant &participant)
{
}

void WorkflowRunner::advance(WorkflowInstance &instance, TaskComponent *top)
{
}

void WorkflowRunner::cancelInstance(WorkflowInstance &instance)
{
}

void WorkflowRunner::spawnTasks(WorkflowInstance &instance)
{
}
