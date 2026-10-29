#ifndef TIVIDY_WORKFLOWDEFINITION_H
#define TIVIDY_WORKFLOWDEFINITION_H

#include "TransitionRule.h"
#include "Types.h"
#include <string>
#include <vector>

class WorkflowDefinition
{
public:
    WorkflowDefinition(const std::string &name, const std::string &initialStatus);
    void addRule(const TransitionRule &rule);
    std::string getName() const;
    std::string getInitialStatus() const;
    const TransitionRule *findRule(const std::string &status, const std::string &action,
                                   const DataMap &data) const;
    std::vector<const TransitionRule *> rulesFrom(const std::string &status,
                                                  const DataMap &data) const;

private:
    std::string name;
    std::string initialStatus;
    std::vector<TransitionRule> rules;
};

#endif // TIVIDY_WORKFLOWDEFINITION_H
