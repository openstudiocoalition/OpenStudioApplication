<h1>CBECC Materials and Constructions</h1>
[CBECC-Com (California Building Energy Code Compliance)](http://bees.archenergy.com/software.html) is an open source project that may be used by Code Agencies, Rating Authorities, or Utility Programs in the development of energy codes, standards, or efficiency programs. Architects, engineers, and energy consultants may also use these tools to demonstrate compliance with energy codes or beyond-code programs. OpenStudio models can be exported to the SDD format which may then be imported into CBECC-Com.  OpenStudio 1.6.0 added the ability to specify material and construction properties needed by CBECC-Com, these are then exported to SDD and imported into CBECC-Com.  This tutorial will explain how to set these properties using the OpenStudio application so they can be properly imported into CBECC-Com.  Future versions of OpenStudio will add additional information to the SDD export with the ultimate goal of being able to run an exported SDD from OpenStudio through the CBECC-Com interface with no manual modification.

## Material Measure Tags
In order to apply energy codes to a SDD model, CBECC-Com requires information that is not used by energy simulation software.  OpenStudio has augmented several model objects with measure tags that can be used to add this information to an OpenStudio model.  These tags allow you to specify information like composite framing material or the standards identifier used to reference a particular material.  This information is not used when running an energy simulation using OpenStudio.  However, this information is included in the SDD export and used by CBECC-Com to apply energy codes.  These tags may also be used by measures that operate on the OpenStudio model.  OpenStudio measure tags are populated with a set of suggested values that are compatible with CBECC-Com.  You may choose to enter any value that you wish, but only the suggested values will work with CBECC-Com.  The set of measure tags available for a Material object is shown in the figure below.  In this example, the material is tagged as belonging to the standards category Roofing.  CBECC-Com provides a set of identifiers for materials in this category, choosing one of these identifiers specifies the material in CBECC-Com's library.  The performance values used in the OpenStudio model are not translated to SDD, only the standards category and identifier as this is all that CBECC-Com allows for compliance analysis.

[![Material Measure Tags](img/cbecc_materials_constructions/cbecc_materials_material1.jpg "Click to view larger version.")](img/cbecc_materials_constructions/cbecc_materials_material1.jpg)

The Composite material standards category behaves slightly differently.  For this category, you are prompted to specify framing material, configuration, depth, size, and cavity insulation for your composite layer.  CBECC-Com uses these properties to model your composite material, there is no standards identifier for composite materials.

[![Composite Material Measure Tags](img/cbecc_materials_constructions/cbecc_materials_material2.jpg "Click to view larger version.")](img/cbecc_materials_constructions/cbecc_materials_material2.jpg)

When modeling fenestration, no standards information is needed at the material level as this is input at the construction level in CBECC-Com.  The only performance data which is translated from OpenStudio material objects to SDD are the u-factor, solar heat gain coefficient, and visible transmittance properties of Simple Glazing System Window objects.

[![Simple Glazing System Window Measure Tags](img/cbecc_materials_constructions/cbecc_materials_material3.jpg "Click to view larger version.")](img/cbecc_materials_constructions/cbecc_materials_material3.jpg)

## Construction Measure Tags

At the construction level, the only measure tags available for opaque constructions are the intended surface type and the construction type, the values for construction type are  populated based on the intended surface type.

[![Composite Construction Measure Tags](img/cbecc_materials_constructions/cbecc_materials_constructions2.jpg "Click to view larger version.")](img/cbecc_materials_constructions/cbecc_materials_constructions2.jpg)

For fenestration constructions that consist of a single layer of a Simple Glazing System Window, the performance values in the Simple Glazing System Window will be used.  However, if you have specified your window constructions in a different way (e.g. layer by layer) you can use the fenestration measure tags to specify data (e.g. number of panes, frame type) that CBECC-Com will use to look up default performance information for compliance analysis.

[![Composite Construction Measure Tags](img/cbecc_materials_constructions/cbecc_materials_constructions1.jpg "Click to view larger version.")](img/cbecc_materials_constructions/cbecc_materials_constructions1.jpg)

## Load the CECTemplate library
It is possible to assign measure tags to your own materials.  However, there is no guarantee that the performance values in your materials will match those used by CBECC-Com for compliance analysis.  Therefore, OpenStudio has been distributed with a library of materials which include both performance data and the measure tags used by CBECC-Com.  This library file may be loaded in the OpenStudio application using "File->Load Library" and selecting CECTemplate.osm.  Currently, this library contains only materials, it does not include constructions or constructions sets, these may be added at a later date.
