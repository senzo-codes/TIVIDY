#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "WorkflowEngine.h"
#include "WorkflowDefinition.h"
#include "TransitionRule.h"
#include "Role.h"
#include "WorkflowInstance.h"
#include "TaskComponent.h"
#include "ApprovalRequest.h"
#include "AuditEvent.h"
#include "Types.h"

namespace {

/// @brief Builds the car insurance process map from the scenario table.
WorkflowDefinition* buildClaimDefinition()
{
    Role handler(1, "Claims Handler");
    Role assessor(2, "Senior Assessor");
    Role manager(3, "Claims Manager");
    Role automated(0, "Automated");

    std::vector<std::string> noSubtasks;
    std::vector<std::string> assessSubtasks;
    assessSubtasks.push_back("Inspect vehicle");
    assessSubtasks.push_back("Estimate repair cost");

    WorkflowDefinition* def = new WorkflowDefinition("Car Insurance Claim", "Registered");

    //  from                  action              to                    role       guardKey     guardValue  days  subtasks
    def->addRule(TransitionRule("Registered",          "Verify policy",    "Verified",            handler,   "",          "",         2, noSubtasks));
    def->addRule(TransitionRule("Registered",          "Reject claim",     "Rejected",            assessor,  "",          "",         2, noSubtasks));
    def->addRule(TransitionRule("Verified",            "Assess damage",    "Assessed",            assessor,  "",          "",         3, assessSubtasks));
    def->addRule(TransitionRule("Assessed",            "Auto fraud check", "Under Investigation", automated, "fraudFlag", "yes",      0, noSubtasks));
    def->addRule(TransitionRule("Assessed",            "Review claim",     "Approved",            assessor,  "",          "",         2, noSubtasks));
    def->addRule(TransitionRule("Under Investigation", "Clear claim",      "Approved",            manager,   "",          "",         5, noSubtasks));
    def->addRule(TransitionRule("Under Investigation", "Reject claim",     "Rejected",            manager,   "",          "",         5, noSubtasks));
    def->addRule(TransitionRule("Approved",            "Pay claim",        "Paid",                manager,   "",          "",         5, noSubtasks));

    return def;
}

/// @brief Finds an open task with this name that is assigned to this person.
TaskComponent* findTask(WorkflowInstance* instance, const std::string& name, int assigneeId)
{
    std::vector<TaskComponent*> open = instance->openLeafTasks();
    for (std::size_t i = 0; i < open.size(); ++i) {
        if (open[i]->getName() == name && open[i]->getAssignee() == assigneeId) {
            return open[i];
        }
    }
    throw std::runtime_error("no open task '" + name + "' for this person");
}

/// @brief The person has the level for the task, so they start and complete it.
void doTask(WorkflowEngine& engine, WorkflowInstance* instance, const std::string& name, int personId)
{
    TaskComponent* task = findTask(instance, name, personId);
    engine.startTask(instance->getId(), task->getId(), personId);
    engine.completeTask(instance->getId(), task->getId(), personId);
}

/// @brief The person lacks the level. They ask, the holder accepts, and the holder does the task.
void requestAndDo(WorkflowEngine& engine, WorkflowInstance* instance, const std::string& name,
                  int askerId, int holderId)
{
    TaskComponent* task = findTask(instance, name, askerId);
    ApprovalRequest* request = engine.requestApproval(instance->getId(), task->getId(), askerId);
    engine.acceptRequest(instance->getId(), request, holderId);
    doTask(engine, instance, name, holderId);
}

/// @brief The asker needs a manager. The middle level cannot take it and passes it up, then the manager does it.
void requestPassUpAndDo(WorkflowEngine& engine, WorkflowInstance* instance, const std::string& name,
                        int askerId, int middleId, int topId)
{
    TaskComponent* task = findTask(instance, name, askerId);
    ApprovalRequest* request = engine.requestApproval(instance->getId(), task->getId(), askerId);
    engine.passUpRequest(instance->getId(), request, middleId);
    engine.acceptRequest(instance->getId(), request, topId);
    doTask(engine, instance, name, topId);
}

/// @brief Prints the final status and the audit log of one claim.
void printAudit(WorkflowInstance* instance)
{
    std::cout << "\nFinal status of " << instance->getName() << ": " << instance->getStatus() << "\n";
    const std::vector<AuditEvent>& log = instance->getAuditLog();
    for (std::size_t i = 0; i < log.size(); ++i) {
        std::cout << "  " << log[i].format() << "\n";
    }
    std::cout << "\n";
}

} // namespace

int main()
{
    WorkflowEngine engine;
    engine.setToday("2026-10-23");

    // People and their levels. Names are placeholders; change them in the input data file if needed.
    engine.addParticipant(1, "Sam", 1, "Claims Handler");
    engine.addParticipant(2, "Lee", 2, "Senior Assessor");
    engine.addParticipant(3, "Pat", 3, "Claims Manager");

    engine.registerDefinition(buildClaimDefinition());

    // Case 1: flagged claim. The automated fraud check moves it to Under Investigation.
    std::cout << "=== Case 1: flagged claim (approved and paid) ===\n";
    DataMap flagged;
    flagged["claimNumber"] = "CLM-2026-0001";
    flagged["policyNumber"] = "POL-88421";
    flagged["fraudFlag"] = "yes";
    WorkflowInstance* c1 = engine.startInstance("Car Insurance Claim", "Claim 1", flagged);

    doTask(engine, c1, "Verify policy", 1);                    // Sam verifies the policy
    requestAndDo(engine, c1, "Inspect vehicle", 1, 2);         // Sam asks, Lee accepts and does it
    requestAndDo(engine, c1, "Estimate repair cost", 1, 2);    // Assess damage is now complete
    // Assessed: the automated check runs and moves the claim to Under Investigation
    requestPassUpAndDo(engine, c1, "Clear claim", 1, 2, 3);    // Lee passes it up, Pat clears it
    requestPassUpAndDo(engine, c1, "Pay claim", 1, 2, 3);      // Lee passes it up, Pat pays
    printAudit(c1);

    // Case 2: claim without the fraud flag. The Senior Assessor reviews it directly.
    std::cout << "=== Case 2: claim without fraud flag (approved and paid) ===\n";
    DataMap clean;
    clean["claimNumber"] = "CLM-2026-0002";
    clean["policyNumber"] = "POL-90114";
    clean["fraudFlag"] = "no";
    WorkflowInstance* c2 = engine.startInstance("Car Insurance Claim", "Claim 2", clean);

    doTask(engine, c2, "Verify policy", 1);
    requestAndDo(engine, c2, "Inspect vehicle", 1, 2);
    requestAndDo(engine, c2, "Estimate repair cost", 1, 2);
    requestAndDo(engine, c2, "Review claim", 1, 2);            // Approved
    requestPassUpAndDo(engine, c2, "Pay claim", 1, 2, 3);      // Paid
    printAudit(c2);

    // Case 3: rejected at the start. An unauthorised attempt is refused first.
    std::cout << "=== Case 3: claim rejected at registration ===\n";
    DataMap rejected;
    rejected["claimNumber"] = "CLM-2026-0003";
    rejected["policyNumber"] = "POL-77002";
    rejected["fraudFlag"] = "no";
    WorkflowInstance* c3 = engine.startInstance("Car Insurance Claim", "Claim 3", rejected);

    try {
        doTask(engine, c3, "Reject claim", 1);                 // Sam lacks the level, so this is refused
    } catch (const std::runtime_error& error) {
        std::cout << "Refused as expected: " << error.what() << "\n";
    }
    requestAndDo(engine, c3, "Reject claim", 1, 2);            // Sam asks, Lee accepts and rejects
    printAudit(c3);

    return 0;
}