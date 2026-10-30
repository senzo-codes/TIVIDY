#include "catch.hpp"
#include "Role.h"
#include "ApprovalRequest.h"


TEST_CASE("Request with nobody holding it yet", "[ApprovalRequest]") {
    Role role(1, "Tebello");
    ApprovalRequest approval(1, role, 1);

    REQUIRE(approval.getTaskId() == 1);
    REQUIRE(approval.getHolderLevel() == 1);
}