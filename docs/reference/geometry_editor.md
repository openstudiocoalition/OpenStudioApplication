# ![][1] [FloorspaceJS](https://github.com/NREL/floorspace.js)
[1]: img/favicon-32x32.png

The OpenStudio Application has integrated the FloorspaceJS web-based geometry editor for simplified entry of 2D floor plans.  More information about FloorspaceJS, including a tutorial on its use, can be found at the [FloorspaceJS project site](https://github.com/NREL/floorspace.js).

[FloorspaceJS](https://github.com/NREL/floorspace.js) is an open source software module that developers can leverage for geometry creation.  FloorspaceJS is meant to cover simple building geometry use cases only.  More complex building geometry is best developed in a full featured CAD tool and exported to gbXML for building energy modeling.  FloorspaceJS reads and writes a custom [floorplan.json](https://raw.githubusercontent.com/NREL/floorspace.js/develop/schema/geometry_schema.json) JSON file format.  New methods have been added to the OpenStudio SDK, which can translate this file format to ThreeJS JSON as well as to OSM.  Additionally, new methods have been added to the SDK, which allows two OpenStudio Models to be merged.  These functionalities are demonstrated by integrating FloorspaceJS directly within the OpenStudio Application.

------
## Getting Started
To try FloorspaceJS within the OpenStudio Application, first navigate to the "Geometry" vertical tab and then to the "Editor" sub tab.  At this time, FloorspaceJS cannot be used on OpenStudio Models with existing geometry (either developed in the OpenStudio SketchUp plug-in, imported through gbXML, or procedurally generated from scripts like the DOE Prototype Measure) as these models do not include the requisite floorplan.json file used by the editor.  For models with no existing geometry a new floorplan may be created by pressing the "New" button next to the "Floorplan" dialog box.  Existing OpenStudio Models that include a floorplan.json model may be edited.

[![Start Screen](img/geometry_editor/start_screen.png "Start Screen")](img/geometry_editor/start_screen.png)

------

## Using FloorspaceJS

A detailed tutorial on FloorspaceJS usage is available at the [FloorspaceJS project site](https://github.com/NREL/floorspace.js).

------

## Preview and Merge
At any time, you can preview your floorplan in 3D by pressing the "Preview" button.

[![Preview 1](img/geometry_editor/preview1.png "Preview 1")](img/geometry_editor/preview1.png)

[![Preview 2](img/geometry_editor/preview2.png "Preview 2")](img/geometry_editor/preview2.png)

Your OpenStudio Model is not altered until you press the "Merge" button.  Performing a merge translates your floorplan to a new OpenStudio Model that is merged them with your current OpenStudio Model.  Any geometry in your current OSM will be replaced by new geometry from the floorplan OSM.  However, non-geometry objects such as HVAC in your current OSM will be preserved.

[![Merge](img/geometry_editor/merge.png "Merge")](img/geometry_editor/merge.png)

Once your floorplan has been merged with the current OSM you will see new objects associated with your floorplan throughout the rest of the OpenStudio Application.

[![Post Merge](img/geometry_editor/post-merge.png "Post Merge")](img/geometry_editor/post-merge.png)

[![Space Tab](img/geometry_editor/space-tab.png "Space Tab")](img/geometry_editor/space-tab.png)

The intention of integrating FloorspaceJS with the OpenStudio Application is to demonstrate how geometry data may be merged seamlessly into the OpenStudio Model to support a variety of sophisticated workflows.  Thermal zones, space types, default construction sets, and building units are synchronized between the two interfaces intuitively and with little effort on the part of the user.  As with any new software feature, some rough edges are expected.  Please let us know of issues you find by sending email to [OpenStudio@nrel.gov](mailto:OpenStudio@nrel.gov).  If FloorspaceJS becomes unresponsive, please press the "Debug" button, navigate to the console, and cut/paste diagnostic messages that may help us identify and fix any issues.

