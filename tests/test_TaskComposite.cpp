#include "catch.hpp"
#include "Role.h"
#include "HumanTask.h"
#include "TaskComposite.h"

TEST_CASE("Composite is Pending, then InProgress, then Completed", "[TaskComposite]") {
    TaskComposite comp(1, "Assess", Role(1, "Clerk"));
    HumanTask* a = new HumanTask(2, "Roof",     Role(1, "Clerk"), "", -1);
    HumanTask* b = new HumanTask(3, "Plumbing", Role(1, "Clerk"), "", -1);
    comp.add(a);
    comp.add(b);

    REQUIRE(comp.getStatus() == "Pending");

    a->setStatus("start");
    REQUIRE(comp.getStatus() == "InProgress");

    a->setStatus("complete");
    b->setStatus("start");
    b->setStatus("complete");
    REQUIRE(comp.getStatus() == "Completed");
}

TEST_CASE("Composite with every child cancelled is Cancelled", "[TaskComposite]") {
    TaskComposite comp(1, "Assess", Role(1, "Clerk"));
    HumanTask* a = new HumanTask(2, "Roof",     Role(1, "Clerk"), "", -1);
    HumanTask* b = new HumanTask(3, "Plumbing", Role(1, "Clerk"), "", -1);
    comp.add(a);
    comp.add(b);

    a->setStatus("cancel");
    b->setStatus("cancel");

    REQUIRE(comp.getStatus() == "Cancelled");
}

TEST_CASE("All closed with one completed is Completed", "[TaskComposite]") {
    TaskComposite comp(1, "Assess", Role(1, "Clerk"));
    HumanTask* a = new HumanTask(2, "Roof",     Role(1, "Clerk"), "", -1);
    HumanTask* b = new HumanTask(3, "Plumbing", Role(1, "Clerk"), "", -1);
    comp.add(a);
    comp.add(b);

    a->setStatus("start");
    a->setStatus("complete");
    b->setStatus("cancel");

    REQUIRE(comp.getStatus() == "Completed");
}

TEST_CASE("Cancelling a composite cancels only its open children", "[TaskComposite]") {
    TaskComposite comp(1, "Assess", Role(1, "Clerk"));
    HumanTask* a = new HumanTask(2, "Roof",     Role(1, "Clerk"), "", -1);
    HumanTask* b = new HumanTask(3, "Plumbing", Role(1, "Clerk"), "", -1);
    comp.add(a);
    comp.add(b);

    a->setStatus("start");
    a->setStatus("complete");
    comp.setStatus("cancel");

    REQUIRE(a->getStatus() == "Completed");
    REQUIRE(b->getStatus() == "Cancelled");
}

TEST_CASE("Composite rejects every operation except cancel", "[TaskComposite]") {
    TaskComposite comp(1, "Assess", Role(1, "Clerk"));

    REQUIRE_THROWS(comp.setStatus("start"));
    REQUIRE_THROWS(comp.setStatus("complete"));
}