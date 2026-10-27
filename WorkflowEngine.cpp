#include "WorkflowEngine.h"
#include "SnapshotHistory.h"
#include "WorkflowSnapshot.h"
#include "WorkflowInstance.h"
#include "TaskComponent.h"
#include "HumanTask.h"
#include "PendingState.h"
#include "InProgressState.h"
#include "CompletedState.h"
#include "CancelledState.h"

#include <map>
#include <set>
#include <sstream>

namespace {

// Snapshot text format (one record per line, fields separated by '|'):
//   I|instanceId|name|status|flagged
//   T|instanceId|taskId|name|status|assigneeId
// '\', '|' and newline inside names/statuses are escaped so they cannot break parsing.
std::string esc(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '\\')      out += "\\\\";
        else if (c == '|')  out += "\\p";
        else if (c == '\n') out += "\\n";
        else                out += c;
    }
    return out;
}

std::string unesc(const std::string& s) {
    std::string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char n = s[++i];
            out += (n == 'p') ? '|' : (n == 'n') ? '\n' : n;
        } else {
            out += s[i];
        }
    }
    return out;
}

std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> fields;
    std::string cur;
    for (char c : line) {
        if (c == '|') { fields.push_back(cur); cur.clear(); }
        else cur += c;
    }
    fields.push_back(cur);
    return fields;
}

// Adapt the status strings here if your states' getName() values differ.
TaskStates* stateFromName(const std::string& name) {
    if (name == "Pending")                          return new PendingState();
    if (name == "In Progress" || name == "InProgress") return new InProgressState();
    if (name == "Completed")                        return new CompletedState();
    if (name == "Cancelled")                        return new CancelledState();
    return nullptr;
}

struct TaskData { int id; std::string status; int assignee; };
struct InstanceData {
    std::string status;
    bool flagged;
    std::vector<TaskData> tasks;
};

} // namespace

// Originator: create the memento
std::string WorkflowEngine::serializeInstances() const {
    std::ostringstream out;
    for (WorkflowInstance* inst : instances) {
        out << "I|" << inst->getId() << '|' << esc(inst->getName()) << '|'
            << esc(inst->getStatus()) << '|' << (inst->isFlagged() ? 1 : 0) << '\n';
        for (TaskComponent* t : inst->getTasks()) {
            out << "T|" << inst->getId() << '|' << t->getId() << '|' << esc(t->getName()) << '|'
                << esc(t->getStatus()) << '|' << t->getAssignee() << '\n';
        }
    }
    return out.str();
}

void WorkflowEngine::saveSnapshot() {
    history->saveSnapshot(new WorkflowSnapshot(today, serializeInstances()));
}

// Originator: restore from the memento
void WorkflowEngine::restoreSnapshot(int index) {
    const WorkflowSnapshot& snap = history->restoreSnapshot(index);   // throws on bad index

    // 1. Parse the memento fully before touching live state.
    std::map<int, InstanceData> saved;
    std::istringstream in(snap.getTasksData());
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::vector<std::string> f = split(line);
        if (f[0] == "I" && f.size() >= 5) {
            InstanceData d;
            d.status  = unesc(f[3]);
            d.flagged = (f[4] == "1");
            saved[std::stoi(f[1])] = d;
        } else if (f[0] == "T" && f.size() >= 6) {
            saved[std::stoi(f[1])].tasks.push_back(
                { std::stoi(f[2]), unesc(f[4]), std::stoi(f[5]) });
        }
    }

    // 2. Apply it to the live instances.
    for (WorkflowInstance* inst : instances) {
        std::map<int, InstanceData>::iterator it = saved.find(inst->getId());
        if (it == saved.end()) continue;              // instance created after the snapshot
        const InstanceData& d = it->second;

        inst->setStatus(d.status);
        inst->setFlagged(d.flagged);

        // Remove tasks that did not exist when the snapshot was taken.
        std::set<int> savedIds;
        for (const TaskData& td : d.tasks) savedIds.insert(td.id);
        std::vector<int> toRemove;
        for (TaskComponent* t : inst->getTasks())
            if (savedIds.count(t->getId()) == 0) toRemove.push_back(t->getId());
        for (int id : toRemove) inst->removeTask(id);

        // Put the surviving tasks back into their saved state and assignee.
        for (const TaskData& td : d.tasks) {
            TaskComponent* t = inst->findTask(td.id);
            if (t == nullptr) continue;
            HumanTask* human = dynamic_cast<HumanTask*>(t);
            if (human != nullptr) {
                TaskStates* s = stateFromName(td.status);
                if (s != nullptr) human->setState(s);
                if (td.assignee >= 0) human->assign(td.assignee);
            }
        }
    }
}
