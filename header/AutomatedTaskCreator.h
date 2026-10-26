#ifndef TIVIDY_AUTOMATEDTASKCREATOR_H
#define TIVIDY_AUTOMATEDTASKCREATOR_H

#include "TaskCreator.h"

class AutomatedTaskCreator : public TaskCreator {
public:
    AutomatedTaskCreator();
    ~AutomatedTaskCreator() override;

    TaskComponent* createTask(int id, const std::string& name, int assigneeId,
                              const std::string& dueDate, const Role& requiredRole) override;
};

#endif // TIVIDY_AUTOMATEDTASKCREATOR_H
