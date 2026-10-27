#include "AutomatedTaskCreator.h"
#include "AutomatedTask.h"

AutomatedTaskCreator::AutomatedTaskCreator() { }

AutomatedTaskCreator::~AutomatedTaskCreator() { }

TaskComponent* AutomatedTaskCreator::createTask(int id, const std::string& name, int assigneeId,
                                                const std::string& dueDate, const Role& requiredRole)
{
    return new AutomatedTask(id, name);
}
