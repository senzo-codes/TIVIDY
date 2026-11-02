#ifndef TIVIDY_HUMANTASKBUILDER_H
#define TIVIDY_HUMANTASKBUILDER_H

#include "TaskBuilder.h"
#include "HumanTask.h"
#include "Role.h"
#include <string>

/**
 * @class HumanTaskBuilder
 * @brief Concrete builder (Builder pattern) that assembles a HumanTask step by step.
 *
 * After reset() the defaults are: no assignee, no due date and a level 1 role.
 * The setters return the builder so calls can be chained.
 *
 * @warning reset() does not give id or name a value. Call setId() and setName()
 *          before getResult().
 * @implements TaskBuilder
 * @see ProcessDirector
 */
class HumanTaskBuilder : public TaskBuilder {
public:
    /**
     * @brief Creates a builder with the defaults applied.
     */
    HumanTaskBuilder();

    /**
     * @brief Destroys the builder.
     */
    ~HumanTaskBuilder() override;

    /**
     * @brief Restores the defaults: no assignee, no due date, level 1 role.
     * @return This builder.
     */
    HumanTaskBuilder& reset() override;

    /**
     * @brief Sets the task id.
     * @param id The id.
     * @return This builder.
     */
    HumanTaskBuilder& setId(int id) override;

    /**
     * @brief Sets the task name.
     * @param name The name.
     * @return This builder.
     */
    HumanTaskBuilder& setName(const std::string& name) override;

    /**
     * @brief Sets who holds the task.
     * @param assigneeId The participant id. -1 means unassigned.
     * @return This builder.
     */
    HumanTaskBuilder& setAssignee(int assigneeId) override;

    /**
     * @brief Sets the due date.
     * @param dueDate The date as YYYY-MM-DD. An empty string means no due date.
     * @return This builder.
     */
    HumanTaskBuilder& setDueDate(const std::string& dueDate) override;

    /**
     * @brief Sets the role the task needs.
     * @param role The required role.
     * @return This builder.
     */
    HumanTaskBuilder& setRequiredRole(const Role& role) override;

    /**
     * @brief Builds the task from the values set so far.
     * @return A newly allocated HumanTask. The caller owns it.
     */
    HumanTask* getResult() override;
private:
    int id;
    std::string name;
    int assigneeId;
    std::string dueDate;
    Role requiredRole;
};

#endif // TIVIDY_HUMANTASKBUILDER_H
