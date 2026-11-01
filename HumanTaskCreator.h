#ifndef TIVIDY_HUMANTASKCREATOR_H
#define TIVIDY_HUMANTASKCREATOR_H

#include "TaskCreator.h"
#include "HumanTaskBuilder.h"
#include "ProcessDirector.h"

/**
 * @class HumanTaskCreator
 * @brief Concrete creator (Factory Method) that makes HumanTask objects.
 *
 * Builds each task through a HumanTaskBuilder driven by a ProcessDirector. The
 * creator holds its own builder, so it cannot be copied.
 *
 * @implements TaskCreator
 * @see HumanTaskBuilder
 * @see ProcessDirector
 */
class HumanTaskCreator : public TaskCreator {
public:
    /**
     * @brief Creates the creator, its builder and its director.
     */
    HumanTaskCreator();

    /**
     * @brief Destroys the creator.
     */
    ~HumanTaskCreator() override;
    HumanTaskCreator(const HumanTaskCreator&) = delete;
    HumanTaskCreator& operator=(const HumanTaskCreator&) = delete;

    /**
     * @brief Creates a human task.
     * @param id The task id.
     * @param name The task name.
     * @param assigneeId The participant holding the task. -1 means unassigned.
     * @param dueDate The due date as YYYY-MM-DD. An empty string means no due date.
     * @param requiredRole The role the task needs.
     * @return A newly allocated HumanTask. The caller owns it.
     */
    TaskComponent* createTask(int id, const std::string& name, int assigneeId,
                              const std::string& dueDate, const Role& requiredRole) override;
private:
    HumanTaskBuilder builder;
    ProcessDirector director;
};

#endif // TIVIDY_HUMANTASKCREATOR_H
