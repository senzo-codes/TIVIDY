#include <vector>
#include <string>
#include "catch.hpp"
#include "Role.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "LeastLoadedAssignment.h"
#include "RoleBasedAssignment.h"
#include "ApprovalService.h"
#include "ApprovalRequest.h"
#include "WorkflowRunner.h"
#include "HumanTask.h"
#include "TaskComposite.h"
#include "TransitionRule.h"
#include "WorkflowDefinition.h"
#include "WorkflowInstance.h"
#include "AuditEvent.h"

TEST_CASE("completeTask completes the task and moves the status", "[completeTask]") {
    ParticipantRegistry reg;
    reg.add(new Participant(1, "Ann", Role(1, "Clerk")));
    LeastLoadedAssignment least(&reg);
    RoleBasedAssignment assignment(&reg, &least);
    ApprovalService approvals(&reg, &least);
    WorkflowRunner runner(&reg, &assignment, &approvals);
    runner.setToday("2026-10-28");

    std::vector<std::string> none;
    WorkflowDefinition def("Claim", "Reported");
    def.addRule(TransitionRule("Reported", "Assess damage", "Assessed", Role(1, "Clerk"), "", "", 3, none));
    WorkflowInstance inst(1, "Claim 1", &def);
    runner.start(inst);

    TaskComponent* task = inst.getTasks()[0];
    Participant* ann = reg.find(1);

    runner.startTask(inst, task, *ann);
    runner.completeTask(inst, task, *ann);

    REQUIRE(task->getStatus() == "Completed");
    REQUIRE(inst.getStatus() == "Assessed");
    REQUIRE(reg.openTaskCount(1) == -1);
}