#include "catch.hpp"
#include "Role.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "LeastLoadedAssignment.h"
#include "RoleBasedAssignment.h"

TEST_CASE("Empty level moves up to the next one", "[RoleBasedAssignment]") {
    ParticipantRegistry reg;
    reg.add(new Participant(3, "Cara", Role(2, "Supervisor")));
    reg.add(new Participant(1, "Ann",  Role(3, "Manager")));

    LeastLoadedAssignment inner(&reg);
    RoleBasedAssignment rule(&reg, &inner);

    Participant* found = rule.assign(Role(1, "Clerk"));

    REQUIRE(found != nullptr);
    REQUIRE(found->getId() == 3);
}

TEST_CASE("Nobody at or above the level returns nullptr", "[RoleBasedAssignment]") {
    ParticipantRegistry reg;
    reg.add(new Participant(1, "Ann", Role(1, "Clerk")));

    LeastLoadedAssignment inner(&reg);
    RoleBasedAssignment rule(&reg, &inner);

    REQUIRE(rule.assign(Role(2, "Supervisor")) == nullptr);
}