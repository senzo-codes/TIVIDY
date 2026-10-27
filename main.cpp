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
    Role automated(0, "Automated");
    Role handler(1, "Claims Handler");
    Role assessor(2, "Senior Assessor");
    Role manager(3, "Claims Manager");

    std::vector<std::string> noSubtasks;
    std::vector<std::string> assessSubtasks;
    assessSubtasks.push_back("Inspect vehicle");
    assessSubtasks.push_back("Estimate repair cost");

    WorkflowDefinition* InsuranceDefinition = new WorkflowDefinition("Car Insurance Claim", "Registered");

    //  FROM -> ACTION -> TO -> ROLE -> GUARDKEY -> GUARDVALUE -> DAYS -> SUBTASKS
    InsuranceDefinition->addRule(TransitionRule("Registered",          "Verify policy",    "Verified",            handler,   "",          "",         2, noSubtasks));
    InsuranceDefinition->addRule(TransitionRule("Registered",          "Reject claim",     "Rejected",            assessor,  "",          "",         2, noSubtasks));
    InsuranceDefinition->addRule(TransitionRule("Verified",            "Assess damage",    "Assessed",            assessor,  "",          "",         3, assessSubtasks));
    InsuranceDefinition->addRule(TransitionRule("Assessed",            "Auto fraud check", "Under Investigation", automated, "fraudFlag", "yes",      0, noSubtasks));
    InsuranceDefinition->addRule(TransitionRule("Assessed",            "Review claim",     "Approved",            assessor,  "",          "",         2, noSubtasks));
    InsuranceDefinition->addRule(TransitionRule("Under Investigation", "Clear claim",      "Approved",            manager,   "",          "",         5, noSubtasks));
    InsuranceDefinition->addRule(TransitionRule("Under Investigation", "Reject claim",     "Rejected",            manager,   "",          "",         5, noSubtasks));
    InsuranceDefinition->addRule(TransitionRule("Approved",            "Pay claim",        "Paid",                manager,   "",          "",         5, noSubtasks));

    return InsuranceDefinition;
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
    
    // Holder accepts it (is reassigned to them)
    engine.acceptRequest(instance->getId(), request, holderId);
    
    // Holder does it
    doTask(engine, instance, name, holderId);
}

/// @brief The asker needs a manager. The middle level cannot take it and passes it up, then the manager does it.
void requestPassUpAndDo(WorkflowEngine& engine, WorkflowInstance* instance, const std::string& name,
                        int askerId, int middleId, int topId)
{
    TaskComponent* task = findTask(instance, name, askerId);
    ApprovalRequest* request = engine.requestApproval(instance->getId(), task->getId(), askerId);
    
    // Middle level passes it up to the top level
    engine.passUpRequest(instance->getId(), request, middleId);
    
    // Top level accepts it
    engine.acceptRequest(instance->getId(), request, topId);
    
    // Top level does it
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

    // THESE ARE PEOPLE AND THIER LEVELS
    engine.addParticipant(1, "Senzo", 1, "Claims Handler");
    engine.addParticipant(2, "Liam", 2, "Senior Assessor");
    engine.addParticipant(3, "Tebello", 3, "Claims Manager");

    engine.registerDefinition(buildClaimDefinition());

    // Case 1: flagged claim. The automated fraud check moves it to Under Investigation.
    std::cout << "=== Case 1: flagged claim (approved and paid) ===\n";
    DataMap flagged;
    flagged["claimNumber"] = "CLM-2026-0001";
    flagged["policyNumber"] = "POL-88421";
    flagged["fraudFlag"] = "yes";
    WorkflowInstance* c1 = engine.startInstance("Car Insurance Claim", "Claim 1", flagged);

    // SENZO HAS LEVEL 1 AUTHORITY, SO HE DOES THIS TASK
    doTask(engine, c1, "Verify policy", 1);                    
    // Senzo gets "Assess damage" (which requires Level 2). He requests approval, Liam accepts.
    requestAndDo(engine, c1, "Inspect vehicle", 1, 2);         
    requestAndDo(engine, c1, "Estimate repair cost", 1, 2);    
    
    // Assessed: the automated check runs and moves the claim to Under Investigation
    
    // "Clear claim" requires Level 3. Senzo requests it, Liam passes it up, Tebello accepts.
    requestPassUpAndDo(engine, c1, "Clear claim", 1, 2, 3);    
    
    // "Pay claim" requires Level 3. Senzo requests, Liam passes up, Tebello accepts.
    requestPassUpAndDo(engine, c1, "Pay claim", 1, 2, 3);      
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
    // "Review claim" requires Level 2. Senzo asks, Liam accepts.
    requestAndDo(engine, c2, "Review claim", 1, 2);            
    
    // "Pay claim" requires Level 3. Senzo asks, Liam passes up, Tebello accepts.
    requestPassUpAndDo(engine, c2, "Pay claim", 1, 2, 3);      
    printAudit(c2);

    // Case 3: rejected at the start. An unauthorised attempt is refused first.
    std::cout << "=== Case 3: claim rejected at registration ===\n";
    DataMap rejected;
    rejected["claimNumber"] = "CLM-2026-0003";
    rejected["policyNumber"] = "POL-77002";
    rejected["fraudFlag"] = "no";
    WorkflowInstance* c3 = engine.startInstance("Car Insurance Claim", "Claim 3", rejected);

    try {
        // Senzo tries to do "Reject claim" (requires Level 2) without approval.
        doTask(engine, c3, "Reject claim", 1);                 
    } catch (const std::runtime_error& error) {
        std::cout << "Refused as expected: " << error.what() << "\n";
    }
    
    // Senzo asks for approval, Liam accepts and rejects.
    requestAndDo(engine, c3, "Reject claim", 1, 2);            
    printAudit(c3);

    return 0;
}