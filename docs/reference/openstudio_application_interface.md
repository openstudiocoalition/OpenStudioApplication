<h1>OpenStudio Application Interface Guide</h1>

------

## Overview
The tabs on the left that move vertically are ordered in a suggested workflow.

[![Overview](img/os_interface/overview.png "Click to view")](img/os_interface/overview.png)

*Above: Overview of the OpenStudio Application Interface.*

------

## Sub-tabs
Sub-tabs are designed to move from more general categories on the left to more specific on the right. On the Constructions tab, the sub-tabs are Construction Sets, Constructions, and Materials. If you are not building your own materials and constructions you may not need to go to the Materials sub-tab.

------

## Right Panel
The right panel provides access to items in the library or in your Model, and allows you to edit some of these items.

- __"My Model"__ displays items that are part of your Model already.

- __"Library"__ includes components and measures that come with the application or are downloaded from the Building Component Library (BCL).

- __"Edit"__ allows you to select certain Components and edit the settings for that Component. It is used in the HVAC Systemstab to edit component settings, assign thermal zones to loops, and to add plenums.

[![Edit Tab](img/os_interface/edit_tab.png "Click to view")](img/os_interface/edit_tab.png)

*Above: Edit settings on some objects in the right panel Edit tab.*

------

## Left Panel Bottom
![Add](img/os_interface/add.png) Add New Item allows the user to add a new Component to the interface. Sometimes the type of item added depends on the selection in the left panel.

[![Add Example](img/os_interface/add_example.png "Click to view")](img/os_interface/add_example.png)

*Above: On the Utility Bill sub-tab, select the type of bill you want to add before hitting the Add button.*

![Duplicate](img/os_interface/duplicate.png) Duplicate a selected object with this button.

![Delete](img/os_interface/delete.png) Delete a selected object with this button.

![Purge](img/os_interface/purge.png) Purge unused objects with this button.

------

## Adding Objects from the Library
To add items from the library, select the Library tab on the right panel and find the item you want to add to the Model. Select that item and drag and drop it into the correct drop zone. Drop zones are surrounded by a dotted line and are programmed to only accept items that will work for that field.

[![Drag-and-drop Constructions](img/os_interface/drag_drop.png "Click to view")](img/os_interface/drag_drop.png)

*Above: Drag-and-drop items from the library to the drop zones in the Model.*

[![Drag-and-drop Service Hot Water](img/os_interface/drag_drop_vrf.png "Click to view")](img/os_interface/drag_drop_vrf.png)

*Above: Drag-and-drop example on the HVAC service hot water.*

------

## Grid View and Multi-edit: Thermal Zones, Space Types, and Refrigeration
Earlier releases have included a grid view for the refrigeration section in the HVAC Systems tab. Now you can edit thermal zones and space types in a grid view. This makes it easier to view all your Zones and Space Types at once and compare settings. By selecting the buttons across the top you can select the fields you would like to see and edit.

[![Grid Image](img/os_interface/grid_view_multi1_small.png "Click to view")](img/os_interface/grid_view_multi1_small.png)

*Above: The grid view provides a spreadsheet style layout. Click on the image to view a larger version.*

Drag Components and Schedules into the grid view. Most items on the grid can be inspected in the right panel Edit tab, except schedules. To edit or view schedules, go to the Schedules tab.

A yellow highlight indicates the item you have selected in the grid. When you start editing the item in the right panel Edit tab, the highlight will disappear, indicating that the focus is now on editing the item. Use the Delete button in the Edit tab to remove a Component from the grid.

Click the shaded portion of a column header if you want to view that column in the "Custom" category.

Click on the color box on the General button to change the rendering color of the Space Type. This will change the rendering color in the SketchUp Plug-in as well.

You can use the Multi-edit feature on individual columns (as shown below) or on whole rows by selecting a row or rows and hitting the Copy button on the bottom left to duplicate those rows.

[![Grid Image](img/os_interface/multi-edit-steps-small.png "Click to view")](img/os_interface/multi-edit-steps-small.png)

*Above: The steps for using the multi-edit function are shown above.*

[![Grid Image](img/os_interface/loads_filter_small.png "Click to view")](img/os_interface/loads_filter_small.png)

*Above: Hit the Loads button to edit and view loads by space type. Click on the name of a component and select the right panel Edit tab to inspect and edit that item.*

------

## File Menu
If you launch the OpenStudio Application from the SketchUp Plug-in, your open file will automatically open in the application. To save a file or open a new file from within the OpenStudio Application, access the corresponding options within the File menu. 

When you save an OpenStudio Model (OSM) in the OpenStudio Application or the SketchUp Plug-in, a folder is saved next to the OSM file. This folder contains external resources such as the weather file, scripts, and simulation results.

Load Library is also a very important feature. This allows you to load building component libraries for specific building types. These libraries are the same as those used in the SketchUp Plug-in templates.

[![File Menu](img/os_interface/file_menu.png "File Menu" "Click to view")](img/os_interface/file_menu.png)

*Above: The File menu contains the new, open, revert, save, import, and export functions.*

------

## Preferences Menu
The Units menu lets you switch between SI and IP units. This affects both input fields and output data on the Results Summary tab. It does not currently affect standard EnergyPlus output files.

The SketchUp Plug-in has access to this as well under `Extensions->OpenStudio->Preferences`.

"Change Default Libraries" allows one to restore default libraries. This feature complements the Load Library feature within the File menu. Loading several libraries could increase the duration to boot the OpenStudio Application. To reduce this duration, restore default libraries. 

[![Preferences Menu](img/os_interface/prefer_menu.png "Click to view")](img/os_interface/prefer_menu.png)

*Above: The Preferences menu contains the units, measure directory, and tool location options.*

------

## Components & Measures Menu
[![Components & Measures Menu](img/os_interface/measures_components_menu.png "Click to view")](img/os_interface/measures_components_menu.png)

*Above: The Components & Measures menu contains the Apply Measure Now, Find Measures, and Find Components features*

### Apply Measures Now
You can apply measures to your Model at any time by going to the Components and Measures menu and selecting the Apply Measures Now option.

This will open a dialog that allows you to choose a measure (downloaded from the BCL) to apply, or even write your own measure and test it.

Once you select a measure you may edit the measure inputs on the right side of the dialog. Click "Apply Measure" to start.

[![Apply Measure Now](img/os_interface/apply_measure_now.png "Click to view")](img/os_interface/apply_measure_now.png)

*Above: This menu item allows you to run one measure on your model.*

### Find Measures and Find Components
The Online BCL window gives you access to an online repository of building energy modeling data called the Building Component Library. Although you can access the [BCL website](https://bcl.nrel.gov/) on its own, OpenStudio Application has integrated access to the BCL. You can access this through the Components & Measures menu.

The first time you open this window you will be prompted for an API key, unless you have already used BCL functionality in the SketchUp Plug-in. [Instructions for finding your API Key](../getting_started/getting_started.md#connecting-with-the-building-component-library) are in the "Getting Started" section.

[![BCL Window](img/os_interface/bcl_window.png "Click to view")](img/os_interface/bcl_window.png)

*Above: This window gives you access to the online BCL to download measures or components.*
