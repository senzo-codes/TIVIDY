#include "HumanTaskCreator.h"

HumanTaskCreator::HumanTaskCreator() : director(&builder) { }

HumanTaskCreator::~HumanTaskCreator() { }

TaskComponent* HumanTaskCreator::createTask(int id, const std::string& name, int assigneeId,
                                            const std::string& dueDate, const Role& requiredRole) 
{
    return director.constructTask(id, name, assigneeId, dueDate, requiredRole);
}
