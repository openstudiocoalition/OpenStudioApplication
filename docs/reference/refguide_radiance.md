#OpenStudio-Radiance Reference Guide
This reference guide explains the methods used in OpenStudio to utilize [Radiance](http://www.radiance-online.org/) to simulate the daylight ingress in your OpenStudio model, allowing for higher fidelity simulations of daylighting-related energy efficiency measures. Radiance is not constrained to simple geometry, and the OpenStudio model supports interior architectural detail such as [interior partition surfaces](../reference/sketchup_plugin_interface#NewInteriorPartitionSurfaceGroup) (for representing columns, soffits, furniture, etc) and air walls (for eliminating thermal zone boundaries that do not correspond to any actual building architecture). This allows for a more accurate characterization of the spatial distribution of daylight in the OpenStudio model.

For OpenStudio [v1.7.0](https://github.com/NREL/OpenStudio/releases/tag/v1.7.0), added support for window shades and wall thickness has increased the utility of the Radiance simulation option in the OpenStudio application.

With OpenStudio [v1.9.0](https://github.com/NREL/OpenStudio/releases/tag/v1.9.0), the Radiance simulation option has been refactored (moved) to a measure, eliminating the "daylight simulation engine" selection radio buttons from the app, and modifying the workflow slightly.

##Overview
In short, OpenStudio manages the model translation, simulation, and results processing of an OpenStudio building model (.osm), deriving geometry, materials, loads, schedules, and site information from the OSM. An annual climate-based daylight simulation is performed, new lighting load schedules are generated and embedded in the osm for the subsequent EnergyPlus simulation. In addition, a number of daylight metrics are calculated for those spaces with daylighting objects in them (illuminance maps and glare sensors), and are reported in a comma-delineated format, suitable for import and plotting in the modeler's tool of choice.

##Modeling Shading Controls with the 3-Phase Method
The 3-Phase method is used in OpenStudio for conducting annual climate-based daylight simulations in a reasonably fast manner. This method solves the flux transfer from the sky to the window separately from the flux transfer from the window to the point(s) of interest in the space. These matrices are called the _daylight matrix_ __(DMX)__ and _view matrix_ __(VMX)__, respectively. A third matrix called the transmission matrix (T), is represented by a bidirectional scattering distribution function (BSDF). The BSDF optically characterizes the shading layer and is inserted into the matrix and multiplied by a sky matrix (s) for each timestep to give a single hourly result for the model:

**I<sub>3ph</sub> = VTDs**

Where:

I = Result matrix containing time series of illuminance or luminance result

V = View matrix, relating outgoing directions on window to desired results at interior

T = Transmission matrix, relating incident window directions to exiting directions (BSDF)

D = Daylight matrix, relating sky patches to incident directions on window

s = Sky matrix, a collection of sky vectors (NOTE: in the 3-phase method, the solar flux is represented by the three sky matrix patches closest to the solar position at the given timestep.)

We rely on the rfluxmtx tool in Radiance to handle the creation of all the necessary daylight coefficient matrices, and on some generic BSDFs from the Building Component Library to represent the shading layers. The OpenStudio-to-Radiance model translator groups the building fenestration by each unique combination of the following properties:

- orientation
- visible light transmittance
- shading control

All windows with no shading control are grouped together into a single group called Window Group Zero (WG0), and are calculated together. Then, windows matching each unique combination of the properties above are also grouped together, into sequentially-numbered groups (WG1, WG2, etc). An annual illuminance schedule for each window group is calculated, and in the case of controlled window groups, a schedule is calculated once for that group with the shades up, and another one for the shades down. A shade schedule is generated, based on incident solar irradiance on the glazing, and this is used to create a final _Building Illuminance Schedule_ which takes the correct timestep illuminance values for each window group and sums them together for each day, for the entire year.



