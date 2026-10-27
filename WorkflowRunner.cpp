#include "WorkflowRunner.h"
#include "WorkflowDefinition.h"
#include "WorkflowInstance.h"
#include "ApprovalService.h"
#include "TaskComposite.h"
#include "HumanTask.h"
#include "AutomatedTask.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "RoleBasedAssignment.h"
#include "AutomatedTaskCreator.h"
#include "CompletionObserver.h"
#include "AuditObserver.h"
#include "Types.h"
#include <stdexcept>
#include <ctime>
#include <cstdio>

namespace {
    std::string addDays(const std::string& ymd, int days) {
        if (ymd.empty() || days <= 0) return std::string();
        std::tm tm = {};
        if (std::sscanf(ymd.c_str(), "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) != 3)
            return std::string();
        tm.tm_year -= 1900; tm.tm_mon -= 1; tm.tm_mday += days;
        std::mktime(&tm);
        char buf[11];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
        return buf;
    }
}

WorkflowRunner::WorkflowRunner(ParticipantRegistry *registry, RoleBasedAssignment *assignment, ApprovalService *approvals)
    : registry(registry), assignment(assignment), approvals(approvals), automatedCreator(new AutomatedTaskCreator()), nextTaskId(1) {
}

WorkflowRunner::~WorkflowRunner() {
    delete automatedCreator;
}

void WorkflowRunner::setToday(const std::string &today) {
    this->today = today;
}

void WorkflowRunner::start(WorkflowInstance &instance) {
    instance.logEvent(AuditType::InstanceCreated, instance.getName(), "Instance '" + instance.getName() + "' created at " + instance.getStatus());
    spawnTasks(instance);
}

void WorkflowRunner::startTask(WorkflowInstance &instance, TaskComponent *task, Participant &participant) {
    if (task->getAssignee() != participant.getId() || !participant.canHandle(*task)) {
        instance.logEvent(AuditType::Refused, participant.getName(), "Refused to start task");
        throw std::runtime_error("AuthorityError");
    }
    task->setStatus("start");
}

void WorkflowRunner::completeTask(WorkflowInstance &instance, TaskComponent *task, Participant &participant) {
    if (task->getAssignee() != participant.getId() || !participant.canHandle(*task)) {
        instance.logEvent(AuditType::Refused, participant.getName(), "Refused to complete task");
        throw std::runtime_error("AuthorityError");
    }

    if (approvals->openRequestFor(task->getId()) != nullptr) {
        throw std::runtime_error("IllegalTransitionError");
    }

    TaskComponent *top = task->getParent() ? task->getParent() : task;

    const TransitionRule* rule = instance.getDefinition()->findRule(instance.getStatus(), top->getName(), instance.getDataMap());
    if (!rule) {
        instance.logEvent(AuditType::Refused, participant.getName(), "No matching rule found");
        return;
    }

    registry->release(task);
    task->setStatus("complete");
}

void WorkflowRunner::advance(WorkflowInstance &instance, TaskComponent *top) {
    if (top->getStatus() != "Completed") {
        return;
    }

    const TransitionRule *rule = instance.getDefinition()->findRule(instance.getStatus(), top->getName(), instance.getDataMap());
    if (!rule) {
        return;
    }

    for (TaskComponent *t : instance.getTasks()) {
        if (t != top && !t->isClosed()) {
            t->setStatus("cancel");
            instance.logEvent(AuditType::OptionCancelled, instance.getName(), "Alternative cancelled");
        }
    }

    instance.setStatus(rule->getToStatus());
    instance.logEvent(AuditType::StatusChanged, instance.getName(), "Moved to " + rule->getToStatus());
    spawnTasks(instance);
}

void WorkflowRunner::cancelInstance(WorkflowInstance &instance) {
    for (TaskComponent *t : instance.openLeafTasks()) {
        registry->release(t);
        t->setStatus("cancel");
    }
    instance.setStatus("Cancelled");
    instance.logEvent(AuditType::InstanceCancelled, instance.getName(), "Instance cancelled");
}

void WorkflowRunner::spawnTasks(WorkflowInstance &instance) {
    std::vector<const TransitionRule*> rules = instance.getDefinition()->rulesFrom(instance.getStatus(), instance.getDataMap());
    if (rules.empty()) {
        instance.logEvent(AuditType::InstanceFinished, instance.getName(), "Instance finished");
        return;
    }

    for (const TransitionRule *rule : rules) {
        Role role = rule->getRequiredRole();

        if (role.isAutomated()) {
            TaskComponent *t = automatedCreator->createTask(nextTaskId++, rule->getAction(), -1, "", role);
            instance.addTask(t);
            static_cast<AutomatedTask*>(t)->execute();
            instance.logEvent(AuditType::AutomatedExecuted, instance.getName(), "Automated task executed");
            instance.setStatus(rule->getToStatus());
            instance.logEvent(AuditType::StatusChanged, instance.getName(), "Moved to " + rule->getToStatus());
            spawnTasks(instance);
            return;
        }

        std::string due = addDays(today, rule->getDueDays());
        const std::vector<std::string> &subtasks = rule->getParallelSubtasks();

        // FIXED: ALWAYS assign to the lowest level (Level 1), just like the HTML demo.
        Role lowestRole(1, ""); 

        if (subtasks.empty()) {
            Participant *p = assignment->assign(lowestRole);
            int pId = p ? p->getId() : -1;

            TaskComponent *t = humanCreator.createTask(nextTaskId++, rule->getAction(), pId, due, role);
            instance.addTask(t);
            if (p) registry->assign(t, pId);

            HumanTask *ht = static_cast<HumanTask*>(t);
            instance.addObserver(new AuditObserver(ht, &instance, registry));
            instance.addObserver(new CompletionObserver(ht, &instance, this));
            instance.logEvent(AuditType::TaskCreated, instance.getName(), "Task created: " + rule->getAction() + " (Assigned to " + (p ? p->getName() : "Unassigned") + ")");
        } else {
            TaskComposite *comp = new TaskComposite(nextTaskId++, rule->getAction(), role);
            instance.addTask(comp);
            instance.logEvent(AuditType::TaskCreated, instance.getName(), "Composite created: " + rule->getAction());

            for (const std::string &subName : subtasks) {
                Participant *p = assignment->assign(lowestRole);
                int pId = p ? p->getId() : -1;

                TaskComponent *sub = humanCreator.createTask(nextTaskId++, subName, pId, due, role);
                comp->add(sub);
                if (p) registry->assign(sub, pId);

                HumanTask *ht = static_cast<HumanTask*>(sub);
                instance.addObserver(new AuditObserver(ht, &instance, registry));
                instance.addObserver(new CompletionObserver(ht, &instance, this));
                instance.logEvent(AuditType::SubtaskCreated, instance.getName(), "Subtask created: " + subName + " (Assigned to " + (p ? p->getName() : "Unassigned") + ")");
            }
        }
    }
}