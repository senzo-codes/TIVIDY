#include "TaskComposite.h"
#include <algorithm>
#include <stdexcept>

TaskComposite::TaskComposite(int id, const std::string& name, const Role& requiredRole) : TaskComponent(id, name, requiredRole) {}

TaskComposite::~TaskComposite() {
    for (TaskComponent* child : children) {
        delete child;
    }
}


void TaskComposite::add(TaskComponent* child) {
    if (child == nullptr) {
        throw std::invalid_argument("Child cannot be null");
    }
    children.push_back(child);
    child->setParent(this);
}


void TaskComposite::remove(TaskComponent* child) {
    auto childPosition = std::find(children.begin(), children.end(), child);

    if (childPosition == children.end()) {
        return;
    }

    TaskComponent* removedChild = *childPosition;
    children.erase(childPosition);
    removedChild->setParent(nullptr);
    delete removedChild;
}


TaskComponent* TaskComposite::getChild(int index) const {
    if (index < 0 || index >= static_cast<int>(children.size())) {
        throw std::out_of_range("Index out of range");
    }
    return children[index];
}


const std::vector<TaskComponent*>& TaskComposite::getChildren() const {
    return children;
}


int TaskComposite::childCount() const {
    return static_cast<int>(children.size());
}


std::string TaskComposite::getStatus() const {
    if (children.empty()) {
        return "Pending";
    }

    int cancelledCount = 0;
    int closedCount = 0;
    int completedCount = 0;
    int inProgressCount = 0;


    for (const TaskComponent* child : children) {
        std::string childStatus = child->getStatus();

        if (childStatus == "InProgress") {
            inProgressCount++;
        }

        if (childStatus == "Cancelled") {
            cancelledCount++;
        }

        if (childStatus == "Completed") {
            completedCount++;
        }

        if (childStatus == "Completed" || childStatus == "Cancelled") { //isClosed()
            closedCount++;
        }
    }

    int size = static_cast<int>(children.size());

    //all cancelled
    if (cancelledCount == size) {
        return "Cancelled";
    }

    //at least one completed the rest cancelled
    if (completedCount != 0 && closedCount == size) {
        return "Completed";
    }

    //at least one in progress or at least one completed but not all closed (some are pending)
    if (inProgressCount > 0 || (completedCount > 0 && closedCount < size)) {
        return "InProgress";
    }

    //For the case where some tasks are completed  but not all tasks are closed
    //Meaning some tasks are pending )
    //some completed the rest pending means InProgress

    return "Pending";
}


bool TaskComposite::isClosed() const {
    std::string status = getStatus();
    if (status == "Completed" || status == "Cancelled") {
        return true;
    }
    return false;
}


void TaskComposite::setStatus(const std::string& operation) {
    if (operation != "cancel") {
        throw std::invalid_argument("Invalid operation. Only 'cancel' is allowed.");
    }

    for (TaskComponent* child : children) {
        if (child->isClosed()) {
            continue;
        }
        child->setStatus("cancel");
    }
}