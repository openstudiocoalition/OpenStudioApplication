# OpenStudio Application

**This repository hosts the graphical interfaces that have historically used the OpenStudio SDK**[<sup>[1]</sup>](#1). These include the **OpenStudio Application** and the **OpenStudio Sketchup Plug-in**:

* The **OpenStudio Application** is a fully featured graphical interface to OpenStudio models including envelope, loads, schedules, and HVAC.
* The **OpenStudio SketchUp Plug-in** is an extension to Trimble’s popular SketchUp 3D modeling tool that allows users to quickly create geometry needed for EnergyPlus using the built-in functionality of Trimble SketchUp including existing drawing tools, integration with Google Earth, Building Maker, and Photo Match


OpenStudio Application     |  Sketchup Plug-in
:-------------------------:|:-------------------------:
<img src="https://www.openstudio.net/sites/all/modules/nrel_pages/images/osapp.png" style="width:33%" alt="OpenStudio Application" />  |  <img src="https://www.openstudio.net/sites/all/modules/nrel_pages/images/sketchup.png" style="width:49%" alt="Sketchup Plug-in" />

<a class="anchor" id="1"><sup>[1]</sup></a> *To understand why the graphical user interfaces are being separated from the SDK, please refer to [A Shift in BTO’s BEM Strategy: A New Future for the OpenStudio Application](https://www.openstudio.net/new-future-for-openstudio-application)*.

## What is the OpenStudio SDK?

[OpenStudio](https://github.com/NREL/OpenStudio) is a cross-platform (Windows, Mac, and Linux) collection of software tools to support whole building energy modeling
using [EnergyPlus](https://github.com/NREL/EnergyPlus) and advanced daylight analysis using [Radiance](https://github.com/NREL/Radiance/).
OpenStudio is an open source project to facilitate community development, extension, and private sector adoption.

OpenStudio includes a **Software Development Kit (SDK)** that allows building researchers and software developers to quickly get started through its multiple entry levels, including access through C++, Ruby, and C#.

More information and documentation is available at the [OpenStudio](https://www.openstudio.net) website.

## Developer Information

[OpenStudio](https://github.com/NREL/OpenStudio) (core SDK) is included as a `git submodule` in the folder `./openstudio`. Be sure to initialize submodules, which can be done during cloning by passing the `--recurse-submodules` flag, or after the fact via `cd OpenStudioApplication/openstudio && git submodule init && git submodule update`.  When pulling updates from the remote repository run `git pull --recurse-submodules` and then `git submodule update --remote --recursive`.

**Temporary information:** Current this submodule is tracking branch `https://github.com/NREL/OpenStudio/tree/OS_App_killswitch` which will soon replace `NREL/OpenStudio/develop3`.

To build, use the root of the repo when configuring with ccmake.

```
cd OpenStudioApplication
mkdir build
cd build
ccmake ../
```
