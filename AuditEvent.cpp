#include "AuditEvent.h"

AuditEvent::AuditEvent(const std::string &timestamp, AuditType type, const std::string &actor, const std::string &detail)
    : timestamp(timestamp), type(type), actor(actor), detail(detail) {}

std::string AuditEvent::format() const
{
    std::string str, typeStr;
    str += "| " + timestamp + " | ";

    switch (type)
    {
    case AuditType::InstanceCreated:
        typeStr = "InstanceCreated";
        break;
    case AuditType::InstanceCancelled:
        typeStr = "InstanceCancelled";
        break;
    case AuditType::InstanceFinished:
        typeStr = "InstanceFinished";
        break;
    case AuditType::TaskCreated:
        typeStr = "TaskCreated";
        break;
    case AuditType::SubtaskCreated:
        typeStr = "SubtaskCreated";
        break;
    case AuditType::AutomatedExecuted:
        typeStr = "AutomatedExecuted";
        break;
    case AuditType::TaskStarted:
        typeStr = "TaskStarted";
        break;
    case AuditType::TaskDone:
        typeStr = "TaskDone";
        break;
    case AuditType::SubtaskDone:
        typeStr = "SubtaskDone";
        break;
    case AuditType::TaskCancelled:
        typeStr = "TaskCancelled";
        break;
    case AuditType::StatusChanged:
        typeStr = "StatusChanged";
        break;
    case AuditType::OptionCancelled:
        typeStr = "OptionCancelled";
        break;
    case AuditType::ApprovalRequested:
        typeStr = "ApprovalRequested";
        break;
    case AuditType::PassedUp:
        typeStr = "PassedUp";
        break;
    case AuditType::ApprovalAccepted:
        typeStr = "ApprovalAccepted";
        break;
    case AuditType::ApprovalRejected:
        typeStr = "ApprovalRejected";
        break;
    case AuditType::TaskReassigned:
        typeStr = "TaskReassigned";
        break;
    case AuditType::DataChanged:
        typeStr = "DataChanged";
        break;
    case AuditType::Refused:
        typeStr = "Refused";
        break;
    case AuditType::PriorityChanged:
        typeStr = "PriorityChanged";
        break;
    default:
        typeStr = "Unknown";
        break;
    }

    str += typeStr;

    str += " | " + actor + " | " + detail + " |";
    return str;
}
