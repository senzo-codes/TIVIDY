#ifndef TIVIDY_AUDITOBSERVER_H
#define TIVIDY_AUDITOBSERVER_H

#include "TaskObserver.h"

class ParticipantRegistry;

class AuditObserver : public TaskObserver {
public:
    AuditObserver(HumanTask* subject, WorkflowInstance* instance, ParticipantRegistry* registry);
    ~AuditObserver() override;

    void update(HumanTask& task) override;
private:
    ParticipantRegistry* registry;
};

#endif // TIVIDY_AUDITOBSERVER_H
