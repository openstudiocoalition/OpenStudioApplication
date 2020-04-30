<h1>Radiance and OpenStudio</h1>
This tutorial explains how to use [Radiance](http://www.radiance-online.org/) to simulate the daylight ingress in your OpenStudio model, allowing for higher fidelity simulations of daylighting-related energy efficiency measures.

For OpenStudio [v1.9.0](https://github.com/NREL/OpenStudio/releases/tag/v1.9.0), the Radiance simulation option has been refactored (moved) to a measure, eliminating the "daylight simulation engine" selection radio buttons from the app, and modifying the workflow slightly. We've also added support for more shading options, including daylight redirecting louvers.

## Workflow
This workflow uses OpenStudio (the plug-in and the application) to perform a [climate-based daylight simulation](http://climate-based-daylighting.com/doku.php?id=academic:climate-based-daylight-modelling), using Radiance as the lighting simulation engine (in lieu of EnergyPlus' daylight simulation options). For implementation details, and caveats, refer to the OpenStudio-Radiance Reference Guide.

The process for using Radiance for daylighting analysis in OpenStudio is not dissimilar from using EnergyPlus. The basic steps are as follows, with the required applications in parentheses:
### 1. Create (or Import) Building Geometry, Define Thermal Zones & Spaces (SketchUp Plug-in)

Many of the Radiance-related elements are not directly accessible from the OpenStudio Application, so regardless of whether you are starting your geometric model from scratch or importing from another CAD tool, you must start in the SketchUp Plugin.

Using the SketchUp Plugin tools, create the following elements:

- Building Shading Objects - to represent overhangs, fins, parapets
- Site Shading Objects - to represent other buildings, trees, ground
- [Interior Partition Surfaces](../reference/sketchup_plugin_interface#NewInteriorPartitionSurfaceGroup) - can be used to define furniture, cubicle walls, & other relevant structures, in addition to refining the interior space geometry (e.g. adding columns, ceiling coffers, soffits)

###2. Define Materials (SketchUp Plug-in or OpenStudio Application)
It is recommended to review the materials of your model constructions to ensure the photometric properties, e.g., visible light transmittance, transmittance model (specular or diffuse), and visible light reflectance, are correct.

- Interior finishes
- Glazing

###3. Add Daylighting Elements (SketchUp Plugin)

For each space where you wish to analyze daylighting, add:

- Illuminance map (primary)
- Daylighting control point (primary)
- Define shading controls
- Assign shading controls

Optionally to these spaces, add:

- Glare sensors
- Create & assign OS:WindowFrameAndDivider objects, to represent wall thickness in the Radiance model (user script)

###4. Configure Daylighting Elements (SketchUp Plugin)

- Associate Space with Thermal Zone(s)
- Assign primary Illuminance Map to daylit Thermal Zone(s)
- Assign primary Daylighting Control Point to Thermal Zone
- Thermal zone load percentages to daylighting controls

###5. Run Simulation (OpenStudio Application)
- Attach a weather file (Site Tab)
- Select Radiance simulation parameters (Simulation Settings Tab)
- Add the "Radiance Daylighting Measure" to the model's workflow (Measures Tab)
- Run the analysis (Run Tab (_duh_))

The Radiance daylighting results will inform the electric lighting load schedules and will be automatically used in the EnergyPlus model

###6. Review Results (Excel, et al.)
The results of the Radiance/EnergyPlus simulation are stored in a few locations:

- **radout.sql** - this file contains all the Radiance-computed daylighting information, as well as the exterior daylight data from the weather file, for reference. Its data can be reviewed with the Results Viewer application.
- **daylightmetrics.csv** - this file contains data for the following metrics:
    - Daylight Autonomy (DA)
    - Continuous Daylight Autonomy (cDA)
    - Useful Daylight Illuminance (UDI)
- **[windowGroupName].ill** - annual daylight illuminance schedules for each window group, used for the shade control simulation


