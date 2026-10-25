#ifndef TIVIDY_ROLETIERHANDLER_H
#define TIVIDY_ROLETIERHANDLER_H

#include "RequestHandler.h"
#include "Role.h"

class AssignmentRule;

class RoleTierHandler : public RequestHandler {
public:
    RoleTierHandler(const Role& tierRole, AssignmentRule* rule);
    ~RoleTierHandler() override;

    void handle(ApprovalRequest& request) override;
private:
    Role tierRole;
    AssignmentRule* rule;
};

#endif // TIVIDY_ROLETIERHANDLER_H
