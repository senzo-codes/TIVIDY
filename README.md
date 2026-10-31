# TIVIDY

**A generic workflow management system, written in C++11.**

---

## Overview

TIVIDY is an extensible, enterprise-grade engine for defining, modelling, executing and monitoring complex organizational processes.

In modern organizations, work rarely consists of isolated tasks. It moves through structured pipelines involving human participants, automated systems, decision gates, conditional approvals and external dependencies. Without a central engine, these processes fall back to email chains, spreadsheets and manual intervention.

TIVIDY replaces that with one execution engine that:

- enforces business rules
- routes work automatically
- tracks progress in real time
- keeps a full audit trail
- handles failures cleanly

---

## Design Patterns

TIVIDY follows object-oriented principles and Gang of Four patterns, which removes the need for large `if/else` and `switch` chains and keeps the system flexible.

| Pattern | Purpose |
|---|---|
| **Composite** | Represents hierarchical work (atomic tasks and parallel subtasks) behind one interface. |
| **State** | Encapsulates state-specific behaviour and governs valid transitions in the task lifecycle. |
| **Chain of Responsibility** | Routes approval requests step by step up through the role levels. |
| **Observer** | Decouples event propagation so the audit log and workflow progression react to task changes. |
| **Decorator** | Attaches labels to tasks, such as priority, due soon and approval pending, without subclassing. |
| **Factory Method** | Separates the creation of human and automated tasks. |
| **Builder** | Builds tasks step by step, apart from their internal representation. |
| **Memento** | Captures and restores snapshots of running workflows without breaking encapsulation. |
| **Iterator** | Traverses open tasks and tasks by due date without exposing their storage. |
| **Facade** | Gives the UI a single entry point, `WorkflowEngine`. |
| **Strategy** | Swaps how tasks are assigned, such as least loaded and role based. |

---

## Demonstration Scenario

To show the generic engine working in a real setting, TIVIDY is demonstrated through a concrete organizational process.

The scenario exercises the core features:

- multi-level approvals
- conditional routing
- automated steps
- parallel subtasks

Together they show the versatility and robustness of the generic system.