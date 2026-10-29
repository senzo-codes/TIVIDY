#ifndef TIVIDY_PARTICIPANTREGISTRY_H
#define TIVIDY_PARTICIPANTREGISTRY_H

#include <map>
#include <vector>
#include <iostream>

class Participant;
class TaskComponent;

class ParticipantRegistry
{
public:
    ParticipantRegistry();
    ~ParticipantRegistry();
    ParticipantRegistry(const ParticipantRegistry &) = delete;
    ParticipantRegistry &operator=(const ParticipantRegistry &) = delete;

    void add(Participant *participant);
    Participant *find(int id) const;
    const std::vector<Participant *> &all() const;
    std::vector<Participant *> byRole(int level) const;
    int maxRoleLevel() const;
    int openTaskCount(int participantId) const;
    void assign(TaskComponent *task, int participantId);
    void release(TaskComponent *task);
    void recountLoad(const std::vector<TaskComponent *> &openTasks);

private:
    std::vector<Participant *> participants;
    std::map<int, int> load;
};

#endif // TIVIDY_PARTICIPANTREGISTRY_H
