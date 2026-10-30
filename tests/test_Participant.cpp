#include "catch.hpp"
#include "Role.h"
#include "Participant.h"
#include "HumanTask.h"

TEST_CASE("canHandle compares levels", "[Participant]") {
    HumanTask task(1, "Assess damage", Role(2, "Supervisor"), "", -1);

    Participant below(1, "Ann",  Role(1, "Clerk"));
    Participant equal(2, "Bea",  Role(2, "Supervisor"));
    Participant above(3, "Cara", Role(3, "Manager"));

    REQUIRE_FALSE(below.canHandle(task));
    REQUIRE(equal.canHandle(task));
    REQUIRE(above.canHandle(task));
}