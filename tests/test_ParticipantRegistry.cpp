#include <memory>
#include "catch.hpp"
#include "Role.h"
#include "Participant.h"
#include "HumanTask.h"
#include "ParticipantRegistry.h"
#include "TaskComponent.h"
#include "HumanTaskBuilder.h"
#include "ProcessDirector.h"
#include <vector>

TEST_CASE("Assign moves the count from the old holder to the new one", "[ParticipantRegistry]") {
    ParticipantRegistry reg;
    reg.add(new Participant(1, "Senzo",   Role(6, "CEO")));
    reg.add(new Participant(2, "Tebello", Role(1, "Intern")));

    HumanTaskBuilder builder;
    ProcessDirector director(&builder);
    std::unique_ptr<TaskComponent> task(director.constructTask(1, "Assisting a client", -1, "2026-10-28", Role(1, "Intern")));

    reg.assign(task.get(), 2);
    REQUIRE(reg.openTaskCount(2) == 1);
    REQUIRE(task->getAssignee() == 2);

    reg.assign(task.get(), 1);
    REQUIRE(reg.openTaskCount(2) == 0);
    REQUIRE(reg.openTaskCount(1) == 1);
    REQUIRE(task->getAssignee() == 1);
}

TEST_CASE("Release lowers only that task's holder count", "[ParticipantRegistry]") {
    ParticipantRegistry reg;
    reg.add(new Participant(1, "Senzo", Role(6, "CEO")));

    HumanTaskBuilder builder;
    ProcessDirector director(&builder);
    std::unique_ptr<TaskComponent> taskA(director.constructTask(1, "Assisting a client", -1, "2026-10-28", Role(1, "Intern")));
    std::unique_ptr<TaskComponent> taskB(director.constructTask(2, "Employees direction", -1, "2026-10-30", Role(6, "CEO")));

    reg.assign(taskA.get(), 1);
    reg.assign(taskB.get(), 1);
    REQUIRE(reg.openTaskCount(1) == 2);

    reg.release(taskA.get());
    REQUIRE(reg.openTaskCount(1) == 1);
}

TEST_CASE("recountLoad rebuilds counts from open tasks", "[ParticipantRegistry]") {
    ParticipantRegistry reg;
    reg.add(new Participant(1, "Ann", Role(1, "Clerk")));
    reg.add(new Participant(2, "Bea", Role(1, "Clerk")));

    HumanTask a(1, "A", Role(1, "Clerk"), "", 1);
    HumanTask b(2, "B", Role(1, "Clerk"), "", 1);
    HumanTask c(3, "C", Role(1, "Clerk"), "", 2);

    std::vector<TaskComponent*> open;
    open.push_back(&a);
    open.push_back(&b);
    open.push_back(&c);

    reg.recountLoad(open);

    REQUIRE(reg.openTaskCount(1) == 2);
    REQUIRE(reg.openTaskCount(2) == 1);
}