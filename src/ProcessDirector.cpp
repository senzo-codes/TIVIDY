#include "ProcessDirector.h"
#include "TaskBuilder.h"

/**
 * @brief Stores the builder
 * @param builder The bulder to be stored
 */
ProcessDirector::ProcessDirector(TaskBuilder *builder)
{
    this->builder = builder;
}

/**
 * @brief Constructing a task
 * @param id task id
 * @param name task name
 * @param assigneeId assignee id
 * @param dueDate task's due date
 * @param requiredRole required role to handle the task
 * @return returns the constructed taskComponent
 */
TaskComponent *ProcessDirector::constructTask(int id, const std::string &name, int assigneeId, const std::string &dueDate, const Role &requiredRole)
{
    builder->reset();
    return builder->getResult();
}
