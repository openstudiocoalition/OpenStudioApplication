<h1>OpenStudio SketchUp Plug-in Interface Guide</h1>
This section provides the legend for toolbars and menus available in the Plug-in. You may also find helpful information in the [OpenStudio Quick Start Guide (PDF)](img/pdfs/openstudio_interface_quickstart.pdf).

![Plug-in Toolbars](img/plugin_reference_guide/toolbars_os.png)

## File Management
Tools in this set create, open, save, import, or export data to or from your model. Some have toolbar icons; some do not. These functions are also located the OpenStudio Plug-in menu.
<table class="table table-striped">
<thead>
<tr>
<th>Icons</th>
<th>Functionality</th>
</tr>
</thead>
<tbody>
<tr>
<td>![New Model From Template Icon](img/plugin_reference_guide/os_new.png)</td>
<td>
#### New OpenStudio Model
This tool closes your current OpenStudio model and creates a new one. On a Mac it creates an empty model and you have to go under the menu item __Extentions/OpenStudio User Scripts/On-Demand Template Generators/Space Type and Construction Set Wizard__ to add template items like constructions, schedules, and space types. On a PC the template dialog will be available once and you can choose templates. If you want to add additional template types you must go to the __Extentions/OpenStudio User Scripts/On-Demand Template Generators/Space Type and Construction Set Wizard__. </td>
</tr>
<tr>
<td>![Open OpenStudio Model Icon](img/plugin_reference_guide/OSOpen-24_0.png)</td>
<td>
#### Open OpenStudio Model
Use this feature to close your current OpenStudio model and open an OpenStudio file (.osm). It does not close your current SketchUp file, but loads a different OpenStudio model into it.</td>
</tr>
<tr>
<td>![Save OpenStudio Model Icon](img/plugin_reference_guide/OSSave-24_0.png)</td>
<td>
#### Save OpenStudio Model
Saves your OpenStudio Model as an OpenStudio file (.osm). If you save your OSM file and then save your SketchUp model, the SketchUp model will maintain a link to the OSM model. If you close and reopen your linked SketchUp file (.skp), it will reload the content from the OSM file. All the energy model content is saved in the OSM file, not in the SketchUp file.

The benefit of having a link to a SketchUp file is to maintain a relationship with non-OpenStudio content that you may have in your model, such as a base plan or base model. A linked SketchUp file also has saved scenes, drawing styles, and section cut location. When saving your OpenStudio model, make sure your filename ends with the ".osm" extension. If it does not have that extension, OpenStudio will not open the file. If you have files without this extension, you can manually rename them; they should open.</td>
</tr>
<tr>
<td>![Save As OpenStudio Model Icon](img/plugin_reference_guide/OSSaveAs-24_0.png)</td>
<td>
#### Save As OpenStudio Model
Using this feature saves your OpenStudio model as a new OSM file and updates the SketchUp file to link to the newly saved OSM file.

When saving your OpenStudio model, make sure your filename ends with the ".osm" extension. If it does not have this extension, OpenStudio will not open the file. If you have files without this extension, you can manually rename them; they should open.
</td>
</tr>
<tr>
<td>![Import EnergyPlus Input Data File Icon](img/plugin_reference_guide/import_idf.png)</td>
<td>
#### Import EnergyPlus Input Data File (IDF)
This feature functions similarly to the Import OpenStudio Model tool. If you import an entire IDF model, your current OSM model is closed and a new one is created from your IDF. When you import and then save an IDF model, you will be prompted to save it as an OSM file. If you want to keep it as an IDF file, you can choose the Export EnergyPlus Input Data File tool instead of the Save OSM tool. If you have added geometry or other objects, such as interior partitions that are unique to an OpenStudio model, you will lose or alter those objects when you export back to an IDF file.

Like the Import OpenStudio Model tool, the Import EnergyPlus Input Data File tool can be used to import additional content into your model. In this import workflow, your OSM model stays open and the import tool adds additional content from a previously saved IDF file.

Content types that can be imported from an IDF file:

- Constructions (including materials)
- Schedules

You can access these features under the Extensions/OpenStudio menu or the `File->Import` menu. *Space Loads* are unique to an OSM, so they cannot be imported from an IDF file.

![Import Dialog](img/plugin_reference_guide/OpenStudioImportMenu_0.png)

*Above: Import dialog allows you to import OpenStudio models, constructions, schedules, and space loads. You can also import IDF files through this menu.*
</td>
</tr>
<tr>
<td>![Export IDF Icon](img/plugin_reference_guide/IDFExport-24.png)
</td>
<td>
#### Export EnergyPlus Input Data File
Use this tool when exporting an IDF file from an OSM model. This can be useful if you need an IDF file to use in an application that does not support the new OSM format. Some model data may be lost when exporting to an IDF because the OSM file format supports many objects, such as interior partition groups and surfaces, that are not in EnergyPlus. If you export an OSM model with unsupported elements to an IDF file, and then reimport it into OpenStudio, the unsupported elements will be gone.

Good practice for a typical workflow is to always use your OSM file as the working file. You can run simulations in EnergyPlus directly from an OSM file by using the OpenStudio RunManager application as a stand-alone application or through the OpenStudio SketchUp Plug-in interface.

When exploring an IDF file, make sure your file name ends with the ".idf" extension. If it does not have this extension, EnergyPlus and other applications may not open the file. If you have files without this extension, you can manually rename them; they should open.
</td>
</tr>
<tr>
<td></td>
<td>
#### Import OpenStudio Model
This tool can function similarly to the Open OpenStudio Model tool. If you import an entire model, your current model is closed and a new one loaded. When you import an OSM model and then save the OSM model, you will be prompted for a new filename. This prevents you from accidentally overwriting your original file.

The Import OpenStudio Model tool can also be used to import additional content into your model. In this import workflow, your OSM model stays open and the import tool adds content from a previously saved OSM file (see the Import dialog image above.) Content types that can be imported from an OSM file follow:

- Constructions (including construction sets, constructions, and materials)
- Schedules
- Space loads
</td>
</tr>
<tr>
<td></td>
<td>
#### Export OpenStudio Model
This tool prompts you for a new filename for your saved file. As you continue to work, you alter your original file instead of the exported file.

![Export Dialog](img/plugin_reference_guide/OpenStudioExportMenu_0.png)

*Above: Export menu available through Extensions menu*
</td>
</tr>
<tr>
<td></td>
<td>
#### Import gbXML File
There is no toolbar icon to use for importing gbXML (Green Building XML) files; this functionality is available through the `Extensions->OpenStudio->Import->Import gbXML Model` menu item only. If you import a gbXML model, your current OSM model is closed and a new one is created from your gbXML. When you import a gbXML model and then save the model, you will be prompted to save it as an OSM file.

The following gbXML element types are imported from gbXML to the OpenStudio Model:

- Construction
- Material
- ScheduleDay
- ScheduleWeek
- Schedule
- Campus
- Building
- Space
- Surface
- SubSurface

</td>
</tr>
<tr>
<td></td>
<td>
#### Export gbXML File
This functionality is available through the `Extensions->OpenStudio->Export->Export gbXML Model` menu item. Some information may be lost when exporting to a gbXML. This is because the OSM file format supports some object types that XMLs do not.
</td>
</tr>
<tr>
<td></td>
<td>
#### Export Untranslated EnergyPlus IDF
When an IDF file is imported into the SketchUp Plug-in, a translation between EnergyPlus IDF and OpenStudio OSM occurs. OpenStudio does not yet support translation of all objects from EnergyPlus IDF to OpenStudio OSM. To support users with workflows that switch between IDF and OSM, the SketchUp Plug-in allows users to export IDF objects that were not translated during the import. These objects are written to a plain text IDF file so the user may then reintegrate these untranslated objects with the IDF produced by the OpenStudio OSM to EnergyPlus IDF translation. After importing an EnergyPlus IDF, a prompt will alert the user about the objects that could not be translated to OpenStudio OSM.

![Export Untranslated IDF](img/plugin_reference_guide/width_untranslatedidf-importdialog.png)

*Above: User Prompt on EnergyPlus IDF Import*
</td>
</tr>
</tbody>
</table>
## Modeling Tools
Tools in this set create new zones, groups, and geometry.
<table class="table table-striped">
<thead>
<tr>
<th>Icons</th>
<th>Functionality</th>
</tr>
</thead>
<tbody>
<tr>
<td>![New Space Icon](img/plugin_reference_guide/new_space.png)
</td>
<td>
#### New Space
You can use the New Space tool to create a new OpenStudio space. Activate the tool and then click where you want your zone origin to be. An OpenStudio space is a top-level object in your SketchUp workspace. If you want to draw in the space, you first need to select the space, then enter it. There are two methods to select a space:

1. Click on the space. This may be difficult when other objects get in the way. 
2. Find and click on the space within SketchUp's Outliner Window. This option is suitable when option 1 is not. 

Now that the space has been selected, a user can enter the space before drawing surfaces. The three methods to enter a selected space are:

1. Double-click on the space, or double-click on the space within SketchUp's Outliner Window. 
2. Right-click on the space, or right-click on the space within SketchUp's Outliner Window. From the resulting contextual menu, click "Edit Group".
3. Click the Edit menu. Under this menu, choose "Group", or "Solid Group". Then click "Edit Group".

While inside a space, beside drawing spaces, you can add objects. Shading groups, interior partition groups, daylighting control points, and luminance maps can be added to a space. An alternate method to make new spaces is to copy and paste existing spaces. After creating a space and adding any objects, you can edit them using the [OpenStudio Inspector](#openstudio-inspector).

![Selected Space and SketchUp's Outliner Window](img/plugin_reference_guide/outliner_window_selected_space.png)

*Above: Screenshot of Selected Space and SketchUp's Outliner Window*

![Entered Space and SketchUp's Outliner Window](img/plugin_reference_guide/outliner_window_entered_space.png)

*Above: Screenshot of Entered Space and SketchUp's Outliner Window*

![New Space Inspect](img/plugin_reference_guide/width_inspector_space.png)

*Above: Screenshot of OpenStudio Inspector with an OpenStudio space selected*
</td>
</tr>
<tr>
<td>![New Shading Surface Group Icon](img/plugin_reference_guide/NewShading-24.png)
</td>
<td>
#### New Shading Surface Group
The New Shading Surface Group tool is used to create a new EnergyPlus shading group. Activate the tool and then click where you want your shading group origin to be. An EnergyPlus shading group can be a top-level object in your SketchUp workspace, or it can be placed within a space. If you want to draw in the group, you first need to select the group, then enter it. There are two methods to select a group:

1. Click on the group. This may be difficult when other objects get in the way. 
2. Find and click on the group within SketchUp's Outliner Window. This option is suitable when option 1 is not. 

Now that the group has been selected, a user can enter the group before drawing surfaces. The three methods to enter a selected group are:

1. Double-click on the group, or double-click on the group within SketchUp's Outliner Window. 
2. Right-click on the group, or right-click on the group within SketchUp's Outliner Window. From the resulting contextual menu, click "Edit Group".
3. Click the Edit menu. Under this menu, choose "Group", or "Solid Group". Then click "Edit Group".

You can create shading surface groups within or outside a space. Once you enter into a shading surface group, you can draw shading surfaces. An alternate method to make new shading groups is to copy and paste existing groups.

</td>
</tr>
<tr>
<td>![New Interior Partition Surface Group Icon](img/plugin_reference_guide/NewPartition-24.png)
</td>
<td>
#### New Interior Partition Surface Group
The New Interior Partition Surface Group tool is used to create a new OpenStudio Interior Partition Group. Activate the tool and then click where you want your interior partition group origin to be. If you want to draw in the group, you first need to select the group, then enter it. There are two methods to select a group:

1. Click on the group. This may be difficult when other objects get in the way. 
2. Find and click on the group within SketchUp's Outliner Window. This option is suitable when option 1 is not. 

Now that the group has been selected, a user can enter the group before drawing surfaces. The three methods to enter a selected group are:

1. Double-click on the group, or double-click on the group within SketchUp's Outliner Window. 
2. Right-click on the group, or right-click on the group within SketchUp's Outliner Window. From the resulting contextual menu, click "Edit Group".
3. Click the Edit menu. Under this menu, choose "Group", or "Solid Group". Then click "Edit Group".

You can create interior partition groups within a space only. Once you enter into an interior partition group, you can draw interior partition surfaces. An alternate method to make new interior partition groups is to copy and paste existing groups. Interior partition surfaces are not used as heat transfer surfaces, but they still have an associated construction. This construction, along with the surface area, is used to create internal mass that is used to run simulations and create objects for IDF export.
</td>
</tr>
<tr>
<td>![New Daylighting Controls Object Icon](img/plugin_reference_guide/NewDaylighting-24.png)
</td>
<td>
#### New Daylighting Control Object
The New Daylighting Control Object tool creates a new OpenStudio Daylighting Control object. This consists of a single sensor, which by default will be placed 30 inches above the surface you click on. You can move the object with the SketchUp Move tool or via the OpenStudio Inspector, and you can set many attributes that feed information to EnergyPlus and [Radiance Daylighting Measure](https://bcl.nrel.gov/node/84189). The Glare Calculation Azimuth Angle of View Direction also changes the appearance of the object in OpenStudio. To report data from the sensors in EnergyPlus, you also need to include light objects in your model. Most space types in the template files already include a light object.

You can create a Daylighting Control object from within a space, or you can copy and paste one from one space to another.

![Daylighting Control Objects](img/plugin_reference_guide/width_SU_DaylightingControlObjects.png)

*Above: Screenshot of Daylighting Controls Objects Inside a Space*

![Daylighting Control Object Selected](img/plugin_reference_guide/width_Inspector_DaylightingControl.png)

*Above: Screenshot of Daylighting Controls Inspector*

To associate a daylighting control object with a thermal zone, select the desired thermal zone object and then choose a primary (and optionally a secondary) daylighting control object. __NOTE__: Secondary daylighting control objects are not supported by Radiance at this time, they (and the percentage of load controlled by them) are ignored.

![OpenStudio Inspector With a Thermal Zone Selected](img/plugin_reference_guide/width_Inspector_thermal_zone.png)

*Above: Screenshot of OpenStudio Inspector with a Thermal Zone selected*
</td>
</tr>
<tr>
<td>![New Illuminance Map Icon](img/plugin_reference_guide/NewIlluminance-24.png)
</td>
<td>
#### New Illuminance Map
The New Illuminance Map tool creates a new OpenStudio Illuminance Map object. This consists of a rectangle with a grid representing map data points. By default it will be placed 30 inches above the surface you click on. You can move and resize the Illuminance Map with the SketchUp Move and Scale Tool and through the OpenStudio Inspector. To obtain data reported for the map in EnergyPlus, you also need to include a Daylighting Controls Object and lights. Most space types in the template files already include a light object.

You can create an illuminance map from  within a space, or you can copy and paste the map into a different space. You can have multiple illuminance maps in a space.

![Illuminance Map Object Inside a Space](img/plugin_reference_guide/illuminance_map.png)

*Above: Screenshot of Illuminance Map Object Inside a Space.*

![OpenStudio Inspector With an Illuminance Map Object Selected](img/plugin_reference_guide/width_Inspector_Illuminance_map.png)

*Above: Screenshot of OpenStudio Inspector With an Illuminance Map Object Selected*

To associate an illuminance map with a thermal zone, select the desired thermal zone object and then choose an illuminance map object.

![OpenStudio Inspector With a Thermal Zone Selected](img/plugin_reference_guide/width_Inspector_thermal_zone.png)

*Above: Screenshot of OpenStudio Inspector With a Thermal Zone Selected*
</td>
</tr>
<tr>
<td>![New Glare Sensor Icon](img/plugin_reference_guide/NewGlare-24.png)
</td>
<td>
#### New Glare Sensor
The New Glare Sensor creates a new OpenStudio Glare Sensor that supports [Radiance Daylighting Measure](https://bcl.nrel.gov/node/84189) simulations. Unlike the Illuminance Map and Daylighting Control Point, the Glare Sensor has no connection to a Thermal Zone. The default placement of the sensor is 30 inches above the surface you click on. You can move the object with the SketchUp move tool or through the OpenStudio Inspector, and you can set many attributes that feed information to Radiance.

You can create a Glare Sensor object from within a space, or you can copy and paste one into a different space.

![Glare Sensor](img/plugin_reference_guide/width_SU_glaresensor.png)

*Above: Screenshot of OpenStudio Inspector Glare Sensor selected*
</td>
</tr>
<tr>
<td>![Surface Matching Icon](img/plugin_reference_guide/SurfaceMatchingSelected-24.png)
</td>
<td>
#### Surface Matching
The Surface Matching tool is used to automatically set outside boundary conditions for space surfaces and assign the appropriate outside boundary object in adjacent spaces when "surface" is the outside boundary condition. The Surface Matching tool can match or unmatch surfaces in your selection or for the entire model.

![Surface Matching Dialog](img/plugin_reference_guide/SU_SurfaceMatchingDialog.png)

*Above: Screenshot of Surface Matching Dialog Box*
</td>
</tr>
<tr>
<td>![Set Space Attributes Icon](img/plugin_reference_guide/edit_spaces.png)
</td>
<td>
#### Set Space Attributes
The Space Attributes Tool can be used to set attributes for one or more selected spaces in your OpenStudio model. The first four pull-down menus (Space Type, Building Story, Construction Set, and Thermal Zone) set attributes of the selected space objects. The last two pull-down menus set attributes in the thermal zone associated with the selected spaces. Most of the menus allow you to choose from a list populated by the appropriate model objects or one of the following options: No Change, Clear Field, or Make a new Object. The exception is the ideal air loads, which has a "no change" option, or yes/no. The Thermostat menu is similar to the others, but does not offer the option to create a new thermostat.

![Space Attributes Dialog](img/plugin_reference_guide/SU_SpaceAttributes_a.png)

*Above: Screenshot of Space Attributes Dialog Box*

The following image shows the model after a new space type is applied to two selected spaces. The render mode is set to render by space type.

![Space Attributes Dialog example](img/plugin_reference_guide/width_SU_SpaceAttributes_b.png)

*Above: Screenshot of Space Attributes Dialog Box*
</td>
</tr>
<tr>
<td>![Create Spaces from Diagrams Icon](img/plugin_reference_guide/extrude.png)
</td>
<td>
#### Create Spaces from Diagrams
The Space Diagram Tool allows you to use native SketchUp tools to draw a space diagram representing your building's floor plan. After drawing the diagram, you can select the surfaces and create a single-story or multistory building based on the diagram. Your model will have one space for each surface in your diagram for each floor. You can use this tool multiple times. If you want to create a new diagram with a unique floor for higher floors, just repeat the steps you used for the original diagram.

Surfaces that are not horizontal will be ignored; surfaces with improper surface normals will be flipped over. Once made, the spaces are like any other OpenStudio space. You can enter the space and alter or extend your geometry to refine it beyond the basic geometry created by the space diagram tool.

![Space Diagram Dialog](img/plugin_reference_guide/width_SU_SpaceDigram_a.png)

*Above: Screenshot of Space Diagram Dialog Box*

The following image shows the resulting model after using the Space Diagram Tool. The loose geometry of the diagram will be saved onto a layer in SketchUp named "OpenStudio - Space Diagrams". Although the resulting spaces are saved in your OpenStudio model (OSM file), the diagram is not. If you want to keep the diagram, you can save the SketchUp file as well. Even if you do not save the SketchUp file you will still have access to the diagram until you quit. If you want to make a change to the diagram, you can delete the spaces, alter the diagram, and re-project them.

![Extruded Model](img/plugin_reference_guide/width_SU_SpaceDiagram_b.png)

*Above: Screenshot of Model After Using Space Diagram Tool*
</td>
</tr>
<tr>
<td>![Project Loose Geometry Icon](img/plugin_reference_guide/project_geo.png)
</td>
<td>
#### Project Loose Geometry
The Project Loose Geometry Tool allows you to use the native SketchUp tools to draw loose geometry onto surfaces of your building envelope. When you are ready to project these surfaces into the spaces of your model, you can open this tool and run it across all loose geometry or the selected loose geometry. Although this is often used to create fenestration, it can also be used to split a base surface into two base surfaces. The advantage of this tool--versus the traditional method of entering and drawing within a space--is that you can quickly create surfaces across many spaces without having to ever enter those spaces.

If some of the surfaces you have selected are not in the same plane as a building surface, they will be ignored.

![Project Loose Geometry Dialog](img/plugin_reference_guide/width_SU_ProjectGeo_a.png)

*Above: Screenshot of Project Loose Geometry Dialog Box*

The image below shows the resulting model after you have projected selected loose geometry. The loose geometry used for the projection operation will be moved to a layer in SketchUp named "OpenStudio - Loose Geometry". While the surfaces made in the spaces by the projection are saved in your OpenStudio Model (OSM file), the loose geometry is not saved in the OpenStudio Model. If you want to save this for future use, you can save the SketchUp file as well.

![Projected Geometry](img/plugin_reference_guide/width_SU_ProjectGeo_b.png)

*Above: Screenshot after Projecting Loose Geometry*
</td>
</tr>
</tbody>
</table>

## Information Tools
Tools in this set are generally for viewing model attributes, however some editing may be done as well.
<table class="table table-striped">
<thead>
<tr>
<th>Icons</th>
<th>Functionality</th>
</tr>
</thead>
<tbody>
<tr>
<td>![OpenStudio Inspector Icon](img/plugin_reference_guide/Inspector-24.png)</td>
<td>
#### OpenStudio Inspector
The OpenStudio Inspector displays information about your currently open OpenStudio model. The Inspector Window is divided into three main areas:

1. The top left is the "Select Type" pane. This has a partial list of OpenStudio object types and identifies how many objects of a specific type are in your model.
2. The bottom left pane is the "Select Object" pane. If you select a "Type" in the type pane, a list of objects of that type will be displayed in the "Select Object" pane. The objects listed in the "Select Object" pane have a number to indicate how many objects in the model refer to this object. Below the list are several buttons. The plus button to create a new object of this type, a copy button to copy the selected object, a minus button to delete the selected object, and a purge button to purge unused objects of this type. For some object types, one or more of these buttons may be disabled.
3. The third and last pane of the Inspector is the "Edit Object" pane at the right. This displays the fields for the currently selected object in the "Select Object" pane. Some fields accept a string; others are edited via a pull-down list. For some object types, some fields may be hidden from the Inspector or locked from editing. This is by design.

The OpenStudio Inspector is render mode aware. If you are in a render by class or render by boundary condition and use the SketchUp select tool to select a space, the OpenStudio space will be selected in the Inspector. If, however, you are in render by space type, thermal zone, or story mode, a space type, thermal zone, or story will be selected in the Inspector. This is assuming that the space is an assigned space type, thermal zone, or story. The construction render mode is similar, but you need to select a surface versus a space to select a construction object in the Inspector. Currently, if you select a thermal zone, space type, or construction object in the Inspector, it will not make a selection in the SketchUp model that correlates to this object.

The Inspector cannot select multiple objects at once. So if you select multiple spaces or surfaces in SketchUp, only one will be active in the Inspector. Similarly, you cannot use the Inspector to batch change the attributes for multiple objects at once.

The [Set Space Attributes for Selected Spaces](#set-space-attributes) tool on the OpenStudio toolbar does allow batch changes for space objects, and to a limited extent for zones that parent the spaces.

![Inspector Dialog](img/plugin_reference_guide/width_SU_Inspector.png)

*Above: Screenshot of OpenStudio Inspector With a Space Object Selected*

Screenshots of the OpenStudio Inspector with other object types selected are shown under the individual pages for those object types.
</td>
</tr>
<tr>
<td>![Surface Search Icon](img/plugin_reference_guide/SurfaceSearch-24.png)</td>
<td>
#### Surface Search
The OpenStudio Surface Search Tool enables you to search your model for surfaces with specific attributes. You can select the search results and visually isolate them by hiding all other surfaces. When you are finished, click "Unhide All" on the Surface Search Dialog to unhide the rest of the surfaces. Before searching on a large IDF, close SketchUp's Outliner Window to expedite the surface searching. While surfaces are hidden you can also use SketchUp's built-in "View Hidden Geometry" to quickly view all hidden surfaces.

You can also search based on Surface Orientation. To do this, set both "from" and "to" to a single value if you want to find surfaces that face a specific direction, or you can set unique values to show surfaces within a range of angles. The "Exclude Horizontal Surfaces" is generally intended to be used along with Surface Orientation search to hide surfaces with no azimuth. There are also check boxes to limit search results to non-convex surfaces or to surfaces with more than a set number of vertices. Once your search is complete, the resulting surfaces are selected in SketchUp. You can use this selection for other operations or scripts.

![Surface Search Dialog](img/plugin_reference_guide/SU_SurfaceSearch.png)

*Above: Screenshot of Surface Search Dialog*
</td>
</tr>
<tr>
<td>![Information Tool Icon](img/plugin_reference_guide/InfoTool-24.png)</td>
<td>
#### Information Tool
The OpenStudio Information Tool activates a cursor that, when placed over a model surface, shows the EnergyPlus or OpenStudio object attributes for that surface. The status bar shows the position of the cursor in world coordinates. The Information Tool is render mode aware and offers different data depending on your current render mode. The following screenshots and captions show the available data for the different render modes.

Render by Surface Type and Render by Boundary condition offer a variety of data. Hold down the shift, control, or shift+control keys while hovering over a surface. Modifier keys are shown by the appropriate captions.

(No Modifier Key)

![Information Tool in Render by Surface Type Mode Showing Surface Data](img/plugin_reference_guide/width_SU_Info_Type_basic.png)

*Above: Screenshot of Information Tool in Render by Surface Type Mode Showing Surface Data*

The same data are displayed while in Render by Boundary Condition mode.

("Shift" Modifier Key)

![Information Tool in Render by Surface Type Mode Showing Construction Data](img/plugin_reference_guide/width_SU_Info_Type_shift-const.png)

*Above: Screenshot of Information Tool in Render by Surface Type Mode Showing Construction Data*

The same data are displayed while in Render by Boundary Condition mode

("Ctrl" Modifier Key)

![Information Tool in Render by Surface Type Mode Showing Material Data](img/plugin_reference_guide/width_SU_Info_Type_shift-material.png)

*Above: Screenshot of Information Tool in Render by Surface Type Mode Showing Material Data*

The same data are displayed while in Render by Boundary Condition mode

("Shift + Ctrl" Modifier Key)

![Information Tool in Render by Surface Type Mode Showing Space Data](img/plugin_reference_guide/width_SU_Info_Type_shiftctrl-space.png)

*Above: Screenshot of Information Tool in Render by Surface Type Mode Showing Space Data*

The same data are displayed while in Render by Boundary Condition mode

(No Modifier Key)

![Information Tool in Render by Construction Mode Showing Construction Data](img/plugin_reference_guide/width_SU_info_construction.png)

*Above: Screenshot of Information Tool in Render by Construction Mode Showing Construction Data*

(No Modifier Key)

![Information Tool in Render by Space Type Mode Showing Space Type Data](img/plugin_reference_guide/width_SU_Info_spacetype.png)

*Above: Screenshot of Information Tool in Render by Space Type Mode Showing Space Type Data*

(No Modifier Key)

![Information Tool in Render by Thermal Zone Mode Showing Thermal Zone Data](img/plugin_reference_guide/width_SU_Info_ThermalZone.png)

*Above: Screenshot of Information Tool in Render by Thermal Zone Mode Showing Thermal Zone Data*
</td>
</tr>
<tr>
<td>![Errors and Warnings Icon](img/plugin_reference_guide/Warning-24.png)
</td>
<td>
#### OpenStudio Errors and Warnings
The Input File Errors and Warnings dialog automatically opens when there is an error or warning when opening or importing a file. Many common geometry errors (upside-down floor or roof, inside-out window, non-coplanar surfaces, etc.) are fixed automatically when you open the input file. This dialog will also report unsupported IDF objects on import.

![Errors and Warnings Dialog](img/plugin_reference_guide/error_02.png)

*Above: Screenshot of Errors and Warnings Dialog Box*
</td>
</tr>
</tbody>
</table>

## Rendering Modes
This toolbar contains render modes that are used to change how heat transfer, shading, and interior partition surfaces are rendered.
<table class="table table-striped">
<thead>
<tr>
<th>Icons</th>
<th>Functionality</th>
</tr>
</thead>
<tbody>
<tr>
<td>![Render by Surface Type Icon](img/plugin_reference_guide/render_surface_class.png)</td>
<td>
#### Render by Surface Type
The OpenStudio Render by Surface Type mode will render walls, roofs, windows, doors, floors, shading surfaces, and interior partition surfaces with unique colors. When you create new geometry, it will be automatically classified and rendered. If you try to assign a color yourself, the application will override you. You never need to use the paint bucket tool in this render mode.

If you set a surface construction to "Air Boundary," it will be rendered as a translucent checkerboard pattern. This construction is intended to represent thermal zone boundaries that do not represent physical walls in the building.

![Model in Render by Surface Type Mode](img/plugin_reference_guide/width_SU_Info_Type_basic.png)

*Above: Screenshot of Model in Render by Surface Type Mode*
</td>
</tr>
<tr>
<td>![Render by Boundary Condition Icon](img/plugin_reference_guide/render_boundary.png)</td>
<td>
#### Render by Outside Boundary Condition
The OpenStudio Render by Outside Boundary Condition mode will render surfaces by their outside boundary conditions. Only space surfaces have boundary conditions, so only they will be rendered with a color. Shading and interior partition surfaces will be white. This is a good rendering mode to diagnose problems with your model. It provides a quick way to globally look at boundary conditions throughout your model. You never need to use the paint bucket tool in this render mode. In the screenshot below, a section cut has been drawn to reveal the inside of the model, making the boundary condition of surfaces easy to see.

If you set a surface construction to "Air Boundary", it will be rendered as a translucent checkerboard pattern. This construction is intended to represent thermal zone boundaries that do not represent physical walls in the building.

![Model in Render by Boundary Condition Mode](img/plugin_reference_guide/SU_Render_Boundary.png)

*Above: Screenshot of Model in Render by Boundary Condition Mode*

The most common boundary conditions you will see are "Outdoors, Surface, and Ground", but there are actually 9 basic boundary modes, with a number of subsets within some conditions. For "Outdoors" there are also unique color variations to indicate if there is sun and or wind exposure. The following image shows the SketchUp materials for the various boundary conditions.

![Render by Boundary Condition Colors](img/plugin_reference_guide/tut_BoundaryColorWorksheet.png)

*Above: Render by Outside Boundary Condition Color Palette*
</td>
</tr>
<tr>
<td>![Render by Construction Icon](img/plugin_reference_guide/render_construction.png)</td>
<td>
#### Render by Construction
The OpenStudio Render by Construction mode renders surfaces by their construction. Although this seems similar to Render by Surface Type, there are a number of differences:

The Inspector Window is divided into three main areas:

- In Render by Construction mode, surfaces are rendered by their construction, so if two windows do not have the same construction they will not have the same render color. The same logic is applied to other surface types.
- Shading surfaces and interior partitions can have constructions assigned. If they do have constructions, they will be rendered just as space surfaces are.
- Construction properties of a surface can be applied directly to a surface, or they can be inherited from other objects (space, space type, story, or building). The first time you click the render by construction button, it shows hard assigned and inherited constructions. If you click it again, it shows only hard assigned constructions. Other surfaces do not have a render color. Clicking the render button again returns you to the default mode where hard assigned and inherited constructions are shown.

As a note, this is not a render by material mode. Hence, two constructions that have the same outside material will have unique render colors. However, in this render mode you can set render color using either the SketchUp paint bucket or the Inspector. To use the Inspector, select the construction object and set the RGB values. These color settings are part of the OpenStudio model and will be maintained the next time you open the model.

![Model in Render by Construction Mode](img/plugin_reference_guide/width_SU_info_construction.png)

*Above: Screenshot of Model in Render by Construction Mode*
</td>
</tr>
<tr>
<td>![Render by Space Type Icon](img/plugin_reference_guide/render_space_type.png)</td>
<td>
#### Render by Space Type
The OpenStudio Render by Space Type mode renders each space based on the Space Type assigned to it. This is a two state button. The first time you click it, hard assigned and inherited space types are rendered. If you click it again, only hard assigned space types are given a render color. Space types can be assigned to a space or inherited from the building object. You can assign space types to a space by using the "Set Attributes for Selected Spaces" tool or the OpenStudio Inspector. The installed templates have preassigned colors for each space type, but you can use SketchUp's paint bucket tool or the OpenStudio Inspector to change the colors. These changes are saved as part of your OpenStudio model.

![Render by Space Type Mode](img/plugin_reference_guide/width_SU_Info_spacetype.png)

*Above: Screenshot of Model in Render by Space Type Mode*
</td>
</tr>
<tr>
<td>![Render by Thermal Zone Icon](img/plugin_reference_guide/render_therm_zone.png)</td>
<td>
#### Render by Thermal Zone
The OpenStudio Render by Thermal Zone mode renders each space based on the thermal zone assigned to it. You can assign a thermal zone to a space using the "Set Attributes for Selected Spaces" tool or the OpenStudio Inspector. When a new thermal zone is made, a color will be assigned to it. You can change that color using SketchUp's paint bucket tool or the OpenStudio Inspector. These changes are saved as part of your OpenStudio model.

![Model in Render by Thermal Zone Mode](img/plugin_reference_guide/width_SU_Info_ThermalZone.png)

*Above: Screenshot of Model in Render by Thermal Zone Mode*
</td>
</tr>
<tr>
<td>![Render by Building Story](img/plugin_reference_guide/render_story.png)</td>
<td>
#### Render by Building Story
The OpenStudio Render by Building Story mode renders each space based on the building story assigned to it. You can assign a building story to a space using the "Set Attributes for Selected Spaces" tool or the OpenStudio Inspector. When a new story is made, a color will be assigned to it. You can change that color by using SketchUp's paint bucket tool or the OpenStudio Inspector. These changes are saved as part of your OpenStudio model.

When you use the "Create Spaces from Diagram" tool to generate a multistory building the stories are assigned as part of that process. There is also a user script that can infer building stories from a model that has none assigned.

![Model in Render by Building Story Mode](img/plugin_reference_guide/width_SU_Info_story.png)

*Above: Screenshot of Model in Render by Building Story Mode*
</td>
</tr>
<tr>
<td></td>
<td>
#### Render by Layer
The Render by Layer is a standard SketchUp feature. It will render content by layer. You can create layers to represent anything you want to track. For example, to organize your project by space types you can create or move zones to specific layers. Keep in mind that the layer structure isn't saved with the OSM file, so any layers you setup are just for your current session.

![Render By Layer mode](img/plugin_reference_guide/width_ss_by_layer.png)

*Above: Screenshot of Model in Render By Layer mode*
</td>
</tr>
<tr>
<td></td>
<td>
#### Render by Surface Normal
The Render by Surface Normal mode is built on the SketchUp face setting where faces are rendered with the default front and back colors as opposed to their assigned materials. The outside face of a heat transfer surface will render white and the inside face will render red. This works as a quick diagnostic tool to look for reversed faces. In a clean model you shouldn't see red from the outside. Shading surfaces are an exception to this where you will see red on one of the two sides of each shading surface.

![Render By Surface Normal mode](img/plugin_reference_guide/width_ss_surface_normal.png)

*Above: Screenshot of Model in Render By Surface Normal mode*
</td>
</tr>
</tbody>
</table>

##  Integration with Other OpenStudio Resources
This set contains tools integration for other OpenStudio applications and resources.
<table class="table table-striped">
<thead>
<tr>
<th>Icons</th>
<th>Functionality</th>
</tr>
</thead>
<tbody>
<tr>
<td>![Launch Current Model in OpenStudio Application Icon](img/plugin_reference_guide/os_24.png)</td>
<td>
#### Launch Current Model in OpenStudio Application
The Launch Current Model in OpenStudio Application button will launch your model in OpenStudio. The model will also stay open in the SketchUp Plug-in. If you make changes and save them in the OpenStudio application, be sure to reload the model in the SketchUp Plug-in before you make any changes in SketchUp.

You should also reload the model in the OpenStudio application if you have made additional changes in SketchUp.
</td>
</tr>
<tr>
<td>![Help Icon](img/plugin_reference_guide/Help-24_0.png)</td>
<td>
#### Help
This takes you to the online help for the OpenStudio SketchUp Plug-in, where you are now.
</td>
</tr>
</tbody>
</table>

##  Additional OpenStudio Plug-in Tools

#### About OpenStudio
For information on the SketchUp Plug-in version go to `Extensions->OpenStudio->About OpenStudio`.

![About Dialog](img/plugin_reference_guide/about_plugin.png)

*Above: Screenshot of SketchUp showing the about dialog path and window*

#### Preferences
For information on the SketchUp Plug-in version go to `Extensions->OpenStudio->Preferences`.

![Preferences Dialog](img/plugin_reference_guide/preferences.png)

*Above: The Preferences dialog enables you to set options related to updates, new thermal zone creation, user scripts, text editor, template OSM files, and the EnergyPlus engine.*

#### Check for Update
Check to see if you are using the most current release of the OpenStudio SketchUp Plugin by going to `Extensions->OpenStudio->Check For Update`.

![Update Dialog](img/plugin_reference_guide/updates.png)

*Above: Screenshot of Update Dialog Box*

##  Helpful SketchUp Tools
<table class="table table-striped">
<thead>
<tr>
<th>Icons</th>
<th>Functionality</th>
</tr>
</thead>
<tbody>
<tr>
<td>![Hide Rest of Model Icon](img/plugin_reference_guide/HideRest-24.png)</td>
<td>
#### Hide Rest of Model
This activates SketchUp's native view function to hide everything but your currently open group or component. Isolating your current zone make is much easier to work on inter-zone geometry.

![Model with rest of model visible](img/plugin_reference_guide/ss_hiderest_01.png)

*Above: Screenshot of Model with rest of model visible*

![model with rest of model hidde](img/plugin_reference_guide/ss_hiderest_02.png)

*Above: Screenshot of model with rest of model hidden*
</td>
</tr>
<tr>
<td>![Hidden Geometry Icon](img/plugin_reference_guide/SU_ViewHidden-24.png)</td>
<td>
#### View Hidden Geometry
This activates SketchUp's native view function to view hidden geometry. This is particularly useful if you have used OpenStudio's Surface Search tool and you need to see the rest of the model to get your bearings.

![Show Hidden Geometry](img/plugin_reference_guide/width_ss_show_hidden.png)

*Above: Screenshot of model in with View Hidden Geometry active*
</td>
</tr>
<tr>
<td>![X-Ray Mode](img/plugin_reference_guide/tbRenderTransparentLarge.png)</td>
<td>
#### View Model in X-Ray Mode
This activates SketchUp's native face style setting to render all surfaces translucently. This allows you to quickly see internal model elements without having to create section cuts or hide parts of the model.

![Active X-Ray Rendering](img/plugin_reference_guide/width_ss_xray.png)

*Above: Screenshot of model with X-ray Rendering active*
</td>
</tr>
</tbody>
</table>
