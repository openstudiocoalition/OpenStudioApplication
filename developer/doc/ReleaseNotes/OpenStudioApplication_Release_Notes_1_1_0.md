# OpenStudio Application Version 1.1.0

_Release Notes - <TDB>_

These release notes describe version 1.1.0 of the OpenStudio Application developed by the OpenStudio Coalition, and associated collaborators. The notes are organized into the following sections:

-  Overview
-  Where to Find OpenStudio Documentation
-  Installation Notes
-  OpenStudio Application: Changelog


# Overview

Following an important announcement related to future development of the OpenStudio Application released in September 2019 ([A Shift in BTO’s BEM Strategy: A New Future for the OpenStudio Application](https://www.openstudio.net/new-future-for-openstudio-application)), **this release marks the last NREL planned software distribution of the OpenStudio Application.**
Below is the list of components that is included in this SDK installer:

__**OpenStudio Application 1.1.0**__
- Application Resources (Includes EnergyPlus & Radiance)
- Ruby API
- Command Line Interface (CLI)
- OpenStudio Application

**Note** that [PAT](https://github.com/NREL/OpenStudio-PAT) is not included in either the SDK or Application installers. You will need to install PAT separately which is distributed on the [OpenStudio-PAT](https://github.com/NREL/OpenStudio-PAT) GitHub page.

# Where to Find OpenStudio Documentation

- OpenStudio Application release documentation, including these release notes, tutorials, and other user documentation, is available at [https://openstudiocoalition.org/](https://openstudiocoalition.org/)
- C++ API documentation is available at [https://openstudio-sdk-documentation.s3.amazonaws.com/index.html](https://openstudio-sdk-documentation.s3.amazonaws.com/index.html)
- OpenStudio Application Developer docs at [https://github.com/openstudiocoalition/OpenStudioApplication/wiki](https://github.com/openstudiocoalition/OpenStudioApplication/wiki)
- Measure development documentation is available at [http://nrel.github.io/OpenStudio-user-documentation/reference/measure_writing_guide/](http://nrel.github.io/OpenStudio-user-documentation/reference/measure_writing_guide/ )


# Installation Notes

OpenStudio Application 1.1.0 is supported on 64-bit Windows 7 – 10, OS X 10.12 – 10.15, and Ubuntu 18.04.

OpenStudio Application 1.1.0 supports [EnergyPlus Release 9.4](https://github.com/NREL/EnergyPlus/releases/tag/v9.4.0), which is bundled with the OpenStudio installer.

OpenStudio Application 1.1.0 supports Radiance 5.0.a.12, which is bundled with the OpenStudio installer; users no longer must install Radiance separately, and OpenStudio will use the included Radiance version regardless of any other versions that may be installed on the system.

OpenStudio Application 1.1.0 uses Ruby 2.5.5 as the supported version.


## Installation Steps

- Download and install [OpenStudioApplication](https://github.com/NREL/OpenStudioApplication/). Select components for installation. Note that OpenStudio Application is a standalone app and does not require you to install OpenStudio SDK.
- Setup a Building Component Library (BCL) account to access online building components and measures. View instructions on how to setup your account and configure the key in OpenStudio.
- The [OpenStudio SketchUp Plug-in](https://github.com/openstudiocoalition/openstudio-sketchup-plugin) requires SketchUp 2019 (not available for Linux). Please see the installation guide on [OpenStudio SketchUp Plugin](https://github.com/openstudiocoalition/openstudio-sketchup-plugin).

# OpenStudio Application: Changelog

The 1.1.0 is a major release.

The 1.1.0 release updates OpenStudio SDK to 3.<TDB>.<TDB> and to use EnergyPlus 9.4. This update includes several new features, performance improvements, and bug fixes. You can find the list of Pull Requests that got into this release [here](https://github.com/NREL/OpenStudioApplication/pulls?utf8=%E2%9C%93&q=is%3Apr+is%3Aclosed+created%3A2020-04-20..2020-07-20+).
TODO: Update the date in the link above

## New Features and Fixes

These are specific to the Application. Please refer to the release notes for OpenStudio SDK for 3.1.0 release to see what has changed with the SDK.

* Updated [OpenStudio SDK](https://github.com/NREL/OpenStudio) from v3.0.0 to v3.1.0

* Updated [EnergyPlus](https://github.com/NREL/EnergyPlus) from v9.3.0 to v9.4.0

* [#185](https://github.com/NREL/OpenStudioApplication/pull/185) - Support WaterCooled VRFs in OSApp

* [#193](https://github.com/NREL/OpenStudioApplication/pull/193) - Add new fields for SimulationControl: `DoHVACSizingSimulationforSizingPeriods` and `MaximumNumberofHVACSizingSimulationPasses`


## Issue Statistics Since Previous Release

* <X> new issues were filed since 2020-04-24 (not including opened pull requests)

* <Y> issues were closed since the 2020-04-24
