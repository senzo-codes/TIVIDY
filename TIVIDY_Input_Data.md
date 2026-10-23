# TIVIDY: Input Data

Each example has two cases. The prototype gives each new instance the next case in turn, so the first instance gets case 1, the second gets case 2, and so on.

Click a task on a person's dashboard to see its case data in a pop-up.

Participant names are left blank, so you can add them in the Admin tab.

---

## 1. Car insurance claim

**Participants**

| Level | Role | Name |
|---|---|---|
| 1 | Claims Handler | |
| 2 | Senior Assessor | |
| 3 | Claims Manager | |

### Case 1: fraud flag set (goes to Under Investigation)

| Field | Value |
|---|---|
| claimNumber | CLM-2026-0001 |
| claimantName | Thandi Nkosi |
| policyNumber | POL-88421 |
| incidentDate | 2026-10-12 |
| vehicleRegistration | GP 123 456 |
| incidentDescription | Rear bumper damaged in a car park |
| fraudFlag | yes |

### Case 2: no fraud flag (goes to Approved)

| Field | Value |
|---|---|
| claimNumber | CLM-2026-0002 |
| claimantName | Pieter van der Merwe |
| policyNumber | POL-90114 |
| incidentDate | 2026-10-15 |
| vehicleRegistration | GP 987 654 |
| incidentDescription | Hail damage to bonnet and roof |
| fraudFlag | no |

**Expected result:** Case 1 ends in Paid, after the investigation and the Clear claim step. Case 2 ends in Paid, after Review claim goes to Approved.

---

## 2. Manufacturing production order

**Participants**

| Level | Role | Name |
|---|---|---|
| 1 | Operator | |
| 2 | Line Lead | |
| 3 | Quality Inspector | |
| 4 | Production Manager | |
| 5 | Plant Director | |

### Case 1: urgent, high value, inspection passes (goes to Shipped, then Closed)

| Field | Value |
|---|---|
| orderNumber | PO-2026-0457 |
| customerOrder | CUS-9913 |
| productCode | PRD-GEAR-22 |
| quantity | 500 |
| dueDate | 2026-11-20 |
| orderValue | 240000 |
| urgent | yes |
| highValue | yes |
| inspectionResult | pass |

### Case 2: not urgent, low value, inspection fails (goes to Rework or Scrap)

| Field | Value |
|---|---|
| orderNumber | PO-2026-0458 |
| customerOrder | CUS-9920 |
| productCode | PRD-GEAR-22 |
| quantity | 120 |
| dueDate | 2026-11-05 |
| orderValue | 38000 |
| urgent | no |
| highValue | no |
| inspectionResult | fail |

**Notes**
- `highValue` is yes when the order value is R100 000 or more.
- `inspectionResult` is recorded during the Inspect quality task. In the demo it is set when the order starts, so the pass or fail path is visible straight away.
- The quantity and the other fields are shown for the case. The engine does not use them to choose a path.

---
