#ifndef TIVIDY_TASKCREATOR_H
#define TIVIDY_TASKCREATOR_H

#include "Role.h"
#include <string>

class TaskComponent;

/**
 * @class TaskCreator
 * @brief Creator interface (Factory Method) for making tasks.
 *
 * Each subclass decides which kind of task it builds.
 *
 * @see HumanTaskCreator
 * @see AutomatedTaskCreator
 */
class TaskCreator {
public:
    /**
     * @brief Destroys the creator through a base-class pointer.
     */
    virtual ~TaskCreator();

    /**
     * @brief Creates a task.
     * @param id The task id.
     * @param name The task name.
     * @param assigneeId The participant holding the task. -1 means unassigned.
     * @param dueDate The due date. An empty string means no due date.
     * @param requiredRole The role the task needs.
     * @return A newly allocated task. The caller owns it.
     */
    virtual TaskComponent* createTask(int id, const std::string& name, int assigneeId,
                                      const std::string& dueDate, const Role& requiredRole) = 0;
};

#endif // TIVIDY_TASKCREATOR_H
