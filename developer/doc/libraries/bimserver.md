# Library: `bimserver` — BIMserver Integration

> **Source:** `src/bimserver/`  
> **CMake target:** `openstudio_bimserver`  
> **Back to:** [Architecture Overview](../architecture.md)

## Purpose

The `bimserver` module provides optional integration with a [BIMserver](http://bimserver.org/) instance — an open-source BIM server that stores and versions IFC building models.

It enables users to:
1. Log on to a BIMserver instance
2. Browse available projects
3. Select a project revision and download it as an OpenStudio Model (`.osm`)
4. Check in new or updated IFC files from local disk

The module offers both **non-blocking (async)** and **blocking** APIs for all operations, allowing it to be used from GUI code (non-blocking + signals) and scripted CLI-style workflows (blocking with timeout).

---

## Key Classes

```mermaid
classDiagram
  class BIMserverConnection {
    <<QObject>>
    +login(username, password)
    +download(revisionID)
    +loginBlocked(username, password, timeout) bool
    +downloadBlocked(projectID, timeout) optional~QString~
  }
  class ProjectImporter {
    <<QDialog>>
    +run() optional~Model~
  }

  ProjectImporter "1" --> "1" BIMserverConnection : owns and drives
```

---

## Communication Protocol

BIMserver exposes a JSON-over-HTTP API. `BIMserverConnection` uses Qt Network (`QNetworkAccessManager`) to issue HTTP POST requests:

```mermaid
sequenceDiagram
  participant UI as ProjectImporter (UI)
  participant CONN as BIMserverConnection
  participant NET as QNetworkAccessManager
  participant BIM as BIMserver (remote)

  UI->>CONN: login(username, password)
  CONN->>NET: POST /json [login request]
  NET->>BIM: HTTP request
  BIM-->>NET: JSON response (token)
  NET-->>CONN: QNetworkReply::finished
  CONN-->>UI: emit operationSucceeded / errorOccured

  UI->>CONN: getAllProjects()
  CONN->>NET: POST /json [getAllProjects + token]
  BIM-->>NET: JSON project list
  CONN-->>UI: emit listAllProjects(QStringList)

  UI->>CONN: download(revisionID)
  CONN->>NET: POST /json [getFileFromRevision]
  BIM-->>NET: OSM string payload
  CONN-->>UI: emit osmStringRetrieved(osmString)
```

---

## Blocking vs. Non-Blocking API

| Non-blocking (async) | Blocking |
|---|---|
| `login(username, password)` → `operationSucceeded`/`errorOccured` | `loginBlocked(username, password, timeout) → bool` |
| `getAllProjects()` → `listAllProjects(QStringList)` | `getAllProjectsBlocked(timeout) → optional<QStringList>` |
| `download(revisionID)` → `osmStringRetrieved(QString)` | `downloadBlocked(projectID, timeout) → optional<QString>` |
| `createProject(name)` → `operationSucceeded`/`errorOccured` | `createProjectBlocked(name, timeout) → bool` |
| `deleteProject(projectID)` → `operationSucceeded`/`errorOccured` | `deleteProjectBlocked(projectID, timeout) → bool` |
| `checkInIFCFile(projectID, path)` → `operationSucceeded`/`errorOccured` | `checkInIFCFileBlocked(projectID, path, timeout) → bool` |
| `getIFCRevisionList(projectID)` → `listAllIFCRevisions(QStringList)` | `getIFCRevisionListBlocked(projectID, timeout) → optional<QStringList>` |

Blocking variants spin a `QEventLoop` with a `QTimer` for the timeout, making them usable in non-GUI contexts.

---

## External Dependencies

| Dependency | Usage |
|---|---|
| **Qt 6** (`QtNetwork`, `QtCore`, `QtWidgets`) | HTTP client, JSON parsing, dialog UI |
| **OpenStudio SDK** | Parsed from OSM string returned by BIMserver |
| **Boost** | `optional` |

---

## Internal Dependencies

| Module | Usage |
|---|---|
| `openstudio_qt_utils` | `Application` singleton, `Utilities` (string/UUID/path conversions), `QMetaTypes` (SDK metatype registration) |

---

## Patterns & Conventions

- **Signal-based error handling** — errors from network operations surface via `errorOccured(QString)` or `bimserverError()` (emitted when the server is unreachable or misconfigured) rather than exceptions.
- **Operation success reporting** — `operationSucceeded(QString)` carries a message identifying which operation completed (`"login"`, `"createProject"`, `"checkInIFC"`, etc.).
- **`boost::optional` returns** — blocking APIs return `boost::none` on timeout/failure.

---

## Source Reference

Class-level documentation is in the corresponding header files under [`src/bimserver/`](../../../src/bimserver/).
