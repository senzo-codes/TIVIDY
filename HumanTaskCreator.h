#ifndef TIVIDY_HUMANTASKCREATOR_H
#define TIVIDY_HUMANTASKCREATOR_H

#include "TaskCreator.h"
#include "HumanTaskBuilder.h"
#include "ProcessDirector.h"

class HumanTaskCreator : public TaskCreator {
public:
    HumanTaskCreator();
    ~HumanTaskCreator() override;
    HumanTaskCreator(const HumanTaskCreator&) = delete;
    HumanTaskCreator& operator=(const HumanTaskCreator&) = delete;

    TaskComponent* createTask(int id, const std::string& name, int assigneeId,
                              const std::string& dueDate, const Role& requiredRole) override;
private:
    HumanTaskBuilder builder;
    ProcessDirector director;
};

#endif // TIVIDY_HUMANTASKCREATOR_H
