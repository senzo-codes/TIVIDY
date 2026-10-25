#ifndef TIVIDY_AUDITEVENT_H
#define TIVIDY_AUDITEVENT_H

#include "AuditType.h"
#include <string>

class AuditEvent {
public:
    AuditEvent(const std::string& timestamp, AuditType type,
               const std::string& actor, const std::string& detail);
    std::string format() const;
private:
    std::string timestamp;
    AuditType type;
    std::string actor;
    std::string detail;
};

#endif // TIVIDY_AUDITEVENT_H
