#ifndef TIVIDY_PROCESSDIRECTOR_H
#define TIVIDY_PROCESSDIRECTOR_H

#include "Role.h"
#include <string>

class TaskBuilder;
class TaskComponent;

/**
 * @class ProcessDirector
 * @brief Director (Builder pattern) that knows the order of the construction steps.
 *
 * It resets the builder, sets every field and asks for the result. The same
 * director can drive any TaskBuilder.
 *
 * @see TaskBuilder
 * @see HumanTaskBuilder
 */
class ProcessDirector {
public:
    /**
     * @brief Creates the director.
     * @param builder The builder to drive. Not owned.
     */
    explicit ProcessDirector(TaskBuilder* builder);

    /**
     * @brief Builds one task.
     * @param id The task id.
     * @param name The task name.
     * @param assigneeId The participant holding the task. -1 means unassigned.
     * @param dueDate The due date. An empty string means no due date.
     * @param requiredRole The role the task needs.
     * @return A newly allocated task. The caller owns it.
     */
    TaskComponent* constructTask(int id, const std::string& name, int assigneeId,
                                 const std::string& dueDate, const Role& requiredRole);
private:
    TaskBuilder* builder;
};

#endif // TIVIDY_PROCESSDIRECTOR_H
