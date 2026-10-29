#ifndef TIVIDY_TASKBUILDER_H
#define TIVIDY_TASKBUILDER_H

#include "Role.h"
#include <string>

class TaskComponent;

class TaskBuilder {
public:
    virtual ~TaskBuilder() = default;
    virtual TaskBuilder& reset() = 0;
    virtual TaskBuilder& setId(int id) = 0;
    virtual TaskBuilder& setName(const std::string& name) = 0;
    virtual TaskBuilder& setAssignee(int assigneeId) = 0;
    virtual TaskBuilder& setDueDate(const std::string& dueDate) = 0;
    virtual TaskBuilder& setRequiredRole(const Role& role) = 0;
    virtual TaskComponent* getResult() = 0;
};

#endif // TIVIDY_TASKBUILDER_H
 