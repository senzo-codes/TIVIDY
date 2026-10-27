#ifndef TIVIDY_PARTICIPANT_H
#define TIVIDY_PARTICIPANT_H

#include "Role.h"
#include <string>

class TaskComponent;

class Participant {
public:
    Participant(int id, const std::string& name, const Role& role);
    int getId() const;
    std::string getName() const;
    Role getRole() const;
    bool canHandle(const TaskComponent& task) const;
private:
    int id;
    std::string name;
    Role role;
};

#endif // TIVIDY_PARTICIPANT_H
 