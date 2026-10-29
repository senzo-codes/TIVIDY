#ifndef TIVIDY_TRANSITIONRULE_H
#define TIVIDY_TRANSITIONRULE_H

#include "Role.h"
#include <string>
#include <vector>

class TransitionRule {
public:
    TransitionRule(const std::string& fromStatus, const std::string& action,
                   const std::string& toStatus, const Role& requiredRole,
                   const std::string& guardKey, const std::string& guardValue,
                   int dueDays, const std::vector<std::string>& parallelSubtasks);
    std::string getAction() const;
    std::string getToStatus() const;
    std::string getFromStatus() const;
    Role getRequiredRole() const;
    int getDueDays() const;
    const std::vector<std::string>& getParallelSubtasks() const;

    bool hasGuard() const;
    const std::string& getGuardKey() const;
    const std::string& getGuardValue() const;

private:
    std::string fromStatus;
    std::string action;
    std::string toStatus;
    Role requiredRole;
    std::string guardKey;
    std::string guardValue;
    int dueDays;
    std::vector<std::string> parallelSubtasks;
};

#endif // TIVIDY_TRANSITIONRULE_H
