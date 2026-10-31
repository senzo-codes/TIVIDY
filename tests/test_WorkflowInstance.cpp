#include <vector>
#include "catch.hpp"
#include "Role.h"
#include "HumanTask.h"
#include "TaskComposite.h"
#include "WorkflowDefinition.h"
#include "WorkflowInstance.h"

TEST_CASE("findTask finds top-level tasks and children", "[WorkflowInstance]") {
    WorkflowDefinition def("Claim", "Reported");
    WorkflowInstance inst(1, "Claim 1", &def);

    TaskComposite* comp = new TaskComposite(1, "Assess", Role(1, "Clerk"));
    inst.addTask(comp);
    comp->add(new HumanTask(2, "Roof", Role(1, "Clerk"), "", -1));

    REQUIRE(inst.findTask(1) == comp);
    REQUIRE(inst.findTask(2) != nullptr);
    REQUIRE(inst.findTask(2)->getName() == "Roof");
    REQUIRE(inst.findTask(99) == nullptr);
}

TEST_CASE("openLeafTasks skips composites and closed tasks", "[WorkflowInstance]") {
    WorkflowDefinition def("Claim", "Reported");
    WorkflowInstance inst(1, "Claim 1", &def);

    TaskComposite* comp = new TaskComposite(1, "Assess", Role(1, "Clerk"));
    inst.addTask(comp);
    HumanTask* done = new HumanTask(2, "Roof",     Role(1, "Clerk"), "", -1);
    HumanTask* open = new HumanTask(3, "Plumbing", Role(1, "Clerk"), "", -1);
    comp->add(done);
    comp->add(open);
    inst.addTask(new HumanTask(4, "Solo", Role(1, "Clerk"), "", -1));

    done->setStatus("start");
    done->setStatus("complete");

    std::vector<TaskComponent*> leaves = inst.openLeafTasks();

    REQUIRE(leaves.size() == 2);
    for (size_t i = 0; i < leaves.size(); i++) {
        REQUIRE(leaves[i]->getId() != 1);   // the composite is not a leaf
        REQUIRE(leaves[i]->getId() != 2);   // the completed child is closed
    }
}