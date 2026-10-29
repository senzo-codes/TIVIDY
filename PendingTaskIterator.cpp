#include "PendingTaskIterator.h"
#include "WorkflowInstance.h"
#include "TaskComponent.h"
#include <string>

PendingTaskIterator::PendingTaskIterator(const std::vector<WorkflowInstance *> &instances, int participantId)
    : index(0)
{
    for (std::size_t i = 0; i < instances.size(); ++i)
    {
        if (instances[i] == nullptr)
        {
            continue;
        }

        std::vector<TaskComponent *> openTasks = instances[i]->openLeafTasks();

        for (std::size_t j = 0; j < openTasks.size(); ++j)
        {
            TaskComponent *task = openTasks[j];
            if (task != nullptr &&
                task->getStatus() == "Pending" &&
                task->getAssignee() == participantId)
            {
                items.push_back(task);
            }
        }
    }
}

PendingTaskIterator::~PendingTaskIterator() {}

bool PendingTaskIterator::hasNext() const
{
    return index < items.size();
}

TaskComponent *PendingTaskIterator::next()
{
    if (hasNext())
    {
        return items[index++];
    }
    return nullptr;
}
