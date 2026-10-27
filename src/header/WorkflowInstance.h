#ifndef TIVIDY_WORKFLOWINSTANCE_H
#define TIVIDY_WORKFLOWINSTANCE_H

#include "Types.h"
#include "AuditType.h"
#include "AuditEvent.h"
#include <string>
#include <vector>
#include <ctime>

#include "WorkflowDefinition.h"
#include "TaskComponent.h"
#include "TaskObserver.h"
#include "AuditEvent.h"
#include "TaskComposite.h"

class TaskComponent;
class TaskObserver;
class WorkflowDefinition;

class WorkflowInstance {
public:
    WorkflowInstance(int id, const std::string& name, const WorkflowDefinition* definition);
    ~WorkflowInstance();
    WorkflowInstance(const WorkflowInstance&) = delete;
    WorkflowInstance& operator=(const WorkflowInstance&) = delete;

    int getId() const;
    std::string getName() const;
    std::string getStatus() const;
    void setStatus(const std::string& status);
    const WorkflowDefinition* getDefinition() const;
    std::string getData(const std::string& key) const;
    void setData(const std::string& key, const std::string& value);
    void addTask(TaskComponent* task);
    void addObserver(TaskObserver* observer);
    void removeTask(int taskId);
    TaskComponent* findTask(int taskId) const;
    const std::vector<TaskComponent*>& getTasks() const;
    std::vector<TaskComponent*> openLeafTasks() const;
    void logEvent(AuditType type, const std::string& actor, const std::string& detail);
    const std::vector<AuditEvent>& getAuditLog() const;
private:
    int id;
    std::string name;
    std::string status;
    const WorkflowDefinition* definition;
    DataMap data;
    std::vector<TaskComponent*> tasks;
    std::vector<TaskObserver*> observers;
    std::vector<AuditEvent> auditLog;
};

#endif // TIVIDY_WORKFLOWINSTANCE_H
