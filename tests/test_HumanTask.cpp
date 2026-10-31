#include "catch.hpp"
#include "Role.h"
#include "HumanTask.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "TaskObserver.h"

class CountingObserver : public TaskObserver {
public:
    int calls;
    CountingObserver(HumanTask* subject) : TaskObserver(subject, nullptr), calls(0) {}
    void update(HumanTask&) { calls++; }
};

TEST_CASE("HumanTask follows its state machine", "[HumanTask]") {
    HumanTask task(1, "Assess", Role(1, "Clerk"), "", -1);

    REQUIRE(task.getStatus() == "Pending");
    REQUIRE_THROWS(task.setStatus("complete")); 

    task.setStatus("start");
    REQUIRE(task.getStatus() == "InProgress");

    task.setStatus("complete");
    REQUIRE(task.getStatus() == "Completed");
    REQUIRE(task.isClosed());
    REQUIRE_THROWS(task.setStatus("cancel"));      
}
