# TIVIDY: Class Specification v4 (C++11)

Based on `TIVIDY_Sytem_Design_v4.pdf`, with the decisions made in review. Each property and method has a plain-English explanation after the dash.

## Legend

| Tag | Meaning |
|---|---|
| **[abstract]** | Has at least one pure virtual method. Cannot be created directly. |
| **PV** | Pure virtual (`= 0`). Every subclass must provide it. |
| **V** | Virtual, with a default that subclasses may replace. |
| **O** | Overrides a virtual method from the parent class. |
| **NV** | Not virtual. Cannot be replaced by a subclass. |
| *(const)* | Reads only, does not change the object. |
| *(static)* | Belongs to the class, not to one object. |
| `+` `-` `#` | public / private / protected |

Rules for every class:
- Every base class with virtual methods has a virtual destructor.
- Classes that hold owned pointers cannot be copied.
- An owner deletes what it owns. A non-owning pointer is marked "non-owning".

## Shared types

- `DataMap` : `std::map<std::string, std::string>` — The case's input data. Every value is text, and numbers and dates are written as text too.
- `AuditType` : `enum class { InstanceCreated, InstanceCancelled, InstanceFinished, TaskCreated, SubtaskCreated, AutomatedExecuted, TaskStarted, TaskDone, SubtaskDone, TaskCancelled, StatusChanged, OptionCancelled, ApprovalRequested, PassedUp, ApprovalAccepted, ApprovalRejected, TaskReassigned, DataChanged, Refused }`
  — The kinds of event written to the audit log. Each value names one thing that happened.
- **Dates** are text in the form `YYYY-MM-DD`. An empty due date means no due date. The date arithmetic (adding days, comparing dates) is written inside `WorkflowRunner` and `DueSoonDecorator`, so there is no separate date class.

## Behaviour rules

1. **New human tasks start at the lowest level that has someone in it.** They are given to the least busy person at that level. If that level is empty, the search moves up one level at a time.
2. **Authority.** A person can start or complete a task only if they are its assignee and their level is at least the task's required level.
3. **Request, don't act.** A person without the authority can only ask for approval. Asking when they already have the authority is refused.
4. **One holder at a time.** An approval request is offered to one person. That person can accept it only if their level meets the required level. Otherwise they pass it up to the next level. The last handler in the chain rejects it, and the task stays with the person who asked.
5. **Locked while asking.** A task cannot be completed while an approval request for it is open.
6. **The definition decides the next status.** When a task finishes, the definition chooses the next status. A row with a guard applies only when the instance's data has the guard's value. Other open alternatives are cancelled.
7. **Parallel subtasks.** A composite is completed when all of its subtasks are completed. Completing the last one moves the workflow on.
8. **Automated tasks run themselves.** The engine runs them straight away and then moves on.
9. **Everything is logged.** Each event goes into the instance's audit log.

---

## 0. Roles and participants

### Role

Attributes:
- `-level : int` — The authority number. 0 is automated, 1 is the lowest human level, and higher numbers have more authority.
- `-name : string` — The label shown to the user, such as "Clerk" or "Manager". Never used in comparisons.

Methods:
- `+Role(level : int, name : const string&)` NV — Creates a role with that number and label.
- `+getLevel() : int` NV *(const)* — Returns the authority number. All comparisons use this.
- `+getName() : string` NV *(const)* — Returns the label for display.
- `+isAutomated() : bool` NV *(const)* — True when the level is 0. Tells the runner to create an automated task.

### Participant

Attributes:
- `-id : int` — The unique number of this person. Used everywhere a person is referred to.
- `-name : string` — The person's name, shown in the audit log.
- `-role : Role` — The person's authority.

Methods:
- `+Participant(id : int, name : const string&, role : const Role&)` NV — Creates the person.
- `+getId() : int` NV *(const)* — Returns the id.
- `+getName() : string` NV *(const)* — Returns the name.
- `+getRole() : Role` NV *(const)* — Returns the role.
- `+canHandle(task : const TaskComponent&) : bool` NV *(const)* — True when this person's level is at least the task's required level. The one check for "is this person senior enough".

### ParticipantRegistry

Attributes:
- `-participants : vector<Participant*>` (owned) — The list of everyone in the system. Deleted with the registry.
- `-load : map<int, int>` — For each person id, how many open tasks they hold.

Methods:
- `+add(participant : Participant*) : void` NV — Adds a person (the registry takes ownership) and sets their open-task count to 0.
- `+find(id : int) : Participant*` NV *(const)* — Returns the person with that id, or `nullptr`.
- `+all() : const vector<Participant*>&` NV *(const)* — Returns the full list, read-only.
- `+byRole(level : int) : vector<Participant*>` NV *(const)* — Returns everyone whose level is exactly that number.
- `+maxRoleLevel() : int` NV *(const)* — Returns the highest level in use, or 0 if there is nobody.
- `+openTaskCount(participantId : int) : int` NV *(const)* — Returns how many open tasks that person holds.
- `+assign(task : TaskComponent*, participantId : int) : void` NV — Gives the task to that person. Takes one off the previous holder's count and adds one to the new holder's count.
- `+release(task : TaskComponent*) : void` NV — Takes the task away from its holder and reduces that holder's count. Used when a task finishes or is cancelled.
- `+recountLoad(openTasks : const vector<TaskComponent*>&) : void` NV — Recalculates every count from the list of open tasks. Used after a snapshot is restored.

---

## 1. Tasks (Composite)

### TaskComponent [abstract]

Attributes:
- `#id : int` — The task's unique number.
- `#name : string` — The action this task is for, such as "Assess damage". The definition is searched using this name when the task finishes.
- `#requiredRole : Role` — The least authority needed to act on this task.
- `#parent : TaskComposite*` (non-owning) — The composite this task belongs to, or `nullptr` if it is a top-level task.

Methods:
- `+TaskComponent(id : int, name : const string&, requiredRole : const Role&)` NV — Stores the id, name and required role.
- `+~TaskComponent()` V — Virtual destructor, so deleting through the base pointer works.
- `+getId() : int` V *(const)* — Returns the id.
- `+getName() : string` V *(const)* — Returns the action name.
- `+getRequiredRole() : Role` V *(const)* — Returns the required role.
- `+getParent() : TaskComposite*` V *(const)* — Returns the owning composite, or `nullptr`.
- `+setParent(parent : TaskComposite*) : void` V — Sets the owning composite. Called only by `TaskComposite::add` and `remove`.
- `+getAssignee() : int` V *(const)* — Returns the assignee's id, or -1 if there is none. The default is -1.
- `+assign(participantId : int) : void` V — Sets the assignee. The default throws, because only human tasks have an assignee.
- `+getDueDate() : string` V *(const)* — Returns the due date as `YYYY-MM-DD`, or empty if there is none. The default is empty.
- `+getStatus() : string` **PV** *(const)* — Returns Pending, InProgress, Completed or Cancelled. Each type works this out differently, so it is pure virtual.
- `+setStatus(operation : const string&) : void` V — Applies `start`, `complete` or `cancel`. The default throws.
- `+isClosed() : bool` **PV** *(const)* — True when the task is Completed or Cancelled.
- `+printTaskInfo() : void` **PV** *(const)* — Prints one row describing the task.
- `+add(child : TaskComponent*) : void` V — Adds a subtask. The default throws, because only a composite holds children.
- `+remove(child : TaskComponent*) : void` V — Removes and deletes a subtask. The default throws.
- `+getChild(index : int) : TaskComponent*` V *(const)* — Returns the subtask at that position. The default returns `nullptr`.

### HumanTask : TaskComponent

Attributes:
- `-assigneeId : int` — The person it is given to, or -1 if nobody.
- `-dueDate : string` — The deadline as `YYYY-MM-DD`, or empty if there is none.
- `-currentState : TaskStates*` — Points to the shared state object for the current status.
- `-priority : bool` — True if the user marked it as a priority.
- `-observers : vector<TaskObserver*>` (non-owning) — The observers to tell when the status changes.

Methods:
- `+HumanTask(id : int, name : const string&, requiredRole : const Role&, dueDate : const string&, assigneeId : int)` NV — Creates the task in the Pending state.
- `+~HumanTask()` O — Destroys the task. Observers remove themselves in their own destructors.
- `+assign(participantId : int) : void` O — Sets the assignee. Only `ParticipantRegistry::assign` calls this.
- `+getAssignee() : int` O *(const)* — Returns the assignee's id.
- `+getDueDate() : string` O *(const)* — Returns the due date.
- `+getStatus() : string` O *(const)* — Returns the name of the current state.
- `+setStatus(operation : const string&) : void` O — Asks the current state to apply the operation, then tells the observers. This is the only way the status changes.
- `+isClosed() : bool` O *(const)* — True when the current state is Completed or Cancelled.
- `+setState(state : TaskStates*) : void` NV — Changes the current state. Called only from inside the state classes.
- `+isPriority() : bool` NV *(const)* — Returns the priority flag.
- `+setPriority(priority : bool) : void` NV — Sets the priority flag.
- `+attach(observer : TaskObserver*) : void` NV — Adds an observer to the list. Called from the observer's constructor.
- `+detach(observer : TaskObserver*) : void` NV — Removes an observer from the list. Called from the observer's destructor.
- `-notify() : void` NV — Tells every observer about the change. It works on a copy of the list, so an observer can remove itself while the loop runs.
- `+printTaskInfo() : void` O *(const)* — Prints the task's row: id, name, status, assignee and due date.

### AutomatedTask : TaskComponent

Attributes:
- `-executed : bool` — True once the automated step has run.

Methods:
- `+AutomatedTask(id : int, name : const string&)` NV — Creates the task. Its required role is level 0 (automated).
- `+~AutomatedTask()` O — Destroys the task.
- `+execute() : void` V — Runs the automated step and marks it executed. The prototype does nothing except mark it.
- `+getStatus() : string` O *(const)* — Returns Completed once executed, otherwise Pending.
- `+setStatus(operation : const string&) : void` O — Always throws. Automated tasks are only run with `execute()`.
- `+isClosed() : bool` O *(const)* — True once executed.
- `+printTaskInfo() : void` O *(const)* — Prints the row, marked as automated.

### TaskComposite : TaskComponent

Used for a parallel row. The composite is the action, and its children are the steps that must all be done.

Attributes:
- `-children : vector<TaskComponent*>` (owned) — The subtasks. Deleted with the composite.

Methods:
- `+TaskComposite(id : int, name : const string&, requiredRole : const Role&)` NV — Creates an empty composite.
- `+~TaskComposite()` O — Deletes its children.
- `+add(child : TaskComponent*) : void` O — Adds a subtask and sets its parent to this composite.
- `+remove(child : TaskComponent*) : void` O — Removes a subtask and deletes it.
- `+getChild(index : int) : TaskComponent*` O *(const)* — Returns the subtask at that position.
- `+childCount() : int` NV *(const)* — Returns how many subtasks there are.
- `+getStatus() : string` O *(const)* — Works out the composite's status from its children:
  - all cancelled gives Cancelled;
  - all closed, with at least one completed, gives Completed;
  - any child started gives InProgress;
  - otherwise Pending.
- `+isClosed() : bool` O *(const)* — True when the status is Completed or Cancelled.
- `+setStatus(operation : const string&) : void` O — Only `cancel` is accepted. It cancels every open child. Any other operation throws.
- `+printTaskInfo() : void` O *(const)* — Prints the composite's row, then each child's row.

Decision: a composite can only sit at the top level. The runner never puts a composite inside another composite.

---

## 2. Task states (State)

### TaskStates [abstract]

Methods:
- `+~TaskStates()` V — Virtual destructor.
- `+handle(task : HumanTask&, operation : const string&) : void` **PV** — Decides whether the operation is allowed from this state. If it is, it moves the task to the next state. If not, it throws.
- `+getName() : string` **PV** *(const)* — Returns the status text.
- `+isClosed() : bool` V *(const)* — True if this state means the task is finished. The default is false.

The four states are shared singletons. Every task points at the same object for each state, so no state is created per task.

### PendingState : TaskStates
- `+instance() : PendingState&` NV *(static)* — Returns the one shared Pending object.
- `+handle(task, operation) : void` O — `start` moves to InProgress, `cancel` moves to Cancelled. Anything else throws.
- `+getName() : string` O *(const)* — Returns "Pending".
- `+isClosed() : bool` O *(const)* — Returns false.

### InProgressState : TaskStates
- `+instance() : InProgressState&` NV *(static)* — Returns the one shared InProgress object.
- `+handle(task, operation) : void` O — `complete` moves to Completed, `cancel` moves to Cancelled. Anything else throws.
- `+getName() : string` O *(const)* — Returns "InProgress".
- `+isClosed() : bool` O *(const)* — Returns false.

### CompletedState : TaskStates
- `+instance() : CompletedState&` NV *(static)* — Returns the one shared Completed object.
- `+handle(task, operation) : void` O — Always throws. A completed task cannot change.
- `+getName() : string` O *(const)* — Returns "Completed".
- `+isClosed() : bool` O *(const)* — Returns true.

### CancelledState : TaskStates
- `+instance() : CancelledState&` NV *(static)* — Returns the one shared Cancelled object.
- `+handle(task, operation) : void` O — Always throws. A cancelled task cannot change.
- `+getName() : string` O *(const)* — Returns "Cancelled".
- `+isClosed() : bool` O *(const)* — Returns true.

---

## 3. Decorators (labels only)

Decorators add a label to a task when it is drawn. They do not change the task or block anything. Each one is a view: the UI creates it when it draws a row, and it never deletes the task.

### Decorator [abstract] : TaskComponent

Attributes:
- `#wrapped : TaskComponent*` (non-owning) — The task being decorated.

Methods:
- `#Decorator(wrapped : TaskComponent*)` NV — Stores the wrapped task.
- `+~Decorator()` V — Does not delete the wrapped task.
- `+getWrapped() : TaskComponent*` NV *(const)* — Returns the task being decorated.
- `+decorationLabel() : string` **PV** *(const)* — Returns the label text, or empty.
- `+getId()`, `getName()`, `getRequiredRole()`, `getParent()`, `setParent()`, `getAssignee()`, `assign()`, `getDueDate()`, `getStatus()`, `setStatus()`, `isClosed()`, `add()`, `remove()`, `getChild()` — **O**. Each one passes the call straight to the wrapped task, so the decorator behaves like the task.
- `+printTaskInfo() : void` O *(const)* — Prints the wrapped task's row, then adds `[label]` if there is a label.

### PriorityDecorator : Decorator
- `-priority : bool` — Whether to show the priority label.
- `+PriorityDecorator(wrapped : TaskComponent*, priority : bool)` NV — Creates the view.
- `+decorationLabel() : string` O *(const)* — Returns "priority" when set, otherwise empty.

### DueSoonDecorator : Decorator
- `-today : string` — The current date as `YYYY-MM-DD`, passed in when the view is made.
- `-daysThreshold : int` — How many days ahead counts as "due soon". The default is 2.
- `+DueSoonDecorator(wrapped : TaskComponent*, today : const string&, daysThreshold : int)` NV — Creates the view.
- `+checkIfDueSoon() : bool` NV *(const)* — True if the task is open, has a due date, and the due date is within the threshold after today and not yet past.
- `+isOverdue() : bool` NV *(const)* — True if the task is open, has a due date, and the due date is before today.
- `+decorationLabel() : string` O *(const)* — Returns "overdue", "due soon", or empty. Overdue is checked first.

### ApprovalPendingDecorator : Decorator
- `-request : const ApprovalRequest*` (non-owning) — The open approval request for this task, or `nullptr`.
- `+ApprovalPendingDecorator(wrapped : TaskComponent*, request : const ApprovalRequest*)` NV — Creates the view.
- `+isLocked() : bool` NV *(const)* — True if there is an open request.
- `+decorationLabel() : string` O *(const)* — Returns "approval pending" while locked, otherwise empty.

---

## 4. Creating tasks (Factory Method and Builder)

### TaskCreator [abstract]
- `+~TaskCreator()` V — Virtual destructor.
- `+createTask(id : int, name : const string&, assigneeId : int, dueDate : const string&, requiredRole : const Role&) : TaskComponent*` **PV** — Creates a task. The caller owns the result.

### HumanTaskCreator : TaskCreator
- `-builder : HumanTaskBuilder` — The builder that holds the settings for the next task. Declared before `director`.
- `-director : ProcessDirector` — Runs the build steps in order.
- `+HumanTaskCreator()` NV — Sets up the director with the builder.
- `+createTask(...) : TaskComponent*` O — Asks the director to build a human task.

### AutomatedTaskCreator : TaskCreator
- `+createTask(...) : TaskComponent*` O — Creates an `AutomatedTask`. The assignee, due date and role are ignored.

### TaskBuilder [abstract]
- `+~TaskBuilder()` V — Virtual destructor.
- `+reset() : TaskBuilder&` **PV** — Clears all settings back to their defaults.
- `+setId(id : int) : TaskBuilder&` **PV** — Sets the id, and returns the builder so calls can be chained.
- `+setName(name : const string&) : TaskBuilder&` **PV** — Sets the name.
- `+setAssignee(assigneeId : int) : TaskBuilder&` **PV** — Sets the assignee.
- `+setDueDate(dueDate : const string&) : TaskBuilder&` **PV** — Sets the due date.
- `+setRequiredRole(role : const Role&) : TaskBuilder&` **PV** — Sets the required role.
- `+getResult() : TaskComponent*` **PV** — Creates the task from the settings. The caller owns it.

### HumanTaskBuilder : TaskBuilder
- `-id`, `-name`, `-assigneeId`, `-dueDate`, `-requiredRole` — The settings for the next task.
- `+reset() : HumanTaskBuilder&` O — Sets the defaults: no assignee, no due date, level 1.
- `+setId`, `+setName`, `+setAssignee`, `+setDueDate`, `+setRequiredRole` : `HumanTaskBuilder&` O — Each stores one setting and returns the builder.
- `+getResult() : HumanTask*` O — Creates the `HumanTask` from the settings.

### ProcessDirector
- `-builder : TaskBuilder*` (non-owning) — The builder to drive.
- `+ProcessDirector(builder : TaskBuilder*)` NV — Stores the builder.
- `+constructTask(id, name, assigneeId, dueDate, requiredRole) : TaskComponent*` NV — Runs reset, setId, setName, setAssignee, setDueDate, setRequiredRole, getResult in that order. Every task is built the same way.

---

## 5. Assigning people (Strategy)

### AssignmentRule [abstract]
- `+~AssignmentRule()` V — Virtual destructor.
- `+assign(role : const Role&) : Participant*` **PV** *(const)* — Returns someone at exactly that level, or `nullptr`.

### LeastLoadedAssignment : AssignmentRule
- `-registry : ParticipantRegistry*` (non-owning) — Where the people and their counts are kept.
- `+LeastLoadedAssignment(registry : ParticipantRegistry*)` NV — Stores the registry.
- `+assign(role : const Role&) : Participant*` O *(const)* — Among the people at that level, returns the one with the fewest open tasks. Ties go to the lower id.

### RoleBasedAssignment : AssignmentRule
- `-registry : ParticipantRegistry*` (non-owning) — Used to find the highest level in use.
- `-inner : AssignmentRule*` (non-owning) — The rule used at each level. Normally `LeastLoadedAssignment`.
- `+RoleBasedAssignment(registry : ParticipantRegistry*, inner : AssignmentRule*)` NV — Stores both.
- `+assign(minRole : const Role&) : Participant*` O *(const)* — Tries the given level first, then each higher level in turn, and returns the first person found.

---

## 6. Approvals (Chain of Responsibility)

### ApprovalRequest
Attributes:
- `-taskId : int` — The task being asked about.
- `-requiredRole : Role` — The level the task needs.
- `-holderLevel : int` — The level of the person it is currently offered to.
- `-holderId : int` — The id of that person, or -1.
- `-assignedTo : int` — The id of the person who accepted it, or -1.
- `-rejected : bool` — True if nobody could take it.

Methods:
- `+ApprovalRequest(taskId : int, requiredRole : const Role&, requesterLevel : int)` NV — Creates an open request with no holder. The holder level starts at the requester's level.
- `+getTaskId() : int` NV *(const)* — Returns the task id.
- `+getHolderId() : int` NV *(const)* — Returns the current holder's id.
- `+getHolderLevel() : int` NV *(const)* — Returns the current holder's level.
- `+offerTo(participantId : int, level : int) : void` NV — Makes that person the current holder.
- `+accept(participantId : int) : void` NV — Records who accepted it. The approval service checks authority first.
- `+markRejected() : void` NV — Marks the request as rejected.
- `+canBeAcceptedBy(level : int) : bool` NV *(const)* — True if the level meets the required level.
- `+isOpen() : bool` NV *(const)* — True while nobody has accepted and nobody has rejected.

### RequestHandler [abstract]
- `#successor : RequestHandler*` (owned) — The next handler in the chain.
- `+~RequestHandler()` V — Deletes the successor, which deletes the rest of the chain.
- `+setSuccessor(successor : RequestHandler*) : void` NV — Links the next handler.
- `+handle(request : ApprovalRequest&) : void` **PV** — Offers the request to someone, or passes it on.

### RoleTierHandler : RequestHandler
One handler per level, so each step goes up one level at a time.
- `-tierRole : Role` — The level this handler covers.
- `-rule : AssignmentRule*` (non-owning) — Used to find someone at this level.
- `+RoleTierHandler(tierRole : const Role&, rule : AssignmentRule*)` NV — Creates the handler.
- `+handle(request : ApprovalRequest&) : void` O — If the request is open, this level is above the current holder, and someone is found here, offers the request to them. Otherwise passes it to the successor.

### FallbackHandler : RequestHandler
- `+handle(request : ApprovalRequest&) : void` O — The end of the chain. If the request is still open, rejects it. The approval service writes the audit entry.

### ApprovalService
Attributes:
- `-registry : ParticipantRegistry*` (non-owning) — Used to find people and to reassign tasks.
- `-rule : AssignmentRule*` (non-owning) — Passed to each tier handler.
- `-chainHead : RequestHandler*` (owned) — The first handler in the chain.
- `-requests : vector<ApprovalRequest*>` (owned) — Every request ever made.

Methods:
- `+ApprovalService(registry : ParticipantRegistry*, rule : AssignmentRule*)` NV — Builds the chain: one tier handler for each level from 2 up to the highest level in use, then a fallback handler.
- `+~ApprovalService()` V — Deletes the requests and the chain.
- `+rebuildChain() : void` NV — Builds the chain again. The engine calls this whenever someone is added, so the chain always matches the roster.
- `+request(instance : WorkflowInstance&, task : TaskComponent*, requester : Participant&) : ApprovalRequest*` NV — Creates a request for a task the requester cannot do. Refused if the requester is the assignee's superior or if a request is already open. Offers the request up the chain and logs the result.
- `+accept(instance : WorkflowInstance&, request : ApprovalRequest*, participant : Participant&) : void` NV — The holder accepts. Refused unless the person is the holder and their level is high enough. Reassigns the task to them and logs it.
- `+passUp(instance : WorkflowInstance&, request : ApprovalRequest*, participant : Participant&) : void` NV — The holder passes it up. Refused unless the person is the holder. Sends it to the next level, and logs the result.
- `+openRequestFor(taskId : int) : ApprovalRequest*` NV *(const)* — Returns the open request for that task, or `nullptr`. The runner uses this to check for a lock.

---

## 7. Workflow definition

### TransitionRule
Attributes:
- `-fromStatus : string` — The status this row applies from.
- `-action : string` — The action that triggers the row.
- `-toStatus : string` — The status it moves to.
- `-requiredRole : Role` — The level needed for the task this row creates.
- `-guardKey : string` — The data field this row checks, such as `fraudFlag`. Empty means no guard. If the field is not set yet, the row still creates its task, and the guard decides the next status when the task completes.
- `-guardValue : string` — The value the field must have, such as `yes`.
- `-dueDays : int` — Days until the task is due. Zero means no due date. Negative means already overdue.
- `-parallelSubtasks : vector<string>` — Names of subtasks. Empty means one task.

Methods:
- `+TransitionRule(fromStatus, action, toStatus, requiredRole, guardKey, guardValue, dueDays, parallelSubtasks)` NV — Creates one row.
- `+getAction() : string` NV *(const)* — Returns the action name.
- `+getToStatus() : string` NV *(const)* — Returns the status it moves to.
- `+getRequiredRole() : Role` NV *(const)* — Returns the level needed.
- `+getDueDays() : int` NV *(const)* — Returns the days until due.
- `+getParallelSubtasks() : const vector<string>&` NV *(const)* — Returns the subtask names.

### WorkflowDefinition
Attributes:
- `-name : string` — The definition's name, used to look it up.
- `-initialStatus : string` — The status a new instance starts in.
- `-rules : vector<TransitionRule>` — The rows. Order matters: the first match wins, so guarded rows come before their default.

Methods:
- `+WorkflowDefinition(name : const string&, initialStatus : const string&)` NV — Creates an empty definition.
- `+addRule(rule : const TransitionRule&) : void` NV — Adds a row. This is where each industry writes its own statuses and actions.
- `+getName() : string` NV *(const)* — Returns the name.
- `+getInitialStatus() : string` NV *(const)* — Returns the starting status.
- `+findRule(status : const string&, action : const string&, data : const DataMap&) : const TransitionRule*` NV *(const)* — Returns the first row whose status and action match and whose guard is empty or matches `data`, or `nullptr`.
- `+rulesFrom(status : const string&, data : const DataMap&) : vector<const TransitionRule*>` NV *(const)* — Returns every row starting from that status whose guard is empty, matches `data`, or refers to a field that is not set yet. An empty list means the instance is finished.

---

## 8. Workflow instance

### WorkflowInstance
Attributes:
- `-id : int` — The instance number.
- `-name : string` — The name shown to the user.
- `-status : string` — The current status.
- `-definition : const WorkflowDefinition*` (non-owning) — The definition this instance follows.
- `-data : DataMap` — The case's input data as text key/value pairs, such as `fraudFlag = yes`. Guards read it. The meaning of each field is up to the industry.
- `-tasks : vector<TaskComponent*>` (owned) — The top-level tasks.
- `-observers : vector<TaskObserver*>` (owned) — The observers attached to this instance's tasks. Deleted before the tasks.
- `-auditLog : vector<AuditEvent>` — The history of this instance.

Methods:
- `+WorkflowInstance(id : int, name : const string&, definition : const WorkflowDefinition*)` NV — Creates the instance at the definition's starting status.
- `+~WorkflowInstance()` V — Deletes the observers first, then the tasks.
- `+getId() : int` NV *(const)* — Returns the id.
- `+getName() : string` NV *(const)* — Returns the name.
- `+getStatus() : string` NV *(const)* — Returns the status.
- `+setStatus(status : const string&) : void` NV — Changes the status. Only the runner calls this.
- `+getDefinition() : const WorkflowDefinition*` NV *(const)* — Returns the definition.
- `+getData(key : const string&) : string` NV *(const)* — Returns the value stored under that key, or empty text if there is none.
- `+setData(key : const string&, value : const string&) : void` NV — Stores a value and logs `DataChanged` with the old and new values.
- `+addTask(task : TaskComponent*) : void` NV — Adds a top-level task. The instance takes ownership.
- `+addObserver(observer : TaskObserver*) : void` NV — Keeps an observer alive for as long as the instance exists.
- `+removeTask(taskId : int) : void` NV — Finds a top-level task by id, removes it and deletes it.
- `+findTask(taskId : int) : TaskComponent*` NV *(const)* — Searches top-level tasks and their children. Returns the task or `nullptr`.
- `+getTasks() : const vector<TaskComponent*>&` NV *(const)* — Returns the top-level tasks.
- `+openLeafTasks() : vector<TaskComponent*>` NV *(const)* — Returns every task that is not closed and is not a composite.
- `+logEvent(type : AuditType, actor : const string&, detail : const string&) : void` NV — Adds one entry to the audit log with the current time.
- `+getAuditLog() : const vector<AuditEvent>&` NV *(const)* — Returns the audit log.

### AuditEvent
Attributes:
- `-timestamp : string` — When it happened.
- `-type : AuditType` — What kind of event it was.
- `-actor : string` — Who caused it.
- `-detail : string` — A sentence describing it.

Methods:
- `+AuditEvent(timestamp, type, actor, detail)` NV — Creates the entry.
- `+format() : string` NV *(const)* — Returns the entry as one line for the audit table.

---

## 9. Engine (Facade), runner and approvals

The UI only calls `WorkflowEngine`. The engine checks ids and passes the work on. The rules live in `WorkflowRunner` and `ApprovalService`.

### WorkflowEngine (Facade)
Attributes:
- `-registry : ParticipantRegistry*` (owned) — The people in the system.
- `-leastLoaded : LeastLoadedAssignment*` (owned) — The least-busy rule.
- `-assignment : RoleBasedAssignment*` (owned) — Wraps `leastLoaded` to start at the lowest level.
- `-approvals : ApprovalService*` (owned) — Handles approval requests.
- `-runner : WorkflowRunner*` (owned) — Runs tasks and statuses.
- `-history : SnapshotHistory*` (owned) — Stored snapshots.
- `-definitions : map<string, WorkflowDefinition*>` (owned) — The process maps, by name.
- `-instances : vector<WorkflowInstance*>` (owned) — The running instances.
- `-today : string` — The current date.
- `-nextInstanceId : int` — The next instance number to use.

Methods:
- `+WorkflowEngine()` NV — Creates all the subsystems.
- `+~WorkflowEngine()` V — Deletes the instances before the subsystems, and everything else it owns.
- `+addParticipant(id : int, name : const string&, level : int, roleName : const string&) : void` NV — Adds a person (refused if the id is taken), then rebuilds the approval chain.
- `+setToday(today : const string&) : void` NV — Sets the current date for the runner.
- `+registerDefinition(definition : WorkflowDefinition*) : void` NV — Stores a process map. Refused if the name is taken.
- `+startInstance(definitionName : const string&, name : const string&, data : const DataMap&) : WorkflowInstance*` NV — Creates an instance with its input data and asks the runner to create its first tasks.
- `+findInstance(id : int) : WorkflowInstance*` NV *(const)* — Returns the instance with that id, or `nullptr`.
- `+dashboardFor(participantId : int) : PendingTaskIterator` NV *(const)* — Builds a `PendingTaskIterator` over all instances for that person. The dashboard uses this to list the tasks the person can start.
- `+startTask(instanceId : int, taskId : int, participantId : int) : void` NV — Finds the three objects, then the runner starts the task.
- `+completeTask(instanceId : int, taskId : int, participantId : int) : void` NV — Finds the three objects, then the runner completes the task.
- `+cancelInstance(instanceId : int) : void` NV — Finds the instance, then the runner cancels it.
- `+requestApproval(instanceId : int, taskId : int, participantId : int) : ApprovalRequest*` NV — Finds the three objects, then the approval service makes the request.
- `+acceptRequest(instanceId : int, request : ApprovalRequest*, participantId : int) : void` NV — Finds the instance and the person, then the approval service accepts.
- `+passUpRequest(instanceId : int, request : ApprovalRequest*, participantId : int) : void` NV — Finds the instance and the person, then the approval service passes it up.
- `+saveSnapshot() : void` NV — Writes every instance and task to one text block, and stores it with today's date.
- `+restoreSnapshot(index : int) : void` NV — Replaces the current instances with the ones in the saved snapshot, then recalculates the workload counts. Throws if the index does not exist.
- `-serializeInstances() : string` NV *(const)* — Writes the text block used by `saveSnapshot`. One line per instance and one per task.

### WorkflowRunner
Attributes:
- `-registry : ParticipantRegistry*` (non-owning) — Used to assign and release people.
- `-assignment : RoleBasedAssignment*` (non-owning) — Used to find someone for a new task.
- `-approvals : ApprovalService*` (non-owning) — Used to check for a lock.
- `-humanCreator : HumanTaskCreator` (owned) — Creates human tasks.
- `-automatedCreator : AutomatedTaskCreator*` (owned) — Creates automated tasks.
- `-today : string` — The current date, used for due dates.
- `-nextTaskId : int` — The next task number to use.

Methods:
- `+WorkflowRunner(registry, assignment, approvals)` NV — Stores the collaborators and starts task numbering at 1.
- `+setToday(today : const string&) : void` NV — Sets the current date.
- `+start(instance : WorkflowInstance&) : void` NV — Logs the creation and creates the first tasks.
- `+startTask(instance, task, participant) : void` NV — Checks that the person is the assignee and has the level. Then starts the task and logs it.
- `+completeTask(instance, task, participant) : void` NV — Steps:
  1. Check the person is the assignee and has the level. Otherwise throw `AuthorityError`.
  2. If an approval request is open for the task, throw `IllegalTransitionError`.
  3. Find the top task (the composite if there is one, otherwise the task). Look up the matching row in the definition. If there is none, log `Refused` and stop.
  4. Complete the task and release it from the person. The observers then log it and call `advance`.
- `+advance(instance, top : TaskComponent*) : void` NV — Called by `CompletionObserver`. If the top task is not completed yet, it waits. Otherwise it cancels the other open top-level tasks, moves the instance to the row's status, logs it, and creates the next tasks.
- `+cancelInstance(instance : WorkflowInstance&) : void` NV — Cancels every open task, releases each from its person, sets the status to Cancelled and logs it.
- `-spawnTasks(instance : WorkflowInstance&) : void` NV — Creates the tasks for the instance's current status. It takes the rows from `rulesFrom`, keeps one row per distinct action (the first one), and creates a task from that row's role, due days and parallel subtasks. If there are no rows, logs `InstanceFinished`.
  - An automated row creates an automated task, runs it, moves the status on, and checks again.
  - A row without subtasks creates one human task at the lowest level that has someone, with a due date from today plus `dueDays`. If nobody is found, it stays unassigned and this is logged.
  - A row with subtasks creates a composite with one human task for each subtask name.
  - Each human task gets a `CompletionObserver` and an `AuditObserver`, added to the instance.

Decorators are not created here. The UI builds them when it draws a task.

### ApprovalService
Covered in section 6.

---

## 10. Snapshots (Memento)

### WorkflowSnapshot
Attributes:
- `-timestamp : string` — When it was saved.
- `-tasksData : string` — The saved text block for all instances and tasks.

Methods:
- `+WorkflowSnapshot(timestamp, tasksData)` NV — Creates the snapshot.
- `+getTimestamp() : string` NV *(const)* — Returns the time it was saved.
- `+getTasksData() : string` NV *(const)* — Returns the saved text.

### SnapshotHistory
Attributes:
- `-snapshots : vector<WorkflowSnapshot*>` (owned) — Every saved snapshot.

Methods:
- `+saveSnapshot(snapshot : WorkflowSnapshot*) : void` NV — Stores the snapshot. The history takes ownership.
- `+restoreSnapshot(index : int) : const WorkflowSnapshot&` NV *(const)* — Returns the snapshot at that position.
- `+count() : int` NV *(const)* — Returns how many snapshots are stored.

---

## 11. Observer and deadlines

The `HumanTask` is the subject. When its status changes, its observers are told. A due date passing is not a status change, so overdue tasks are not an observer. The UI shows them with a label (section 3).

### TaskObserver [abstract]
Attributes:
- `#subject : HumanTask*` (non-owning) — The task being watched.
- `#instance : WorkflowInstance*` (non-owning) — The instance the task belongs to.

Methods:
- `+TaskObserver(subject : HumanTask*, instance : WorkflowInstance*)` NV — Stores both and attaches itself to the task.
- `+~TaskObserver()` V — Detaches itself from the task.
- `+update(task : HumanTask&) : void` **PV** — Called whenever the task's status changes.

### CompletionObserver : TaskObserver
- `-runner : WorkflowRunner*` (non-owning) — Used to move the workflow on.
- `+CompletionObserver(subject, instance, runner)` NV — Creates the observer.
- `+update(task : HumanTask&) : void` O — When the task is Completed, finds the top task and asks the runner to advance. Does nothing for other statuses.

### AuditObserver : TaskObserver
- `-registry : ParticipantRegistry*` (non-owning) — Used to get the person's name for the log.
- `+AuditObserver(subject, instance, registry)` NV — Creates the observer.
- `+update(task : HumanTask&) : void` O — Writes one audit entry: started, done, or cancelled, with the person's name.

---

## 12. Iterators

### Iterator [abstract]
- `+~Iterator()` V — Virtual destructor.
- `+hasNext() : bool` **PV** *(const)* — True while there are more items.
- `+next() : TaskComponent*` **PV** — Returns the next item and moves on.

### PendingTaskIterator : Iterator
- `-items : vector<TaskComponent*>` — The Pending tasks assigned to one person, across all instances. Taken when the iterator is created.
- `-index : size_t` — The current position.
- `+PendingTaskIterator(instances : const vector<WorkflowInstance*>&, participantId : int)` NV — Goes through every instance's open leaf tasks and keeps only those whose status is `Pending` and whose assignee is `participantId`. InProgress tasks are not included.
- `+hasNext() : bool` O *(const)* — True while the position is inside the list.
- `+next() : TaskComponent*` O — Returns the item and moves on.

### DueDateIterator : Iterator
- `-items : vector<TaskComponent*>` — A copy of the tasks to walk.
- `-index : size_t` — The current position.
- `+DueDateIterator(tasks : const vector<TaskComponent*>&)` NV — Copies the tasks and sorts them.
- `+sortByDate() : void` NV — Sorts priority tasks first, then by earliest due date. Tasks with no due date go last.
- `+hasNext() : bool` O *(const)* — True while the position is inside the list.
- `+next() : TaskComponent*` O — Returns the item and moves on.

---

## Generic engine and industry-specific parts

Both scenarios run on the same engine. The following are reused without change.

**Generic (same for both scenarios):** `WorkflowEngine`, `WorkflowRunner`, `ApprovalService`, `ApprovalRequest`, the `RequestHandler` family, the `AssignmentRule` family, `ParticipantRegistry`, `Participant`, `Role`, the `TaskComponent` family, the `TaskStates` family, the `Decorator` family, the `TaskCreator` family, `TaskBuilder`, `HumanTaskBuilder`, `ProcessDirector`, `WorkflowInstance`, `AuditEvent`, the `TaskObserver` family, `SnapshotHistory` and the `Iterator` family.

**Industry-specific (data only):**
- The `WorkflowDefinition` and its `TransitionRule` rows.
- The list of people and their levels.
- What each data field means. The engine only compares text values.

The engine never compares a status or action name to a fixed value. The only fixed names are the four states (Pending, InProgress, Completed, Cancelled).

---

## Design decisions

- Keep both creators: `HumanTaskCreator` and `AutomatedTaskCreator`.
- Keep `ProcessDirector` as a separate class that runs the build steps in order.
- Parallel subtasks are in the demo. A composite only sits at the top level.
- Decorators are name labels only: `PriorityDecorator`, `DueSoonDecorator`, `ApprovalPendingDecorator`. They are created when a row is drawn.
- `WorkflowEngine` is a simple facade over `WorkflowRunner` and `ApprovalService`. It also calls `SnapshotHistory`.
- Four states: Pending, InProgress, Completed, Cancelled.
- Least-loaded assignment is used for picking someone at a level.
- The audit log is written by `AuditObserver`.
- Overdue tasks are shown with a label only. Nothing escalates or reassigns them.
- `DueDateIterator` sorts its copy and walks it. It is used to order tasks by priority and due date.
- `PendingTaskIterator` is used by the person's dashboard. It lists only Pending tasks assigned to that person, across all instances. InProgress tasks are not included.
- The approval chain goes one level at a time, and is rebuilt whenever a person is added. The last handler (`FallbackHandler`) rejects the request.
- Save and restore are in the demo, so Memento counts toward the design.

---

## Pattern coverage

Eleven GoF patterns are used. Each category has at least two. Five are on the spec's required list: Observer, Iterator, Builder, Chain of Responsibility and Facade. The spec requires at least five.

| Category | Pattern | Classes |
|---|---|---|
| Creational | Factory Method | `TaskCreator`, `HumanTaskCreator`, `AutomatedTaskCreator` |
| Creational | Builder *(required)* | `TaskBuilder`, `HumanTaskBuilder`, `ProcessDirector` |
| Structural | Composite | `TaskComponent`, `TaskComposite`, `HumanTask`, `AutomatedTask` |
| Structural | Decorator | `Decorator`, `PriorityDecorator`, `DueSoonDecorator`, `ApprovalPendingDecorator` |
| Structural | Facade *(required)* | `WorkflowEngine` |
| Behavioural | State | `TaskStates` and the four state classes |
| Behavioural | Strategy | `AssignmentRule`, `LeastLoadedAssignment`, `RoleBasedAssignment` |
| Behavioural | Observer *(required)* | `HumanTask` (subject), `TaskObserver`, `CompletionObserver`, `AuditObserver` |
| Behavioural | Iterator *(required)* | `Iterator`, `PendingTaskIterator`, `DueDateIterator` |
| Behavioural | Chain of Responsibility *(required)* | `RequestHandler`, `RoleTierHandler`, `FallbackHandler` |
| Behavioural | Memento | `WorkflowSnapshot`, `SnapshotHistory` |

---