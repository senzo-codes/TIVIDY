#include "catch.hpp"
#include "Role.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "LeastLoadedAssignment.h"

TEST_CASE("Ties go to the lower id", "[LeastLoadedAssignment]") {
    ParticipantRegistry reg;
    reg.add(new Participant(3, "Cara", Role(1, "Clerk")));
    reg.add(new Participant(1, "Ann",  Role(1, "Clerk")));

    LeastLoadedAssignment rule(&reg);

    REQUIRE(rule.assign(Role(1, "Clerk"))->getId() == 1);
}  