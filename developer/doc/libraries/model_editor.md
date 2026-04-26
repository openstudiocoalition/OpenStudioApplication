# Library: `model_editor` — Generic Model Inspector

> **Source:** `src/model_editor/`  
> **CMake target:** `openstudio_modeleditor`  
> **Back to:** [Architecture Overview](../architecture.md)

## Purpose

`model_editor` provides a generic, IDD schema-driven inspector for any OpenStudio `ModelObject` or `WorkspaceObject`. Rather than hand-coding a bespoke widget for every object type, it interrogates the object's IDD definition at runtime and generates the appropriate input controls (spinboxes, comboboxes, labels) automatically.

It is used:
- By the SketchUp plugin as a standalone model inspector window
- By `openstudio_lib` for the right-column object inspector
- As a debugging tool to inspect arbitrary model objects without domain-specific views

---

## Key Classes

```mermaid
classDiagram
  class InspectorGadget {
    <<QWidget, Nano::Observer>>
    +layoutModel(workspaceObj, recursive, hideChildren)
    +layoutModelObj(modelObj, recursive, hideChildren, locked)
    +removeWorkspaceObject(wObj, type, uuid)
    +toggleGreenButton(isPushButton)
    signals: nameChanged(QString)
    signals: toggleUnitsClicked(bool)
  }
  class IGWidget {
    <<QWidget, Nano::Observer>>
    +sizeHint() QSize
  }
  class InspectorDialog {
    <<QDialog>>
    +setWorkspace(workspace)
    +selectedObjectHandles() vector~Handle~
    signals: workspaceChanged, workspaceObjectAdded, workspaceObjectRemoved
  }
  class AccessPolicyStore {
    <<singleton>>
    +loadFile(path) bool
    +getPolicy(iddObjectType, fieldIndex) AccessPolicy
  }

  InspectorDialog "1" *-- "1" InspectorGadget : embeds
  InspectorGadget --> AccessPolicyStore : consults for field visibility
  InspectorGadget *-- IGWidget : renders fields into
```

| Class | File | Description |
|---|---|---|
| `InspectorGadget` | [InspectorGadget.hpp](../../../src/model_editor/InspectorGadget.hpp) | Core widget. Accepts any `WorkspaceObject` or `ModelObject` and auto-generates input controls from the IDD field definitions. |
| `IGWidget` | [InspectorGadget.hpp](../../../src/model_editor/InspectorGadget.hpp) | Lightweight `QWidget` / `Nano::Observer` base used as the scroll area content for the generated controls. |
| `InspectorDialog` | [InspectorDialog.hpp](../../../src/model_editor/InspectorDialog.hpp) | Modal or modeless dialog that hosts an `InspectorGadget`. Can observe a `Workspace` and keeps the display in sync as objects change. |
| `AccessPolicyStore` | [AccessPolicyStore.hpp](../../../src/model_editor/AccessPolicyStore.hpp) | Singleton that reads an XML policy file to determine whether each IDD field is `FREE` (editable), `LOCKED` (read-only), or hidden. |
| `GithubReleases` | [GithubReleases.hpp](../../../src/model_editor/GithubReleases.hpp) | Checks GitHub releases API for newer application versions. |
| `BridgeClasses` | [BridgeClasses.hpp](../../../src/model_editor/BridgeClasses.hpp) | Adapts OpenStudio nano signals to Qt slots so workspace change notifications reach Qt-connected widgets. |

---

## Field Rendering Rules

`InspectorGadget` generates different Qt widgets for each IDD field type based on the `AccessPolicyStore` policy:

| IDD Field Type | `FREE` policy | `LOCKED` policy |
|---|---|---|
| Real | `QDoubleSpinBox` | `QLabel` |
| Integer | `QSpinBox` | `QLabel` |
| Alpha / String | `QLineEdit` | `QLabel` |
| Choice | `IGComboBox` (custom) | `QLabel` |
| Boolean | `QCheckBox` | `QLabel` |
| Object reference | `QLineEdit` + lookup button | `QLabel` |

---

## External Dependencies

| Dependency | Usage |
|---|---|
| **Qt 6** (`QtWidgets`, `QtCore`) | Widget rendering, event handling |
| **OpenStudio SDK** (`WorkspaceObject`, `ModelObject`, `IddObject`, `IddField`) | IDD schema introspection for field type/name/range |
| **Nano signal-slot** | `Nano::Observer` mixin for low-overhead workspace change notifications |

---

## Internal Dependencies

| Module | Usage |
|---|---|
| `openstudio_qt_utils` | `Application` singleton, `Utilities` (string/UUID/path conversions), `QMetaTypes` (SDK metatype registration), `OSProgressBar` |

> **Note:** `Application`, `OSProgressBar`, `QMetaTypes`, `Utilities`, and `UserSettings` were previously in `model_editor` and have been moved to `openstudio_qt_utils` and `shared_gui_components` respectively.

---

## Patterns & Conventions

- **IDD-driven layout** — the widget layout is entirely data-driven from the OpenStudio IDD schema, requiring no code changes when new object types are added to the SDK.
- **Policy file** — `AccessPolicyStore` reads an XML file at startup. To lock or hide a field for a specific object type, edit the policy file rather than the widget code.
- **Nano signals (not Qt signals)** — `InspectorGadget` and `IGWidget` use `Nano::Observer` (a lightweight signal-slot system that does not require `QObject`) for model change notifications, minimising MOC overhead.
- **SWIG interface** — `ModelEditor.i` and `Qt.i` are SWIG interface files used to generate Ruby and Python bindings for the model editor library (used by the SketchUp plugin).

---

## Class Documentation

- [InspectorGadget](../classes/model_editor/InspectorGadget.md)
- [InspectorDialog](../classes/model_editor/InspectorDialog.md)
- [AccessPolicyStore](../classes/model_editor/AccessPolicyStore.md)
