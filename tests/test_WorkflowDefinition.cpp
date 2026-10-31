#include <vector>
#include <string>
#include "catch.hpp"
#include "Role.h"
#include "TransitionRule.h"
#include "WorkflowDefinition.h"

TEST_CASE("findRule: a guarded row beats the default row", "[WorkflowDefinition]") {
    WorkflowDefinition def("Claim", "Reported");
    std::vector<std::string> none;
    def.addRule(TransitionRule("Assessed", "Approve", "ManagerReview", Role(2, "Supervisor"), "amount", "high", 3, none));
    def.addRule(TransitionRule("Assessed", "Approve", "Approved",      Role(2, "Supervisor"), "", "", 3, none));

    DataMap high;
    high["amount"] = "high";
    DataMap empty;

    const TransitionRule* guarded = def.findRule("Assessed", "Approve", high);
    const TransitionRule* plain   = def.findRule("Assessed", "Approve", empty);

    REQUIRE(guarded != nullptr);
    REQUIRE(guarded->getToStatus() == "ManagerReview");
    REQUIRE(plain != nullptr);
    REQUIRE(plain->getToStatus() == "Approved");
}

TEST_CASE("findRule returns nullptr when nothing matches", "[WorkflowDefinition]") {
    WorkflowDefinition def("Claim", "Reported");
    std::vector<std::string> none;
    def.addRule(TransitionRule("Assessed", "Approve", "Approved", Role(2, "Supervisor"), "", "", 3, none));

    DataMap empty;

    REQUIRE(def.findRule("Assessed", "Reject", empty) == nullptr);
    REQUIRE(def.findRule("Reported", "Approve", empty) == nullptr);
}

TEST_CASE("rulesFrom returns only rows from that status", "[WorkflowDefinition]") {
    WorkflowDefinition def("Claim", "Reported");
    std::vector<std::string> none;
    def.addRule(TransitionRule("Reported", "Assess",  "Assessed", Role(1, "Clerk"),      "", "", 3, none));
    def.addRule(TransitionRule("Assessed", "Approve", "Approved", Role(2, "Supervisor"), "", "", 3, none));
    def.addRule(TransitionRule("Assessed", "Reject",  "Rejected", Role(2, "Supervisor"), "", "", 3, none));

    DataMap empty;

    REQUIRE(def.rulesFrom("Assessed", empty).size() == 2);
    REQUIRE(def.rulesFrom("Reported", empty).size() == 1);
}

TEST_CASE("rulesFrom is empty for a finished status", "[WorkflowDefinition]") {
    WorkflowDefinition def("Claim", "Reported");
    std::vector<std::string> none;
    def.addRule(TransitionRule("Reported", "Assess", "Assessed", Role(1, "Clerk"), "", "", 3, none));

    DataMap empty;

    REQUIRE(def.rulesFrom("Assessed", empty).empty());
}