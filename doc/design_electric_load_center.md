# Design: Electric Load Center Distribution Tab

## 1. Overview

Add a new top-level vertical tab **"Electric Load Center"** for viewing and editing
`ElectricLoadCenterDistribution` (ELCD) objects and their child components
(generators, inverter, storage, storage converter, transformer).

The UI follows the same graphics-based two-level navigation pattern used by the
**Refrigeration** sub-tab under HVAC Systems:
- **Overview** — full system diagram: Utility Grid + global transformers on the
  left, Main Panel in the centre, ELCD cards on the right (one per
  `ElectricLoadCenterDistribution`), each showing a per-ELCD summary including
  its optional `LoadCenterPowerConditioning` transformer at the subpanel boundary
- **Detail** — a full schematic for one selected ELCD, whose layout adapts to the
  chosen Electrical Buss Type, with drop zones for every child component

Reference diagram from EnergyPlus I/O Reference:

```
                                    ┌── Electric Load Center A ──────────────────┐
                                    │  [LCPC Transformer?]  Inverter   Gen Op     │
Utility Grid                        │                                  Gen 1      │
   │                                │                                  Gen 2      │
   │   PowerInFromGrid Transformer  │◀──────── subpanel ──────────────────────── │
   ├──[  Drop  ]──────▶ Main Panel  │
   │                       │        │◀──────── subpanel ──────────────────────── │
   │◀──[  Drop  ]──────────│        │  [LCPC Transformer?]  Inverter  Storage Op  │
      PowerOutToGrid                │                       Converter  Storage    │
                           │        └── Electric Load Center B ──────────────────┘
                           ▼
              Building & HVAC Electric Meters
```

---

## 2. Tab Placement

Insert between `HVAC_SYSTEMS` and `OUTPUT_VARIABLES` in `OSDocument::VerticalTabID`:

```cpp
enum VerticalTabID {
    ...
    HVAC_SYSTEMS,
    ELECTRIC_LOAD_CENTER,   // NEW
    OUTPUT_VARIABLES,
    ...
};
```

Three icon images needed:
```
:/images/on_electric_load_center_tab.png
:/images/off_electric_load_center_tab.png
:/images/disabled_electric_load_center_tab.png
```

---

## 3. Screen 1 — Overview

The overview is a single `QGraphicsScene` rendered in a `QGraphicsView`. It
replicates the EnergyPlus architecture diagram.

### 3.1 Layout Zones

```
┌───────────────────────────────────────────────────────────────────────────────┐
│  Electric Load Centers                                                         │
├────────────────────────┬──────────────────────┬───────────────────────────────┤
│  UTILITY GRID PANEL    │   MAIN PANEL         │  ELCD CARDS                   │
│                        │   (dashed border)    │                               │
│  [PowerInFromGrid]     │                      │  ┌─ ELCD A ─────────── 🔍 ✕ ┐│
│  Drop Transformer      │◀───── subpanel ──────│  │ [LCPC transformer drop]   ││
│         │              │                      │  │ Inverter  GenOp  Gen 1    ││
│         ▼              │◀───── subpanel ──────│  │                  Gen 2    ││
│  [ Main Panel box ]    │                      │  │ ● valid                   ││
│         ▲              │                      │  └───────────────────────────┘│
│         │              │                      │                               │
│  [PowerOutToGrid]      │          │           │  ┌─ ELCD B ─────────── 🔍 ✕ ┐│
│  Drop Transformer      │          ▼           │  │ [LCPC transformer drop]   ││
│                        │   Building & HVAC    │  │ Inverter  Converter       ││
│                        │   Electric Meters    │  │ StorageOp  Storage        ││
│                        │                      │  │ ⚠ invalid                 ││
│                        │                      │  └───────────────────────────┘│
│                        │                      │                               │
│                        │                      │  ┌─────────────────────────┐  │
│                        │                      │  │  Drop ELCD here         │  │
│                        │                      │  └─────────────────────────┘  │
└────────────────────────┴──────────────────────┴───────────────────────────────┘
```

### 3.2 Utility Grid Panel (left column)

Two `OSDropZoneItem`-derived graphics objects for `ElectricLoadCenterTransformer`:

| Widget label        | Enforced `transformerUsage`  |
|---------------------|------------------------------|
| Power In From Grid  | `PowerInFromGrid`            |
| Power Out To Grid   | `PowerOutToGrid`             |

When a transformer is dropped whose `transformerUsage` differs from the target
slot, offer: *"Set this transformer's usage to [X]?"* — Yes / Cancel.

### 3.3 ELCD Cards (right column)

Each card (`ELCDSystemMiniView`) shows, from top to bottom:

1. **Header**: ELCD name + 🔍 zoom-in button + ✕ remove button
2. **Subpanel connector**: labelled arrow pointing left toward the Main Panel
3. **LoadCenterPowerConditioning transformer slot** (optional):
   - If set: shows the transformer name with a ✕ remove button
   - If empty: small dashed drop zone labelled "Drop Transformer (LoadCenterPowerConditioning)"
   - This slot enforces `transformerUsage == LoadCenterPowerConditioning`
4. **Component summary strip** (icons + counts):
   - Generators: count + type icon(s)
   - Inverter: 1 or 0 (shown only if buss type supports it)
   - Storage: 1 or 0 (shown only if buss type supports it)
   - Converter: 1 or 0 (shown only if buss type supports it)
   - Buss type label (abbreviated)
5. **Validity indicator**: green ● (valid) or red ⚠ (invalid)
   - Clicking ⚠ opens the validation message box (see §6)

### 3.4 ELCD Drop Zone

Dashed-border placeholder at the bottom of the right column. Accepts drops of
`ElectricLoadCenterDistribution` objects from the Library. Can also trigger
"Create new ELCD" if an appropriate object type is dropped from an empty library.

---

## 4. Screen 2 — Detail View (per ELCD)

Accessed via 🔍 on an ELCD card. A back button (↑) returns to the overview.

### 4.1 Header Bar

```
[↑ Back]   Name: [_________________________]
Electrical Buss Type:      [DirectCurrentWithInverter ▾]
Generator Operation Scheme:[Baseload ▾]
[✓ Validate]   ● valid
```

- **Buss Type combobox**: changing triggers incompatibility check (§5)
- **Generator Operation Scheme**: `Baseload`, `DemandLimit`, `TrackElectrical`,
  `TrackSchedule`, `TrackMeter`, `FollowThermal`, `FollowThermalLimitElectrical`
  - When `TrackSchedule`: a schedule drop zone appears below
  - When `TrackMeter`: a meter name text field appears below
- **Validate**: runs `validityCheck()` with log capture (§6)

### 4.2 Schematic Diagrams by Buss Type

The schematic area is a `QGraphicsScene`. Components are `OSDropZoneItem`-derived
objects. An empty slot shows a dashed "Drop here" box; a filled slot shows the
object name with a ✕ button. Clicking a filled slot selects the object in the
right-column inspector.

#### `AlternatingCurrent`

```
┌──────────────────────────────────────────────────┐
│                                          subpanel │
│  ┌───────────────────┐                     │      │
│  │ Generators (AC)   │                     ▼      │
│  │ ┌──────────────┐  │          [LCPC Transformer]│
│  │ │ Gen 1   ✕    │  │──────────▶  (drop zone)   │──▶ Main Panel
│  │ ├──────────────┤  │                            │
│  │ │ Drop gen...  │  │                            │
│  │ └──────────────┘  │                            │
│  └───────────────────┘                            │
└──────────────────────────────────────────────────┘
```
Inverter / Storage / Converter slots hidden.

#### `AlternatingCurrentWithStorage`

```
┌────────────────────────────────────────────────────────────────┐
│                                                       subpanel  │
│  ┌───────────────────┐                                  │       │
│  │ Generators (AC)   │   ┌─────────────────────────┐   ▼       │
│  │ ┌──────────────┐  │   │  Storage Operation       │  [LCPC]  │
│  │ │ Gen 1   ✕    │  │──▶│  scheme: [▾]             │──▶ Main  │
│  │ ├──────────────┤  │   │  Min SoC [0.2]  Max [1.0]│  Panel   │
│  │ │ Drop gen...  │  │   │  ┌──────────────────────┐│          │
│  │ └──────────────┘  │   │  │ Storage (AC) [Drop]  ││          │
│  └───────────────────┘   │  └──────────────────────┘│          │
│                           │  (Converter optional)    │          │
│                           └─────────────────────────┘          │
└────────────────────────────────────────────────────────────────┘
```
Inverter slot hidden.

#### `DirectCurrentWithInverter`

```
┌──────────────────────────────────────────────────────────────┐
│                                                     subpanel  │
│  ┌───────────────────┐   ┌─────────────┐               │     │
│  │ Generators (DC)   │   │  Inverter   │               ▼     │
│  │ ┌──────────────┐  │──▶│  [Drop]     │──────────▶  [LCPC] │
│  │ │ Gen 1   ✕    │  │   └─────────────┘            ──▶ Main│
│  │ ├──────────────┤  │                                Panel  │
│  │ │ Drop gen...  │  │                                       │
│  │ └──────────────┘  │                                       │
│  └───────────────────┘                                       │
└──────────────────────────────────────────────────────────────┘
```
Storage / Converter slots hidden.

#### `DirectCurrentWithInverterDCStorage`

```
┌───────────────────────────────────────────────────────────────────────┐
│                                                             subpanel   │
│  ┌───────────────────┐   ┌─────────────────────────┐                  │
│  │ Generators (DC)   │   │  Storage Operation       │  ┌──────────┐   │
│  │ ┌──────────────┐  │   │  scheme: [▾]             │  │ Inverter │   │
│  │ │ Gen 1   ✕    │  │──▶│  Min SoC [0.2]  Max [1.0]│─▶│ [Drop]  │──▶[LCPC]
│  │ ├──────────────┤  │   │  ┌──────────────────────┐│  └──────────┘  ──▶ Main
│  │ │ Drop gen...  │  │   │  │ Storage (DC) [Drop]  ││                  Panel
│  │ └──────────────┘  │   │  ├──────────────────────┤│
│  └───────────────────┘   │  │ Converter (AC→DC)[Drop]│
│                           │  └──────────────────────┘│
│                           └─────────────────────────┘
└───────────────────────────────────────────────────────────────────────┘
```

#### `DirectCurrentWithInverterACStorage`

```
┌───────────────────────────────────────────────────────────────────────┐
│                                                             subpanel   │
│  ┌───────────────────┐   ┌──────────┐   ┌─────────────────────────┐   │
│  │ Generators (DC)   │   │ Inverter │   │  Storage Operation       │   │
│  │ ┌──────────────┐  │──▶│ [Drop]  │──▶│  scheme: [▾]             │──▶[LCPC]
│  │ │ Gen 1   ✕    │  │   └──────────┘   │  ┌──────────────────────┐│  ──▶ Main
│  │ ├──────────────┤  │                  │  │ Storage (AC) [Drop]  ││  Panel
│  │ │ Drop gen...  │  │                  │  └──────────────────────┘│
│  │ └──────────────┘  │                  └─────────────────────────┘
│  └───────────────────┘
└───────────────────────────────────────────────────────────────────────┘
```
Converter slot hidden.

### 4.3 LoadCenterPowerConditioning Transformer

Shown in every buss type at the right edge of the schematic (the "subpanel"
boundary toward the Main Panel). Single drop zone for
`ElectricLoadCenterTransformer` with `transformerUsage == LoadCenterPowerConditioning`.
If empty, shows a small dashed box labelled "LCPC Transformer (optional)".

### 4.4 Storage Operation Fields (conditional)

Visible only when buss type includes storage. Shown inline in the Storage
Operation box in the schematic:

| Scheme | Extra fields shown |
|--------|--------------------|
| `TrackFacilityElectricDemandStoreExcessOnSite` | Min/Max SoC only |
| `TrackMeterDemandStoreExcessOnSite` | Min/Max SoC + Track Meter Name |
| `TrackChargeDischargeSchedules` | Min/Max SoC + Design Charge Power + Design Discharge Power + Charge Schedule + Discharge Schedule |
| `FacilityDemandLeveling` | Min/Max SoC + Design Charge Power + Design Discharge Power + Utility Demand Target |

`TrackChargeDischargeSchedules` and `FacilityDemandLeveling` also require a
Storage Converter object (shown as an error in the validity indicator if absent).

---

## 5. Buss Type Change — Incompatibility Handling

`validityCheck()` treats incompatible-but-present components as **Warnings** (they
won't be translated to E+ but are not fatal). Only missing required components
(inverter for DC buss, storage for storage buss) return `false`.

The app should therefore:

1. When the buss type combobox changes, compute the set of components that are
   present but incompatible with the new type.
2. If any such components exist, show a confirmation dialog:

```
"Changing the Electrical Buss Type to 'AlternatingCurrent' will make the
following objects incompatible (they will be ignored during simulation):

  • Inverter: 'SimpleInverter_1'
  • Storage: 'LiIonBattery_A'

Would you like to remove them from the model now for clarity?

[Remove & Switch]   [Keep & Switch]   [Cancel]
```

3. On **Remove & Switch**: call `resetInverter()` / `resetElectricalStorage()` /
   `resetStorageConverter()` as appropriate, then `setElectricalBussType()`.
4. On **Keep & Switch**: call `setElectricalBussType()` only. The validity
   indicator will turn ⚠ due to the warning.
5. On **Cancel**: revert the combobox, no model changes.

Incompatibility table (components made incompatible by target buss type):

| Target Buss Type | Incompatible if present |
|------------------|------------------------|
| `AlternatingCurrent` | inverter, storage, converter |
| `AlternatingCurrentWithStorage` | inverter |
| `DirectCurrentWithInverter` | storage, converter |
| `DirectCurrentWithInverterDCStorage` | — (only adds requirements) |
| `DirectCurrentWithInverterACStorage` | converter |

---

## 6. Validation

### Log Capture

`validityCheck()` uses `LOG(Info/Warn/Error, ...)` from the OpenStudio logging
system. Capture messages using `openstudio::StringStreamLogSink`:

```cpp
openstudio::StringStreamLogSink sink;
sink.setLogLevel(openstudio::Info);   // capture Info, Warn, Error
bool valid = elcd.validityCheck();
std::vector<openstudio::LogMessage> msgs = sink.logMessages();
```

Build the display string from `msgs`, colouring by log level:
- `Info` → grey
- `Warn` → orange/yellow
- `Error` → red

### Automatic (validity indicator on cards and detail header)

After every model change affecting an ELCD, debounce via
`QTimer::singleShot(0, ...)` then call `validityCheck()` and update the indicator
icon (● green / ⚠ red).

### On-demand (Validate button in detail header)

Opens a `QMessageBox` (or a custom non-modal dialog) showing all captured log
messages. If `valid == true` and no messages: "✓ Configuration is valid."

---

## 7. Right Column (Library / My Model)

Configure via `MainRightColumnController::configureForElectricLoadCenterSubTab()`.

| Active context | Library filters to |
|----------------|--------------------|
| Overview — no selection | `ElectricLoadCenterDistribution`, `ElectricLoadCenterTransformer` |
| Overview — global transformer slot active | `ElectricLoadCenterTransformer` (usage: PowerInFromGrid or PowerOutToGrid) |
| Detail — generator list | all `Generator` subtypes |
| Detail — inverter slot active | `ElectricLoadCenterInverterSimple`, `ElectricLoadCenterInverterPVWatts`, `ElectricLoadCenterInverterLookUpTable` |
| Detail — storage slot active | `ElectricLoadCenterStorageSimple`, `ElectricLoadCenterStorageLiIonNMCBattery` |
| Detail — converter slot active | `ElectricLoadCenterStorageConverter` |
| Detail — LCPC transformer slot active | `ElectricLoadCenterTransformer` (usage: LoadCenterPowerConditioning) |

Clicking a filled component in the detail view → selects it in My Model tab and
shows its fields in the inspector.

---

## 8. Architecture

### 8.1 New Files

```
src/openstudio_lib/
  ElectricLoadCenterDistributionTabController.hpp/.cpp
      Extends MainTabController.
      Constructor creates ELCDTabView, owns ELCDController, wires OSDocument signals.

  ElectricLoadCenterDistributionTabView.hpp/.cpp
      Extends MainTabView (no sub-tab buttons; navigation is internal to the scene).

  ElectricLoadCenterDistributionController.hpp/.cpp
      Main domain controller (analogous to RefrigerationController).
      - Owns ELCDView, m_gridScene, m_detailScene, m_listController
      - Handles zoom in/out, drop events, model mutations, dirty+refresh cycle
      - m_currentELCD: boost::optional<model::ElectricLoadCenterDistribution>

  ElectricLoadCenterDistributionGraphicsItems.hpp/.cpp
      All QGraphicsObject subclasses:
        ELCDView                       QWidget, header + QGraphicsView
        ELCDSystemGridView             grid layout of ELCDSystemMiniView items
        ELCDSystemMiniView             one ELCD card in overview (350×420 px)
        ELCDDropZoneView               "Drop ELCD here" placeholder
        ELCDTransformerDropZoneView    global transformer drop zone (used twice in overview)
        ELCDSystemDetailView           full schematic for one ELCD
        ELCDGeneratorsView             scrollable list of generators
        ELCDGeneratorDetailView        one generator row in the list
        ELCDComponentSlotView          generic single-object drop zone
                                       (inverter, storage, converter, LCPC transformer)
        ELCDStorageOperationView       Storage Operation box with scheme combobox
                                       and conditional sub-fields

  ElectricLoadCenterDistributionScene.hpp/.cpp
      Minimal QGraphicsScene subclass.
```

### 8.2 Modified Files

| File | Change |
|------|--------|
| `OSDocument.hpp` | Add `ELECTRIC_LOAD_CENTER` to `VerticalTabID` |
| `OSDocument.cpp` | `createTabButtons()`: add button; `createTab()`: add case |
| `MainRightColumnController.hpp/.cpp` | Add `configureForElectricLoadCenterSubTab()` |
| `CMakeLists.txt` | Add new source/header files to `openstudio_lib` target |
| `src/openstudio_app/Resources.qrc` | Register new tab icons |
| `openstudiolib.qss` | Any new widget-specific styles |

### 8.3 Class Hierarchy

```
OSDocument
 └─creates─▶ ElectricLoadCenterDistributionTabController : MainTabController
               ├─owns─▶ ElectricLoadCenterDistributionTabView : MainTabView
               └─owns─▶ ElectricLoadCenterDistributionController : QObject
                           ├─owns─▶ ELCDView : QWidget
                           │          └─ QGraphicsView (shows m_gridScene or m_detailScene)
                           ├─owns─▶ m_gridScene : ELCDScene
                           │          ├─ ELCDSystemGridView (GridLayoutItem)
                           │          │    ├─ n × ELCDSystemMiniView
                           │          │    └─ ELCDDropZoneView
                           │          ├─ UtilityGridPanel (left column, static)
                           │          │    ├─ ELCDTransformerDropZoneView (PowerInFromGrid)
                           │          │    └─ ELCDTransformerDropZoneView (PowerOutToGrid)
                           │          └─ MainPanelItem (static, arrows to/from ELCD cards)
                           ├─owns─▶ m_detailScene : ELCDScene
                           │          └─ ELCDSystemDetailView
                           │               ├─ ELCDGeneratorsView
                           │               ├─ ELCDComponentSlotView  (inverter)
                           │               ├─ ELCDStorageOperationView (storage + scheme)
                           │               │    └─ ELCDComponentSlotView (storage object)
                           │               ├─ ELCDComponentSlotView  (converter)
                           │               └─ ELCDComponentSlotView  (LCPC transformer)
                           └─owns─▶ m_listController : ELCDListController : OSListController
```

### 8.4 Key Signals / Slots

```
ELCDSystemMiniView
  signal zoomInClicked(Handle)            → ELCDController::zoomInOnELCD(Handle)
  signal removeClicked(Handle)            → ELCDController::onRemoveELCD(Handle)
  signal validityIndicatorClicked(Handle) → ELCDController::onShowValidityReport(Handle)
  signal lcpcTransformerDropped(OSItemId, Handle elcd)
                                          → ELCDController::onLCPCTransformerDrop(...)

ELCDSystemDetailView
  signal backClicked()                    → ELCDController::zoomOutToGrid()
  signal bussTypeChangeRequested(QString) → ELCDController::onBussTypeChangeRequested(...)
  signal generatorDropped(OSItemId)       → ELCDController::onGeneratorDrop(...)
  signal generatorRemoveClicked(OSItemId) → ELCDController::onGeneratorRemove(...)
  signal inverterDropped(OSItemId)        → ELCDController::onInverterDrop(...)
  signal storageDropped(OSItemId)         → ELCDController::onStorageDrop(...)
  signal converterDropped(OSItemId)       → ELCDController::onConverterDrop(...)
  signal lcpcTransformerDropped(OSItemId) → ELCDController::onLCPCTransformerDrop(...)
  signal componentRemoveClicked(OSItemId) → ELCDController::onComponentRemove(...)
  signal validateClicked()                → ELCDController::onValidate()

ELCDTransformerDropZoneView (overview, global)
  signal transformerDropped(OSItemId, QString usage)
                                          → ELCDController::onGlobalTransformerDrop(...)

ElectricLoadCenterDistributionController
  signal modelObjectSelected(OptionalModelObject, bool readOnly)
                                          → MainRightColumnController::inspectModelObject()
  signal dropZoneItemSelected(OSItem*, bool readOnly)
                                          → MainRightColumnController::inspectModelObjectByItem()
```

### 8.5 Refresh / Dirty Pattern

```cpp
// Connect model onChange to refresh
model.getImpl<detail::Model_Impl>()->onChange.connect<
    ElectricLoadCenterDistributionController,
    &ElectricLoadCenterDistributionController::refresh>(this);

void ElectricLoadCenterDistributionController::refresh() {
    m_dirty = true;
    QTimer::singleShot(0, this, &ElectricLoadCenterDistributionController::refreshNow);
}

void ElectricLoadCenterDistributionController::refreshNow() {
    if (!m_dirty) return;
    m_dirty = false;
    if (m_currentELCD) {
        refreshDetailView(*m_currentELCD);
    } else {
        refreshGridView();
    }
}
```

---

## 9. Component Compatibility Table

| Buss Type | Generators | Inverter | Storage type | Converter | LCPC Xfmr |
|-----------|-----------|----------|--------------|-----------|-----------|
| `AlternatingCurrent` | AC | ✗ hide | ✗ hide | ✗ hide | optional |
| `AlternatingCurrentWithStorage` | AC | ✗ hide | AC | optional | optional |
| `DirectCurrentWithInverter` | DC | ✓ required | ✗ hide | ✗ hide | optional |
| `DirectCurrentWithInverterDCStorage` | DC | ✓ required | DC | optional | optional |
| `DirectCurrentWithInverterACStorage` | DC | ✓ required | AC | ✗ hide | optional |

"AC generators" = `GeneratorMicroTurbine`, `GeneratorWindTurbine`, `GeneratorFuelCell`
"DC generators" = `GeneratorPhotovoltaic`, `GeneratorPVWatts`

---

## 10. Open Questions (resolved)

| # | Question | Resolution |
|---|----------|------------|
| 1 | Tab icon | Create new artwork; placeholder: reuse/adapt existing electrical symbol |
| 2 | Generator sub-types in Library | List all types individually under an "Electric Load Center" category |
| 3 | LoadCenterPowerConditioning placement | Shown in **both** overview (mini drop zone per card) and detail view (LCPC slot at right edge of schematic) |
| 4 | Multiple inverters | SDK: max 1 per ELCD. Single drop zone in detail. |
| 5 | `validityReport()` API | Use `StringStreamLogSink` to capture `LOG()` output from `validityCheck()` |
| 6 | Generator DC/AC classification | Hard-coded type map in app (see table in §9); can be revisited if SDK adds an API |
| 7 | Inspector for generators | Use `InspectorGadget` (same mechanism as HVAC component inspector) via right column |
| 8 | Storage operation fields placement | Inline in the schematic's Storage Operation box (not a separate panel) |

---

## 11. Implementation Phases

| Phase | Scope | Key deliverable |
|-------|-------|-----------------|
| **1** | Scaffolding | Tab registration, empty view, vertical tab button, CMake |
| **2** | Overview grid | ELCD cards, add/remove ELCD, zoom navigation wired up |
| **3** | Overview left panel | Global transformer drop zones, Main Panel and arrows |
| **4** | Detail — `DirectCurrentWithInverter` | Most common case; generators list + inverter drop zone |
| **5** | Detail — remaining 4 buss types | AlternatingCurrent, ACWithStorage, DCWithInverterDCStorage, DCWithInverterACStorage |
| **6** | LCPC transformer | In overview cards and detail schematic |
| **7** | Buss type change dialog | Incompatibility detection and Remove/Keep/Cancel dialog |
| **8** | Storage Operation inline fields | Scheme combobox + conditional sub-fields |
| **9** | Right-column integration | Library filtering per active context, inspector for components |
| **10** | Validation | StringStreamLogSink capture, validity indicator, Validate button dialog |
