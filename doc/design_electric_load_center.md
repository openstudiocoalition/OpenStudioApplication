# Design: Electric Load Center Distribution Tab

## 1. Overview

Add a new top-level vertical tab **"Electric Load Center"** for viewing and editing
`ElectricLoadCenterDistribution` (ELCD) objects and their child components
(generators, inverters, storage, storage converter, transformer).

The UI follows the same graphics-based, two-level navigation pattern used by the
**Refrigeration** sub-tab under HVAC Systems:
- **Overview** — a grid of ELCD "cards" plus global transformer drop zones
- **Detail** — a schematic diagram for one selected ELCD, whose layout adapts to
  the chosen Electrical Buss Type

---

## 2. Tab Placement

Insert the tab between `HVAC_SYSTEMS` and `OUTPUT_VARIABLES` in `OSDocument::VerticalTabID`:

```cpp
enum VerticalTabID {
    ...
    HVAC_SYSTEMS,
    ELECTRIC_LOAD_CENTER,   // NEW
    OUTPUT_VARIABLES,
    ...
};
```

Three icon images are needed (on / off / disabled):
```
:/images/on_electric_load_center_tab.png
:/images/off_electric_load_center_tab.png
:/images/disabled_electric_load_center_tab.png
```

---

## 3. Screen 1 — Overview (System Grid)

```
┌─────────────────────────────────────────────────────────────────┐
│  [←] Electric Load Centers                                      │
│─────────────────────────────────────────────────────────────────│
│  Global Transformers                                             │
│  ┌───────────────────────┐  ┌───────────────────────┐           │
│  │  PowerInFromGrid      │  │  PowerOutToGrid        │           │
│  │  [ Drop Transformer ] │  │  [ Drop Transformer ]  │           │
│  └───────────────────────┘  └───────────────────────┘           │
│─────────────────────────────────────────────────────────────────│
│  ┌──────────────────────┐  ┌──────────────────────┐             │
│  │ ELCD A          🔍 ✕ │  │ ELCD B          🔍 ✕ │             │
│  │ 3 generators         │  │ 2 generators         │             │
│  │ 1 inverter           │  │ 1 inverter           │             │
│  │ 0 converters         │  │ 0 converters         │             │
│  │ 1 storage            │  │ 0 storage            │             │
│  │ ~DC w/ Inverter~     │  │ ~DC w/ Inverter~     │             │
│  │ ● valid              │  │ ⚠ invalid            │             │
│  └──────────────────────┘  └──────────────────────┘             │
│                                                                   │
│  ┌─────────────────────────────────────────┐                    │
│  │  Drop  Electric Load Center Distribution│  (dashed outline)  │
│  └─────────────────────────────────────────┘                    │
└─────────────────────────────────────────────────────────────────┘
```

### 3.1 Global Transformer Panel

At the top of the overview, **two drop zones** for `ElectricLoadCenterTransformer`
objects, differentiated by their `transformerUsage` field:

| Drop Zone Label     | Enforced `transformerUsage` value |
|---------------------|-----------------------------------|
| Power In From Grid  | `PowerInFromGrid`                 |
| Power Out To Grid   | `PowerOutToGrid`                  |

Accepting a drop:
- If the dropped transformer has a different `transformerUsage`, show a dialog:
  *"This transformer is set to [X]. Set it to [Y] to use it here?"* Yes / Cancel.
- If a transformer is already assigned, replacing it moves the old one back to
  unassigned (it loses its `transformerUsage` relationship with the ELCD system).

Note: these transformers are **model-level** (not per-ELCD); they appear at this
level because `ElectricLoadCenterTransformer` with `PowerInFromGrid` or
`PowerOutToGrid` usage is not owned by any specific ELCD.

### 3.2 ELCD Cards

Each card (equivalent to `RefrigerationSystemMiniView`) displays:
- **Name** (editable on double-click, or via inspector)
- **Generator count**
- **Inverter count** (0 or 1)
- **Storage count** (0 or 1)
- **Storage Converter count** (0 or 1)
- **Buss type** — abbreviated string (e.g. *DC w/ Inverter + DC Storage*)
- **DC/AC icon** — waveform symbol (AC ∿ vs DC ─)
- **Validity indicator** — green ● (valid) or red ⚠ (invalid), computed from
  `validityCheck()`; clicking the red indicator opens a message box with the
  validation log
- **🔍 button** — zoom into the Detail view for this ELCD
- **✕ button** — remove this ELCD from the model (confirmation dialog)

### 3.3 ELCD Drop Zone

A dashed-border placeholder card at the end of the grid accepts drops of
`ElectricLoadCenterDistribution` objects from the Library panel (right column).
It can also serve as the target to create a new ELCD (via an "+ Add" button in
the toolbar or by dropping).

---

## 4. Screen 2 — Detail View (per ELCD)

Accessed by clicking 🔍 on an ELCD card. A back button (↑) returns to the
overview.

### 4.1 Header Bar

```
[↑ Back]  ELCD Name: [___________]
Electrical Buss Type: [DirectCurrentWithInverter ▾]
Generator Operation:  [Baseload ▾]
[✓ Validate]   ● valid / ⚠ invalid
```

- **Buss Type combobox** — changing triggers the incompatibility check (§5)
- **Generator Operation combobox** — `Baseload`, `DemandLimit`, `TrackElectrical`,
  `TrackSchedule`, `TrackMeter`, `FollowThermal`, `FollowThermalLimitElectrical`
- **Validate button** — runs `validityCheck()` and opens a dialog with the result
  and any log messages
- **Validity indicator** — auto-refreshes on any model change

### 4.2 Schematic Diagrams by Buss Type

The central area renders a schematic that adapts to `electricalBussType()`.
Components are `OSDropZoneItem`-derived graphics objects.  An empty slot shows a
dashed "Drop here" box; a filled slot shows the object name with a remove (✕) button.

#### `AlternatingCurrent`
```
┌──────────────┐         ┌─────────────────┐
│  Generators  │────────▶│  Main Panel     │
│  (AC)        │         │  (building load)│
└──────────────┘         └─────────────────┘
```
Components shown: Generators list only.
Inverter, Storage, Converter slots are **hidden**.

#### `AlternatingCurrentWithStorage`
```
┌──────────────┐         ┌──────────────────────────────────┐
│  Generators  │────────▶│  Storage Operation               │────▶ Main Panel
│  (AC)        │         │  scheme: [TrackFacility... ▾]    │
└──────────────┘         └───────────────┬──────────────────┘
                                         │
                                ┌────────▼─────────┐
                                │  Storage (AC)    │
                                │  [ Drop / name ] │
                                └──────────────────┘
```
Components: Generators, Storage.
Converter slot shown (optional, for AC↔DC charging if needed).
Inverter slot **hidden**.

#### `DirectCurrentWithInverter`
```
┌──────────────┐     ┌──────────┐         ┌─────────────┐
│  Generators  │────▶│ Inverter │────────▶│  Main Panel │
│  (DC)        │     │ [Drop]   │         └─────────────┘
└──────────────┘     └──────────┘
```
Components: Generators, Inverter.
Storage, Converter slots **hidden**.

#### `DirectCurrentWithInverterDCStorage`
```
┌──────────────┐   ┌──────────────────────────────────┐   ┌──────────┐
│  Generators  │──▶│  Storage Operation               │──▶│ Inverter │──▶ Main Panel
│  (DC)        │   │  scheme: [TrackFacility... ▾]    │   │ [Drop]   │
└──────────────┘   └────────────────┬─────────────────┘   └──────────┘
                                    │
                           ┌────────▼──────────┐
                           │  Storage (DC)     │
                           │  [ Drop / name ]  │
                           └────────┬──────────┘
                                    │
                           ┌────────▼──────────┐
                           │  Converter (AC→DC)│
                           │  [ Drop / name ]  │
                           └───────────────────┘
```
Components: Generators, Inverter, Storage (DC), Converter.

#### `DirectCurrentWithInverterACStorage`
```
┌──────────────┐   ┌──────────┐   ┌──────────────────────────────────┐
│  Generators  │──▶│ Inverter │──▶│  Storage Operation               │──▶ Main Panel
│  (DC)        │   │ [Drop]   │   │  scheme: [TrackFacility... ▾]    │
└──────────────┘   └──────────┘   └────────────────┬─────────────────┘
                                                    │
                                           ┌────────▼──────────┐
                                           │  Storage (AC)     │
                                           │  [ Drop / name ]  │
                                           └───────────────────┘
```
Components: Generators, Inverter, Storage (AC).
Converter slot **hidden**.

### 4.3 Generators Sub-Panel

The Generators panel appears in all buss types.  It behaves like
`RefrigerationCompressorView`:
- Scrollable vertical list of assigned generators, each showing:
  - Type icon (PV ☀, MicroTurbine ⚙, WindTurbine 🌬, FuelCell ⚡, etc.)
  - Name
  - ✕ remove button
  - Click → inspects object in the right column
- A "Drop generator here" zone at the bottom
- The right-column Library is filtered to show generator types

### 4.4 Storage Operation Fields (conditional)

When `electricalBussType` includes storage, show below the storage drop zone:
- **Storage Operation Scheme** combobox
- Conditional schedule drop zones that appear/hide based on the scheme:
  - `TrackChargeDischargeSchedules`: charge fraction schedule + discharge fraction schedule
  - `FacilityDemandLeveling`: utility demand target fraction schedule
- **Min/Max State of Charge** spinboxes (0.0–1.0)

---

## 5. Buss Type Change — Incompatibility Handling

When the user changes `electricalBussType` via the combobox in the detail view,
check which existing components become incompatible:

| Old → New                          | Components to remove                     |
|------------------------------------|------------------------------------------|
| Any DC type → `AlternatingCurrent` | inverter, storage, converter             |
| Any DC type → `ACWithStorage`      | inverter, converter (keep storage if AC) |
| `DCWithInverter` → `DCWithInverterDCStorage` | none (add slots)           |
| `DCWithInverterDCStorage` → `DCWithInverter` | storage, converter          |
| `DCWithInverterDCStorage` → `DCWithInverterACStorage` | converter         |
| `ACWithStorage` → `DC` types       | storage (unless compatible), converter   |

If any incompatible components exist, before applying the change show:

```
"Changing the Electrical Buss Type to 'AlternatingCurrent' will remove
the following components that are incompatible with this buss type:

  • Inverter: SimpleInverter_1
  • Storage: LiIonBattery_A

Do you want to proceed?"

[Proceed]   [Cancel]
```

On **Proceed**: remove the listed child objects, then call `setElectricalBussType()`.
On **Cancel**: revert the combobox to the previous value, no model changes.

---

## 6. Validation

### Automatic (per card in overview)
- After every model change affecting an ELCD, call `validityCheck()` and update
  the card's indicator icon (green ● / red ⚠).
- Use a debounce timer (like the Refrigeration `m_dirty` + `QTimer::singleShot`
  pattern) to avoid redundant recalculations.

### On-demand (detail view)
- The **"✓ Validate"** button in the detail header runs `validityCheck()` and
  opens a `QMessageBox` showing:
  - Pass: "✓ This Electric Load Center Distribution is valid."
  - Fail: "⚠ Validity issues found:\n<log messages from validityReport()>"

---

## 7. Right Column (Library / My Model)

When the ELCD tab is active, configure the right column via
`MainRightColumnController::configureForElectricLoadCenterSubTab()`:

| Context                        | Library shows                                     |
|--------------------------------|---------------------------------------------------|
| Overview (no ELCD selected)    | `ElectricLoadCenterDistribution`, `ElectricLoadCenterTransformer` |
| Detail — generator zone active | All `Generator` subtypes                          |
| Detail — inverter zone active  | All `Inverter` subtypes                           |
| Detail — storage zone active   | All `ElectricalStorage` subtypes                  |
| Detail — converter zone active | `ElectricLoadCenterStorageConverter`              |
| Detail — transformer zone      | `ElectricLoadCenterTransformer`                   |

Clicking any assigned component in the detail view selects it in the right column
(My Model tab) and shows its fields in the inspector.

---

## 8. Architecture

### 8.1 New Files

```
src/openstudio_lib/
  ElectricLoadCenterDistributionController.hpp/.cpp
      Main domain controller (analogous to RefrigerationController).
      Owns the view, scene, list controller; handles drop events and
      model mutations; manages overview ↔ detail navigation.

  ElectricLoadCenterDistributionGraphicsItems.hpp/.cpp
      All QGraphicsObject subclasses:
        ELCDView                      - top-level QWidget (header + QGraphicsView)
        ELCDSystemGridView            - grid layout of ELCDSystemMiniView items
        ELCDSystemMiniView            - one ELCD card in overview
        ELCDSystemDetailView          - one ELCD schematic in detail mode
        ELCDGeneratorsView            - scrollable list of generators
        ELCDGeneratorDetailView       - one generator row
        ELCDComponentView             - generic single-item drop zone (inverter /
                                        storage / converter)
        ELCDTransformerDropZoneView   - global transformer drop zone (used twice)
        ELCDDropZoneView              - "Drop ELCD here" placeholder

  ElectricLoadCenterDistributionScene.hpp/.cpp
      Minimal QGraphicsScene subclass (like RefrigerationScene).

  ElectricLoadCenterDistributionTabController.hpp/.cpp
      Extends MainTabController.  Creates the tab view, owns the domain
      controller, wires up signals for OSDocument.

  ElectricLoadCenterDistributionTabView.hpp/.cpp
      Extends MainTabView.  No sub-tabs needed (navigation is internal to
      the graphics view, not MainTabView buttons).
```

### 8.2 Modified Files

| File | Change |
|------|--------|
| `OSDocument.hpp` | Add `ELECTRIC_LOAD_CENTER` to `VerticalTabID` |
| `OSDocument.cpp` | `createTabButtons()`: add button; `createTab()`: add case |
| `MainRightColumnController.hpp/.cpp` | Add `configureForElectricLoadCenterSubTab()` |
| `CMakeLists.txt` | Add new source/header files |
| `src/openstudio_app/Resources/openstudiolib.qss` | Add any new widget styles |
| Tab icon images | Add on/off/disabled PNG images to resources |

### 8.3 Class Diagram (simplified)

```
OSDocument
  └─creates─▶ ElectricLoadCenterDistributionTabController : MainTabController
                ├─owns─▶ ElectricLoadCenterDistributionTabView : MainTabView
                └─owns─▶ ElectricLoadCenterDistributionController : QObject
                            ├─owns─▶ ELCDView : QWidget
                            │          └─contains─▶ QGraphicsView
                            │                          └─shows─▶ m_gridScene / m_detailScene
                            ├─owns─▶ m_gridScene : QGraphicsScene
                            │          └─contains─▶ ELCDSystemGridView
                            │                          └─n × ELCDSystemMiniView
                            │                          └─1 × ELCDDropZoneView
                            ├─owns─▶ m_detailScene : QGraphicsScene
                            │          └─contains─▶ ELCDSystemDetailView
                            │                          ├─▶ ELCDGeneratorsView
                            │                          ├─▶ ELCDComponentView  (inverter)
                            │                          ├─▶ ELCDComponentView  (storage)
                            │                          ├─▶ ELCDComponentView  (converter)
                            │                          └─▶ ELCDComponentView  (transformer)
                            └─owns─▶ m_listController : ELCDListController : OSListController
```

### 8.4 Key Signals / Slots

```
ELCDSystemMiniView
  signal zoomInClicked(Handle elcdHandle)          → controller slot: zoomInOnELCD()
  signal removeClicked(Handle elcdHandle)          → controller slot: onRemoveELCD()
  signal validityIndicatorClicked(Handle)          → controller slot: onShowValidityReport()

ELCDSystemDetailView
  signal backClicked()                             → controller slot: zoomOutToGrid()
  signal bussTypeChanged(QString newType)          → controller slot: onBussTypeChanged()
  signal generatorDropped(OSItemId, Handle elcd)  → controller slot: onGeneratorDrop()
  signal inverterDropped(OSItemId, Handle elcd)   → controller slot: onInverterDrop()
  signal storageDropped(OSItemId, Handle elcd)    → controller slot: onStorageDrop()
  signal converterDropped(OSItemId, Handle elcd)  → controller slot: onConverterDrop()
  signal componentRemoveClicked(OSItemId)          → controller slot: onComponentRemove()
  signal validateClicked(Handle elcd)             → controller slot: onValidate()

ELCDTransformerDropZoneView
  signal transformerDropped(OSItemId, QString usage) → controller slot: onTransformerDrop()

ElectricLoadCenterDistributionController
  signal modelObjectSelected(OptionalModelObject, bool readOnly)
      → MainRightColumnController::inspectModelObject()
```

### 8.5 Refresh / Dirty Pattern

Same as `RefrigerationController`:
```cpp
void ElectricLoadCenterDistributionController::refresh() {
    m_dirty = true;
    QTimer::singleShot(0, this, &ElectricLoadCenterDistributionController::refreshNow);
}
void ElectricLoadCenterDistributionController::refreshNow() {
    if (!m_dirty) return;
    m_dirty = false;
    // rebuild scenes
}
```
Connect `model.getImpl<Model_Impl>()->onChange` to `refresh()`.

---

## 9. Component Compatibility Rules Summary

| Buss Type | Generators | Inverter | Storage | Converter |
|-----------|-----------|----------|---------|-----------|
| `AlternatingCurrent` | AC types | ✗ hidden | ✗ hidden | ✗ hidden |
| `AlternatingCurrentWithStorage` | AC types | ✗ hidden | AC types | optional |
| `DirectCurrentWithInverter` | DC types | ✓ required | ✗ hidden | ✗ hidden |
| `DirectCurrentWithInverterDCStorage` | DC types | ✓ required | DC types | optional |
| `DirectCurrentWithInverterACStorage` | DC types | ✓ required | AC types | ✗ hidden |

Transformer (`LoadCenterPowerConditioning` usage) is optional for all buss types
and shown consistently in the detail view.

---

## 10. Open Questions

1. **Tab icon**: Do we have an existing icon to repurpose, or create new artwork?

2. **Generator sub-types in Library**: Should the right-column library show all
   generator subtypes (PV, FuelCell, MicroTurbine, WindTurbine, PVWatts) as
   separate entries, or group them under a "Generator" category?

3. **LoadCenterPowerConditioning transformer**: Should it appear in the global
   transformer panel (top of overview) or inside the ELCD detail view?  It is
   logically per-ELCD, so the detail view makes more sense.

4. **Multiple inverters**: The SDK allows only one inverter per ELCD.  The detail
   view treats it as a single drop zone.  Confirm this is correct.

5. **`validityReport()` availability**: The design assumes a method returning
   human-readable log messages alongside the boolean `validityCheck()`.  Confirm
   the exact SDK API (it was referenced in the SDK source at lines 575–723).

6. **Generator DC/AC classification**: The incompatibility dialog mentions "AC
   generators" vs "DC generators".  Does the SDK provide a way to query which
   generator types are AC vs DC (e.g. `GeneratorPhotovoltaic` is DC)?  If not,
   we maintain a hard-coded type map in the app.

7. **Right-column "My Model" tab**: When a generator is selected in the detail
   view, should the inspector show all its fields (possibly via InspectorGadget),
   or only a curated subset like other inspector views?

8. **Storage operation fields**: Show as part of the graphics diagram inline, or
   in a separate "properties" panel below the diagram?  The diagram approach is
   more consistent with HVAC, but these fields are data, not topology.

---

## 11. Implementation Phases

| Phase | Scope |
|-------|-------|
| **1** | Tab registration, empty view, vertical tab button |
| **2** | Overview: ELCD card grid, add/remove ELCD, validity indicator |
| **3** | Overview: global transformer drop zones |
| **4** | Detail: buss-type-aware schematic for `DirectCurrentWithInverter` (most common case first) |
| **5** | Detail: remaining 4 buss type layouts |
| **6** | Detail: incompatibility dialog on buss type change |
| **7** | Right column library integration (per-context filtering) |
| **8** | Storage operation fields panel |
| **9** | Validation dialog and auto-validity indicators |
