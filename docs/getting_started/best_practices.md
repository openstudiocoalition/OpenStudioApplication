<h1>Modeling Best Practices</h1>
This page is a collection of best practices that have been developed over time and by experience. Following these best practices will help you avoid the issues that users experience most often.

## General
- __Save your model often and with different versions.__ Save multiple versions of your OpenStudio Model as you progress; this makes it easier to recover from an error and provides takeoff points for model variations.
- EnergyPlus is a detailed simulation engine, but you have to tell it how you want your building modeled. A common misunderstanding is that EnergyPlus will interpret "voids" between zones "correctly." EnergyPlus does not compute heat transfer between zones if they do not share a surface, so you must input the shared surfaces and set appropriate boundary conditions. To correctly model air transfer between zones, you have to add input objects that describe the airflow between zones. To model daylight transmission between zones, you need to add interior windows. Just because a building looks right, does not mean it is modeled correctly; digging in to the IDF and looking at detailed results is the only way to determine that you have modeled your building correctly.
- When you import an IDF into OpenStudio, it may be altered as part of the import process. For example, if your windows are not in the same plane as their base surface, OpenStudio moves them into the base surface plane. There may be other changes as well. If you want to keep your raw IDF untouched, you should "SaveAs", either as soon as you import it, or before you import it.
To rename objects, go to the OpenStudio Inspector. Do not alter the names in SketchUp's Entity Information dialog.
- The OpenStudio Inspector works on one object at a time. If you select multiple objects or surfaces and make a change, it will affect only one object.

## Guidelines for Spaces
- Spaces should be top-level objects. Do not combine multiple spaces into a group or component, or put a space under another group.
- Name your spaces to make large models easier to manage and search. You may want to name your surfaces and subsurfaces.
- Geometry within a space should be convex.
- Ideally, spaces, not just space surfaces, are also convex.
- After creating a new space or shading group, double-click the group to edit it. The Outliner or a keyboard shortcut can also be used to edit a zone, shading group, or interior partition group.

## Guidelines for Building Surfaces
- Do not group a selection of EnergyPlus surfaces or subsurfaces together. All surfaces under a zone should be ungrouped.
- Model exterior walls to the outside face of the wall.
- Draw interior walls to their centerlines if possible. You may, however, choose any edge, as long as you are consistent with adjacent zones. Surfaces for adjacent zones must not overlap each other.
- Divide a surface into two by drawing a line across the face.
- Try to avoid deleting a surface by erasing a shared edge with another surface. This does not update the subsurfaces of the deleted surface to point to the new base surface. If this becomes necessary, you can delete one edge of the subsurface to remove it and then redraw the edge to create a new subsurface assigned to the correct surface.
- As much as possible, you should avoid using curves (arcs or circles) to create your geometry. Curves and arcs will create faceted surfaces and increase simulation time. If you do use curves, set the segment count as low as possible. Before you extrude a curve in SketchUp, you can select it and right click to view the "Entity Information" window. There you can change the number of segments. If you want to make changes to curves via the Object Information window, you have to "View Hidden Geometry" to select individual facets. You cannot globally change the whole curve at once.

## Guidelines for Subsurfaces
- Do not place a subsurface (window or door) inside another subsurface.
- Do not place two subsurfaces against each other. Place a small space between them.
- Do not make a window the size of an entire base surface. Use SketchUp's Offset Tool to inset the sub-surface some nominal amount.
- Do not draw a subsurface that shares two edges with a base surface. This will create a new base surface. You can draw a window at the edge of wall, but not at the edge as well as the roof or floor.
- Delete a subsurface by either removing all the edges (one at a time) or by double-clicking to select the face and edges and then deleting. Do not delete the face only, as this leaves a visual hole in the base surface.

## Guidelines for Interior Partition Surfaces
By default interior partition surfaces are included and converted to internal mass objects for EnergyPlus simulation. Adding a large number of interior partitions surfaces will increase your simulation runtime. If this is a concern, you can model similar internal mass objects within a space as a single larger object.

## Guidelines for Shading Surfaces
- Shading surfaces need to be drawn in their own group. This group can be outside or inside a zone. This is a change in behavior from previous versions where shading surfaces could be created as loose geometry in an EnergyPlus zone.
- By default EnergyPlus mirrors shading surfaces, so typically face orientation does not matter. In some cases, however, such as when you add PV to a shading surface, it must be oriented correctly. As with building surfaces, the darker side is the outward normal.

<!--## Deciding on the Level of Detail for Your Model
## Run Simulations on Local Hard Drive.
## Don't use Undo in the SketchUp Plugin
## When Working in the SketchUp Plugin and OpenStudio Application at the Same Time Make sure to Manage Files Properly.
-->
