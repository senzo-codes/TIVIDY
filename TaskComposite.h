#ifndef TIVIDY_TASKCOMPOSITE_H
#define TIVIDY_TASKCOMPOSITE_H

#include "TaskComponent.h"
#include <vector>

class TaskComposite : public TaskComponent {
public:
    TaskComposite(int id, const std::string& name, const Role& requiredRole);
    ~TaskComposite() override;

    void add(TaskComponent* child) override;
    void remove(TaskComponent* child) override;
    TaskComponent* getChild(int index) const override;
    const std::vector<TaskComponent*>& getChildren() const;
    int childCount() const;
    std::string getStatus() const override;
    bool isClosed() const override;
    void setStatus(const std::string& operation) override;
    void printTaskInfo() const override;
private:
    std::vector<TaskComponent*> children;
};

#endif // TIVIDY_TASKCOMPOSITE_H
