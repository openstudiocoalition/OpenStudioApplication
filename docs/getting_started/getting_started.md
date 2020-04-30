<h1>Installation and Introductory Tutorial</h1>
This page walks you through installing the OpenStudio Application, the basics of its suite of applications, and the fundamental workflow.

## Installation Instructions
OpenStudio Application is supported on 64-bit versions of Windows 7 &ndash; Windows 10, OS X 10.12 &ndash; 10.14, and Ubuntu 18.04.
OpenStudio Application supports the latest EnergyPlus release which is bundled with the OpenStudio Application installer. The dependencies for the OpenStudio Application 1.0.0 include EnergyPlus 9.3.0, Radiance 5.0.a.12, SketchUp 2019 Pro, and Ruby 2.5.5.

###Installation Steps
__Download and install OpenStudio Application__

1. Download the latest release of OpenStudio Application [here](https://github.com/NREL/OpenStudioApplication/releases).
2. Choose the installer that matches your operating system. The OpenStudio Application package contains the following components:
    - OpenStudio Application
    - EnergyPlus
    - Ruby API
    - C# API
    - Command Line Interface
    - Radiance
    - Application Resources

__Optional - Download and install SketchUp and the Openstudio SketchUp Plugin__

There are a few options for generating geometry for the OpenStudio Application, including the built-in FloorspaceJS interface. One option is to use the OpenStudio Sketchup Plugin. To use the plugin:

1. Download and install SketchUp. Presently, the OpenStudio SketchUp Plugin requires [SketchUp 2019 Pro](https://www.sketchup.com/) (not available for Linux). 
2. Download the OpenStudio SketchUp Plugin [here](https://github.com/NREL/openstudio-sketchup-plugin/releases). Follow the [procedure to install the Plugin](https://help.sketchup.com/en/installing-ruby-plugins-extensions).

__Optional - Setup a Building Component Library (BCL) Account__  

BCL content can now be accessed from within the OpenStudio SketchUp Plugin and from the standalone OpenStudio Application. To take advantage of this integration, you will need to follow the steps outlined here to request a BCL key.

1. Login to [Building Component Library (BCL)](https://bcl.nrel.gov/). Create an account if you don't have one. Follow the instructions from the email sent to you upon registration and then login.
2. Click on the `My Dashboard` near the top right of the website.
3. Copy the text after `API v1.1 key`: then paste the key into the OpenStudio Application, or the SketchUp Plugin. __You only need to configure the key in one place and it will be used globally throughout the OpenStudio Application tools.__ Additionally, this key will be maintained when you install updates to the OpenStudio Application.

Enter the key through the OpenStudio SketchUp Plugin under the menu `Extensions->OpenStudio User Scripts->Building Component Library->Setup BCL Key` and click `OK`. __Or__ register the key through the OpenStudio Application under the menu `Components & Measures->Find Components`. If a key is not already registered, it will prompt you for one.

![Key request dialog](img/bcl_key_request.png)

__Optional - Install Parametric Analysis Tool (PAT)__  

1. Download the latest release of PAT [here](https://github.com/NREL/OpenStudio-PAT/releases) (not available for Linux).

## Workflow Overview
The typical OpenStudio Application workflow is shown in the diagram below.

[![Workflow Diagram](img/workflow_diagram.png "Click to view")](img/workflow_diagram.png)

*About: Click on the diagram above to view a larger version.*

Data Viewer is used to view simulation results. The section within documentation for Running Simulation & Viewing Results has information on using [Data Viewer](../tutorials/running_your_simulation/#using-data-viewer).

The [Parametric Analysis Tool Interface Guide](http://nrel.github.io/OpenStudio-user-documentation/reference/parametric_analysis_tool_2/) provides an introduction to the interface and workflow for creating multiple design alternatives from a seed model.

## Introductory Tutorial
The tutorial below was created before the grid view was added to the Space Types and Thermal Zones tabs. Grid view allows you to view and edit more than one space type or thermal zone at a time. Go to the [OpenStudio Application Interface Guide](../reference/openstudio_application_interface.md) to learn more about grid view.

### Selecting a Library for Constructions, Loads, and Schedules

Libraries that are included with the OpenStudio Application contain data for constructions, loads, and schedules for 6 vintages across all U.S. climate zones. They also contain data for 9 vintages specific to DEER. The Libraries do not contain any geometry. Load libraries by using the menu under __File/Load Library__. Then select your vintage from the default file set. The Library data is organized such that you can access specific constructions and loads, or you can apply construction sets and space types that contain whole packages of subsequent dependent characteristics. For example a Library Space Type contains several specific loads and associated schedules for that space type, all of which become part of your model if you add the Space Type to your model. You can also load previously generated models as Libraries through the __File/Load Library__ menu by navigating to any OpenStudio Model file from the browser window that pops up. It can take a few minutes to load. 

![New OpenStudio Model From Template Dialog](img/menu_libraries.png)

*Above: Load a new Library using the menu.*

It is good practice to regularly clear the Libraries that load when the OpenStudio Application opens by using the menu under __Preferences/Change Default Libraries__ and selecting __Restore Defaults__. If the OpenStudio Application is loading several Libraries it will slow down significantly.

![New OpenStudio Model From Template Dialog](img/default_libraries.png)

Vintages:

- DOE reference (Pre-1980, 1980-2004)
- ASHRAE standard (189.1-2009, 90.1-2004, 90.1-2007, 90.1-2010, 90.1-2013)
- DEER (pre-1975, 1985, 1996, 2003, 2007, 2011, 2014, 2015, 2017)

Climate Zones: 1 - 8

![Climate Zone Map](img/create_model/climate_zones.png)

### FloorspaceJS - Building Envelope

For additional information on the integrated FloorspaceJS interface, go to the [FloorspaceJS Interface Guide](../reference/geometry_editor.md)

### SketchUp Plugin - Building Envelope
For additional information on the SketchUp Plugin interface, go to the [OpenStudio SketchUp Plugin Interface Guide](../reference/sketchup_plugin_interface.md).


<!--
## Introductory Tutorial
- Choosing a Template
- Modeling the Building Envelope
- Assigning Building Activity
- Assigning Thermal Zones
- Assign Thermostats
- Saving the OpenStudio Model from the SketchUp Plugin
- Moving from the Plugin to the OpenStudio Application
- Adding Weather and Design Day Files
- Adding a Mechanical System
- Running a Simulation
- Viewing Simulation Results
-->
