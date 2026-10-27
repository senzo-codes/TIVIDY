#ifndef TIVIDY_AUDITTYPE_H
#define TIVIDY_AUDITTYPE_H

enum class AuditType {
    InstanceCreated, InstanceCancelled, InstanceFinished,
    TaskCreated, SubtaskCreated, AutomatedExecuted,
    TaskStarted, TaskDone, SubtaskDone, TaskCancelled,
    StatusChanged, OptionCancelled,
    ApprovalRequested, PassedUp, ApprovalAccepted, ApprovalRejected,
    TaskReassigned, DataChanged, Refused
};

#endif // TIVIDY_AUDITTYPE_H
