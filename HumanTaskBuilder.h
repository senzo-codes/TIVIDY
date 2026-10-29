#ifndef TIVIDY_HUMANTASKBUILDER_H
#define TIVIDY_HUMANTASKBUILDER_H

#include "TaskBuilder.h"
#include "HumanTask.h"
#include "Role.h"
#include <string>

class HumanTaskBuilder : public TaskBuilder
{
public:
    HumanTaskBuilder();
    ~HumanTaskBuilder() override;

    HumanTaskBuilder &reset() override;
    HumanTaskBuilder &setId(int id) override;
    HumanTaskBuilder &setName(const std::string &name) override;
    HumanTaskBuilder &setAssignee(int assigneeId) override;
    HumanTaskBuilder &setDueDate(const std::string &dueDate) override;
    HumanTaskBuilder &setRequiredRole(const Role &role) override;
    HumanTask *getResult() override;

private:
    int id;
    std::string name;
    int assigneeId;
    std::string dueDate;
    Role requiredRole;
};

#endif // TIVIDY_HUMANTASKBUILDER_H
