#ifndef TIVIDY_PROCESSDIRECTOR_H
#define TIVIDY_PROCESSDIRECTOR_H

#include "Role.h"
#include <string>

class TaskBuilder;
class TaskComponent;

class ProcessDirector {
public:
    explicit ProcessDirector(TaskBuilder* builder);
    TaskComponent* constructTask(int id, const std::string& name, int assigneeId,
                                 const std::string& dueDate, const Role& requiredRole);
private:
    TaskBuilder* builder;
};

#endif // TIVIDY_PROCESSDIRECTOR_H
