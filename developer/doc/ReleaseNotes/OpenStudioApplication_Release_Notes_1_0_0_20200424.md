# OpenStudio Application Version 1.0.0

_Release Notes - 4/24/2020_

These release notes describe version 1.0.0 of the OpenStudio Application developed by the National Renewable Energy Laboratory (NREL), Buildings and Thermal Systems, Commercial Buildings Research Group, Tools Development Section, and associated collaborators. The notes are organized into the following sections:

-  Overview
-  Where to Find OpenStudio Documentation
-  Installation Notes
-  OpenStudio Application: Changelog


# Overview

Following an important announcement related to future development of the OpenStudio Application released in September 2019 ([A Shift in BTO’s BEM Strategy: A New Future for the OpenStudio Application](https://www.openstudio.net/new-future-for-openstudio-application)), **this release marks the last NREL planned software distribution of the OpenStudio Application.**
Below is the list of components that is included in this SDK installer:

__**OpenStudio Application 1.0.0**__
- Application Resources (Includes EnergyPlus & Radiance)
- Ruby API
- Command Line Interface (CLI)
- OpenStudio Application

**Note** that [PAT](https://github.com/NREL/OpenStudio-PAT) is not included in either the SDK or Application installers. You will need to install PAT separately which is distributed on the [OpenStudio-PAT](https://github.com/NREL/OpenStudio-PAT) GitHub page.

# Where to Find OpenStudio Documentation

- OpenStudio release documentation, including these release notes, tutorials, and other user documentation, is available at [https://www.openstudio.net/](https://www.openstudio.net/.)
- C++ API documentation is available at [https://openstudio-sdk-documentation.s3.amazonaws.com/index.html](https://openstudio-sdk-documentation.s3.amazonaws.com/index.html)
- OpenStudio Application Developer docs at [https://github.com/NREL/OpenStudioApplication/wiki](https://github.com/NREL/OpenStudioApplication/wiki)
- Measure development documentation is available at [http://nrel.github.io/OpenStudio-user-documentation/reference/measure_writing_guide/](http://nrel.github.io/OpenStudio-user-documentation/reference/measure_writing_guide/ )
- A roadmap for planned features is available at http://nrel.github.io/OpenStudio-user-documentation/getting_started/roadmap/.


# Installation Notes

OpenStudio Application 1.0.0 is supported on 64-bit Windows 7 – 10, OS X 10.12 – 10.15, and Ubuntu 18.04.

OpenStudio Application 1.0.0 supports [EnergyPlus Release 9.3](https://github.com/NREL/EnergyPlus/releases/tag/v9.3.0), which is bundled with the OpenStudio installer. 

OpenStudio Application 1.0.0 supports Radiance 5.0.a.12, which is bundled with the OpenStudio installer; users no longer must install Radiance separately, and OpenStudio will use the included Radiance version regardless of any other versions that may be installed on the system. 

OpenStudio Application 1.0.0 now uses **Ruby 2.5.5** as the supported version (2.2.4 previously).


## Installation Steps

- Download and install [OpenStudioApplication](https://github.com/NREL/OpenStudioApplication/). Select components for installation. Note that OpenStudio Application is a standalone app and does not require you to install OpenStudio SDK.
- Setup a Building Component Library (BCL) account to access online building components and measures. View instructions on how to setup your account and configure the key in OpenStudio.
- The [OpenStudio Application SketchUp Plug-in](https://github.com/NREL/openstudio-sketchup-plugin) requires SketchUp 2019 (not available for Linux). The OpenStudio Application SketchUp Plug-in does not support older versions of SketchUp. SketchUp must be installed before OpenStudio Application to automatically activate the plugin. If you install SketchUp after OpenStudio Application, simply rerun the OpenStudio Application installer. Please see the installation guide on [OpenStudio Application SketchUp Plugin](https://github.com/NREL/openstudio-sketchup-plugin). 

# OpenStudio Application: Changelog

The 1.0.0 is a major release
The 1.0.0 release updates OpenStudio SDK to 3.0.0 and to use EnergyPlus 9.3. This update includes several new features, performance improvements, and bug fixes. You can find the list of Pull Requests that got into this release [here](https://github.com/NREL/OpenStudioApplication/pulls?utf8=%E2%9C%93&q=is%3Apr+is%3Aclosed+created%3A2019-12-07..2020-04-20+).


## New Features and Fixes

These are specific to the Application. Please refer to the release notes for OpenStudio SDK for 3.0.0 release to see what has 
changed with the SDK. 

* Updated [OpenStudio SDK](https://github.com/NREL/OpenStudio) from v2.9.1 to v3.0.0

* Updated [EnergyPlus](https://github.com/NREL/EnergyPlus) from v9.2.0 to v9.3.0

* [#100](https://github.com/NREL/OpenStudioApplication/pull/100) - Properly register the OS:Construction:AirBoundary in OS App

* [#98](https://github.com/NREL/OpenStudioApplication/pull/98) - Make Geometry Editor work on Ubuntu (including in local build folder)

* [#94](https://github.com/NREL/OpenStudioApplication/pull/94) - Add cmake macro to find SDK for published s3 bucket

* [#93](https://github.com/NREL/OpenStudioApplication/pull/93) - Restore the style of the OSApp on Windows

* [#90](https://github.com/NREL/OpenStudioApplication/pull/90) - Fix Download progress bar cleared when finished downloading

* [#89](https://github.com/NREL/OpenStudioApplication/pull/89) - Enable MP builds and link to ruby lib for bindings (Windows)

* [#88](https://github.com/NREL/OpenStudioApplication/pull/88) - Improve reporting for Import SDD Xml file to include Logger errors/warnings 

* [#86](https://github.com/NREL/OpenStudioApplication/pull/86) - Fix need to showControls when switching tabs 

* [#84](https://github.com/NREL/OpenStudioApplication/pull/84) - Fix Refrigeration Grid should be declared unavailable for all but "Refigeration"

* [#82](https://github.com/NREL/OpenStudioApplication/pull/82) - Fix purge on Facility > Shading subtab should delete empty shading surface groups and shading surfaces without groups

* [#75](https://github.com/NREL/OpenStudioApplication/pull/75) - Application can now use the OpenStudio SDK to build vs having to build the submodule first, which simplifies the build process.

* [#73](https://github.com/NREL/OpenStudioApplication/pull/73) - Fix Blacked Out User Selection within Dropdown Menus on Ubuntu 

* [#72](https://github.com/NREL/OpenStudioApplication/pull/72) - Add Fan:SystemModel to OpenStudioApplication 

* [#69](https://github.com/NREL/OpenStudioApplication/pull/69) - Fix Sketchup 2019 plugin for Mac osx

* [#61](https://github.com/NREL/OpenStudioApplication/pull/61) - Fix crash in Schedule tab

* [#60](https://github.com/NREL/OpenStudioApplication/pull/60) - Support holidays in schedule GUI


## Issue Statistics Since Previous Release

* 47 new issues were filed since 2020-01-01 (not including opened pull requests)

* 34 issues were closed since the 2020-01-01
