#ifndef TIVIDY_TASKCOMPONENT_H
#define TIVIDY_TASKCOMPONENT_H

#include "Role.h"
#include <string>

class TaskComposite;

class TaskComponent
{
public:
    TaskComponent(int id, const std::string &name, const Role &requiredRole);
    virtual ~TaskComponent();

    virtual int getId() const;
    virtual std::string getName() const;
    virtual Role getRequiredRole() const;
    virtual TaskComposite *getParent() const;
    virtual void setParent(TaskComposite *parent);
    virtual int getAssignee() const;
    virtual void assign(int participantId);
    virtual std::string getDueDate() const;
    virtual std::string getStatus() const = 0;
    virtual void setStatus(const std::string &operation);
    virtual bool isClosed() const = 0;
    virtual void printTaskInfo() const = 0;
    virtual void add(TaskComponent *child);
    virtual void remove(TaskComponent *child);
    virtual TaskComponent *getChild(int index) const;

protected:
    int id;
    std::string name;
    Role requiredRole;
    TaskComposite *parent;
};

#endif // TIVIDY_TASKCOMPONENT_H
