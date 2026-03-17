# Class: `GeometryEditorController`

> **Module:** `openstudio_lib`  
> **Header:** [src/openstudio_lib/GeometryEditorController.hpp](../../../../src/openstudio_lib/GeometryEditorController.hpp)  
> **Library doc:** [libraries/openstudio_lib.md](../../libraries/openstudio_lib.md)

## Purpose

`GeometryEditorController` manages the Geometry tab's interactive 3D editor. The editor is implemented as a Qt WebEngine view hosting a JavaScript application that renders and edits OpenStudio geometry. The controller bridges the C++ model layer and the JavaScript geometry engine via `OSWebEnginePage`.

---

## Class Diagram

```mermaid
classDiagram
  class OSQObjectController {
    <<base>>
  }
  class GeometryEditorController {
    +GeometryEditorController(isIP, model)
    +view() QWidget*
    #m_view: QWidget*
  }
  class GeometryEditorView {
    <<QWidget>>
    +webEngineView() QWebEngineView*
  }
  class OSWebEnginePage {
    <<QWebEnginePage>>
    +setModel(model)
    +getModel() Model
    signals: modelChanged(Model)
  }

  OSQObjectController <|-- GeometryEditorController
  GeometryEditorController "1" *-- "1" GeometryEditorView : creates and stores as m_view
  GeometryEditorView "1" *-- "1" OSWebEnginePage : hosts
```

---

## Constructor

| Parameter | Type | Description |
|---|---|---|
| `isIP` | `bool` | Whether to display dimensions in imperial units |
| `model` | `const model::Model&` | The OpenStudio model whose geometry is displayed |

---

## Key Public Methods

| Method | Returns | Description |
|---|---|---|
| `view()` | `QWidget*` | The view widget (`GeometryEditorView`) to embed in the tab |

---

## Architecture Note

The geometry editor is built around a JS/C++ bridge:

```mermaid
flowchart LR
  CPP["C++ Model\n(openstudio::model::Model)"]
  CTRL["GeometryEditorController"]
  PAGE["OSWebEnginePage\n(QWebEnginePage)"]
  WEB["Qt WebEngine\n(Chromium)"]
  JS["JavaScript\nGeometry Engine\n(Three.js / custom)"]
  GLTF["GLTF\n(TinyGLTF)"]

  CPP --"serialize to GLTF"--> GLTF
  GLTF --> PAGE
  PAGE --"postMessage / QWebChannel"--> WEB
  WEB --> JS
  JS --"user edits"--> WEB
  WEB --"runJavaScript callback"--> PAGE
  PAGE --"deserialize GLTF"--> CPP
  CTRL --> PAGE
```

Model geometry is serialised to GLTF JSON and injected into the JavaScript engine; user edits in 3D are sent back as GLTF diffs, which `OSWebEnginePage` deserialises back into OpenStudio model objects.

See [OSWebEnginePage](OSWebEnginePage.md) for the JS bridge API details.
