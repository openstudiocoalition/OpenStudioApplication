<h1>Finding Model Data</h1>
Good inputs are critical to getting good results from energy modeling. This page provides a list of resources that might be helpful in determining inputs.

## OpenStudio Building Type Templates
The New OpenStudio Model From Template tool is represented by this icon in the toolbar: ![New OpenStudio Model](img/plugin_reference_guide/os_new.png "New OpenStudio Model"). Templates contain data for constructions, loads, and schedules for four vintages across all U.S. climate zones. Templates do not contain any geometry.

<iframe width="640" height="360" src="https://www.youtube.com/embed/wzzY_W2WELo?end=43" allowfullscreen></iframe>

*Above: This video shows you how to choose a template before setting up your model.*

![New OpenStudio Model From Template Dialog](img/create_model/from_template.png)

*Above: Dialog to choose template.*

Vintages:

- DOE Ref Pre-1980
- DOE Ref 1980-2004
- DOE Ref 2004
- 189.1-2009
- 90.1-2007

Climate Zones: 1 - 8

![Climate Zone Map](img/create_model/climate_zones.png)

## Building Component Library (BCL)
### Find Measures and Find Components
An online source of building energy modeling data:

- Enables drag-and-drop modeling for quick technology evaluation
- Provides consistent, detailed inputs to drive decision-making
- Searchable and readily available within applications

The BCL window gives you access to an online repository of building energy modeling data and scripts called the [Building Component Library](https://bcl.nrel.gov/). Although you can access the BCL website on its own, OpenStudio has integrated access to the BCL from within the application. You can access this through the "Window" menu.

You will be prompted for an API key the first time you open this window, unless you have already used BCL functionality in the SketchUp Plug-in. [Instructions for finding your API Key](../getting_started/getting_started.md#connecting-with-the-building-component-library) are in the "Getting Started" section.

![Online BCL Dialog](img/os_interface/bcl_window.png)

*Above: This window gives you access to the online BCL to download measures or components.*
