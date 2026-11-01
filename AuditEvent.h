#ifndef TIVIDY_AUDITEVENT_H
#define TIVIDY_AUDITEVENT_H

#include "AuditType.h"
#include <string>

/**
 * @class AuditEvent
 * @brief One entry in a workflow instance's audit log.
 *
 * Records when something happened, what kind of event it was, who caused it and
 * a sentence describing it. Events are created by WorkflowInstance::logEvent and
 * are not changed afterwards.
 *
 * @see AuditType
 * @see WorkflowInstance
 */
class AuditEvent {
public:
    /**
     * @brief Creates one audit entry.
     * @param timestamp When the event happened, as text.
     * @param type The kind of event.
     * @param actor Who caused the event.
     * @param detail A sentence describing the event.
     */
    AuditEvent(const std::string& timestamp, AuditType type,
               const std::string& actor, const std::string& detail);

    /**
     * @brief Formats the event as one line of text.
     * @return A line of the form "| timestamp | Type | actor | detail |".
     */
    std::string format() const;
private:
    std::string timestamp;
    AuditType type;
    std::string actor;
    std::string detail;
};

#endif // TIVIDY_AUDITEVENT_H
