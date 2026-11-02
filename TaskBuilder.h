#ifndef TIVIDY_TASKBUILDER_H
#define TIVIDY_TASKBUILDER_H

#include "Role.h"
#include <string>

class TaskComponent;

/**
 * @class TaskBuilder
 * @brief Builder interface for assembling a task one field at a time.
 *
 * The setters return the builder so calls can be chained.
 *
 * @see HumanTaskBuilder
 * @see ProcessDirector
 */
class TaskBuilder {
public:
    /**
     * @brief Destroys the builder through a base-class pointer.
     */
    virtual ~TaskBuilder() = default;

    /**
     * @brief Restores the default values.
     * @return This builder.
     */
    virtual TaskBuilder& reset() = 0;

    /**
     * @brief Sets the task id.
     * @param id The id.
     * @return This builder.
     */
    virtual TaskBuilder& setId(int id) = 0;

    /**
     * @brief Sets the task name.
     * @param name The name.
     * @return This builder.
     */
    virtual TaskBuilder& setName(const std::string& name) = 0;

    /**
     * @brief Sets who holds the task.
     * @param assigneeId The participant id. -1 means unassigned.
     * @return This builder.
     */
    virtual TaskBuilder& setAssignee(int assigneeId) = 0;

    /**
     * @brief Sets the due date.
     * @param dueDate The date as YYYY-MM-DD. An empty string means no due date.
     * @return This builder.
     */
    virtual TaskBuilder& setDueDate(const std::string& dueDate) = 0;

    /**
     * @brief Sets the role the task needs.
     * @param role The required role.
     * @return This builder.
     */
    virtual TaskBuilder& setRequiredRole(const Role& role) = 0;

    /**
     * @brief Builds the task from the values set so far.
     * @return A newly allocated task. The caller owns it.
     */
    virtual TaskComponent* getResult() = 0;
};

#endif // TIVIDY_TASKBUILDER_H
 