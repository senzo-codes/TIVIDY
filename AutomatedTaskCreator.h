#ifndef TIVIDY_AUTOMATEDTASKCREATOR_H
#define TIVIDY_AUTOMATEDTASKCREATOR_H

#include "TaskCreator.h"

/**
 * @class AutomatedTaskCreator
 * @brief Concrete creator (Factory Method) that makes AutomatedTask objects.
 *
 * @implements TaskCreator
 * @see AutomatedTask
 */
class AutomatedTaskCreator : public TaskCreator {
public:
    /**
     * @brief Creates the creator.
     */
    AutomatedTaskCreator();

    /**
     * @brief Destroys the creator.
     */
    ~AutomatedTaskCreator() override;

    /**
     * @brief Creates an automated task.
     * @param id The task id.
     * @param name The task name.
     * @param assigneeId Ignored. Automated tasks have no assignee.
     * @param dueDate Ignored. Automated tasks have no due date.
     * @param requiredRole Ignored. Automated tasks always use the automated role.
     * @return A newly allocated AutomatedTask. The caller owns it.
     */
    TaskComponent* createTask(int id, const std::string& name, int assigneeId,
                              const std::string& dueDate, const Role& requiredRole) override;
};

#endif // TIVIDY_AUTOMATEDTASKCREATOR_H
