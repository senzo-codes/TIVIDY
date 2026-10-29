#ifndef TIVIDY_DECORATOR_H
#define TIVIDY_DECORATOR_H

#include "TaskComponent.h"

class Decorator : public TaskComponent {
public:
    ~Decorator() override;

    TaskComponent* getWrapped() const;
    virtual std::string decorationLabel() const = 0;

    int getId() const override;
    std::string getName() const override;
    Role getRequiredRole() const override;
    TaskComposite* getParent() const override;
    void setParent(TaskComposite* parent) override;
    int getAssignee() const override;
    void assign(int participantId) override;
    std::string getDueDate() const override;
    std::string getStatus() const override;
    void setStatus(const std::string& operation) override;
    bool isClosed() const override;
    void add(TaskComponent* child) override;
    void remove(TaskComponent* child) override;
    TaskComponent* getChild(int index) const override;
    void printTaskInfo() const override;
protected:
    explicit Decorator(TaskComponent* wrapped);
    TaskComponent* wrapped;
};

#endif // TIVIDY_DECORATOR_H
