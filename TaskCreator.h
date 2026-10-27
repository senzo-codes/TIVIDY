#ifndef TIVIDY_TASKCREATOR_H
#define TIVIDY_TASKCREATOR_H

#include "Role.h"
#include <string>

class TaskComponent;

class TaskCreator {
public:
    virtual ~TaskCreator();
    virtual TaskComponent* createTask(int id, const std::string& name, int assigneeId,
                                      const std::string& dueDate, const Role& requiredRole) = 0;
};

#endif // TIVIDY_TASKCREATOR_H
