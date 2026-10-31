#ifndef TIVIDY_WORKFLOWSNAPSHOT_H
#define TIVIDY_WORKFLOWSNAPSHOT_H

#include <string>

class WorkflowSnapshot
{
public:
    WorkflowSnapshot(const std::string &timestamp, const std::string &tasksData);
    std::string getTimestamp() const;
    std::string getTasksData() const;

private:
    std::string timestamp;
    std::string tasksData;
};

#endif // TIVIDY_WORKFLOWSNAPSHOT_H
