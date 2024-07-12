


# Create Bar From Space Type Ratios

## Description
Creates one or more rectangular building elements based on space type ratios of selected mix of building types, along with user arguments that describe the desired geometry characteristics.

## Modeler Description
The building floor area can be described as a footprint size or as a total building area. The shape can be described by its aspect ratio or can be defined as a set width. Because this measure contains both DOE and DEER inputs, care needs to be taken to choose a template compatable with the selected building types. See readme document for additional guidance.

## Measure Type
ModelMeasure

[//]: # (Only finds value if taxonomy method is added to measure.rb, won't read out of measure.xml)
## Taxonomy
Envelope.Form

___
## Table of Contents
- [Measure Overview](#measure-overview)<br/>
- [DOE and DEER Template and Building Type Mapping](#doe-and-deer-template-and-building-type-mapping)<br/>
  - [Functionality and Input Overview](#functionality-and-input-overview)<br/>
  - [Space Type Ratios](#space-type-ratio)<br/>
  - [Floor Area](#floor-area)<br/>
  - [Height, Stories, and Orientation](#height-stories-and-orientation)<br/>
  - [Building Shape](#building-shape)<br/>
  - [Window to Wall Ratio](#window-to-wall-ratio)<br/>
  - [Party Walls and Exposure of Top and Bottom Stories](#party-walls-and-exposure-of-top-and-bottom-stories)<br/>
  - [Calculation Methods and Articulation Logic](#calculation-methods-and-articulation-logic)<br/>
- [Development Comments](#development-comments)<br/>
- [Automatically Generated Argument List](#arguments)<br/>

## Measure Overview

The intent of this measure is to create basic geometry given a mix of building types and high level characteristics about the building envelope like the number of stories, aspect ratio, orientation etc. This will result in one or more rectangular building elements. The spaces will have stub space type assignments but will not have any internal loads or and the surfaces will not have constructions. The stub space type ratios are based on DOE prototype buildings, depending on the building type you select. The typical workflow is to run other measures after this measure that populate the stub space types and other model elements. This measure has more than 30 arguments and can look intimidating, but all of them have defaults, so it will run properly without you changing any arguments; however at a minimum you will typically want to at least choose the primary building type, building floor area, and number of stories above grade. Make use of additional arguments as you have a need to.

This is generally meant to run on an empty model, but it does not actively delete any geometry or objects that may already be in the model.

## DOE and DEER Template and Building Type Mapping

Note, that this particular measure has access to both DOE and DEER building types and templates (vintages). It is important when using this measure that the template and the selected building type(s) are all DOE or are all DEER. Below is a list of for each.

#### DOE
- Templates:
  - DOE Ref Pre-1980
  - DOE Ref 1980 - 2004
  - 90.1-2004
  - 90.1-2007
  - 90.1-2010
  - 90.1-2013
  - 90.1-2016
  - 90.1-2019
  - NREL ZNE Ready 2017 (not currently complete for all building types)
  - ComStock DOE Ref Pre-1980
  - ComStock DOE Ref 1980-2004
  - ComStock 90.1-2004
  - ComStock 90.1-2007
  - ComStock 90.1-2010
  - ComStock 90.1-2013
- Building Types:
  - SecondarySchool
  - PrimarySchool
  - SmallOffice
  - MediumOffice
  - LargeOffice
  - SmallHotel
  - LargeHotel
  - Warehouse
  - RetailStandalone
  - RetailStripmall
  - QuickServiceRestaurant
  - FullServiceRestaurant
  - MidriseApartment
  - HighriseApartment
  - Hospital
  - Outpatient
  - SuperMarket
  - Laboratory
  - LargeDataCenterLowITE
  - LargeDataCenterHighITE
  - SmallDataCenterLowITE
  - SmallDataCenterHighITE

#### DEER
- Templates:
  - DEER Pre-1975
  - DEER 1985
  - DEER 1996
  - DEER 2003
  - DEER 2007
  - DEER 2011
  - DEER 2014
  - DEER 2015
  - DEER 2017
  - DEER 2020
  - DEER 2025
  - DEER 2030
  - DEER 2035
  - DEER 2040
  - DEER 2045
  - DEER 2050
  - DEER 2055
  - DEER 2060
  - DEER 2065
  - DEER 2070
  - DEER 2075
- Building Types: (If you pick a DEER template then space types ratios should come from one or more of these building types)
  - Asm
  - DMo
  - ECC
  - EPr
  - ERC
  - ESe
  - EUn
  - GHs
  - Gro
  - Hsp
  - Htl
  - MBT
  - MFm
  - MLI
  - Mtl
  - Nrs
  - OfL
  - OfS
  - RFF
  - RSD
  - Rt3
  - RtL
  - RtS
  - SCn
  - SFm
  - SUn
  - WRf

[//]: # (Provide link to map DEER abbreviation to full building type name)
[//]: # (Would be nice to make these lists dynamic from the measure to they don't become outdated)

## Functionality and Input Overview

#### Space Type Ratios

While Create Bar from Building Type Ratios pulls in a pre-determined list and ratios of space types for each building type, this measures allows the modeler to bring in any combination of space types from multiple building types. The main limitation is that the space types need to come from the same OpenStudio Standards Template. You can't mix spaces from DOE and DEER or a space type that is unique to DOE Ref Pre-1980 and another space type unique to ASHRAE 90.1 2013.

The string is setup like a hash where the key is a concatenated BuildingType SpaceType combination separated by a vertical pipe and spaces " | ". Then a separator is added before the fractional value that looks like " => ". Spaces in the string are optional except for the one after the comma that is necessary. The space type ratio fractional values should add up to 1.0.

Below are a few example use cases for this measure
- If you want to use a space types from a prototype model but want to use a different mix of space types than the prototype building, which is generally where space type ratios in Create Bar from Building Type Ratios come from.
- You want to exclude or more more space types from a building type, such as a school auditorium.
- You want to pull in one or more extra space types from another building type such as adding a retail space to a restaurant.
- You want to make a a mixed use building that pulls a completely custom selection of space types together. Keep in mind when pulling in space types from multiple buildings that schedules may vary. That may be fine if your goal is a mixed use building, but if you are trying to make a new custom building type you need to be aware of this. As a note for more basic mixed use buildings Create Bar from Building Type Ratio supports up to 4 building types as inputs.

#### Floor Area

The typical use case for this measure is to set the total building floor area using the `Total Building Floor Area` argument. With this approach you can maintain a set building floor area while changing the number of stories. An alternate workflow is to set a static footprint area. This alternate workflow is enabled by setting a non-zero value for the `Single Floor Area` argument.

[//]: # (todo - add screenshot of same floor area on different num stories and shapes)

#### Height, Stories, and Orientation

The Floor to floor height for the building is controlled through the `Typical Floor to FLoor Height` argument. It is the first of a number of arguments where a value of 0 represents a **smart default**. If leave the default value of zero in place it will pick a building specific floor to floor height. For example 9' would be picked for a small hotel and 14' for a hospital.

Some building types have space type specific definitions for height that can be enabled with the `Enable Custom Height Bar Application` argument. If I building doesn't have any custom heights defined the value of this argument will not matter. One example of where this argument would have an impact is for the gym and auditorium in the SecondarySchool which would have a height 26' instead of the default school height of 13'. This is accomplished by pulling space types with a custom height into a separate rectangular building that sits away from the main structure. If this argument is set to false then all of the space types will have the same height, which for the school would be the default of 13'

![No Custom Height Space Types](./docs/custom_height_false.png?raw=true)
*Secondary school without custom space type heights enabled*

![Custom Height Space Types](./docs/custom_height_true.png?raw=true)
*Secondary school with custom space type heights enabled*

There are two arguments that control the number of stories the building has. `Number of Stories Above Grade` as the name suggests determines how many above grade stories the building has. There is an argument later on `Calculation Method for Story Multiplier` that determines if zone multipliers are used to minimize the number of zones in the model. `Number of Stories Below Grade` will and stories with ground boundary conditions for walls. It is worth pointing out that currently there isn't building type specific logic to determine which space types end up in the basement, so you may not have the desired outcome in basements. That may be addressed in futrue updates to this measure. Basements are not added below custom space type bars. Note that `Number of Stories Above Grade` doesn't have to take a whole number. If you chose a value of `5.5` then the building will have 4 full stories and a half 5th story. `Number of Stories Below Grade` has to be a whole number

![Baesment](./docs/basement.png?raw=true)
*Secondary school with 1 below grade story*

The `Building Rotation` argument will rotate the building after the geometry is created. So if you for example make a bar oriented east/west and then rotate it 80 degrees, the end result will be a bar just off of being north/south orientation. A positive value rotates clockwise. If you had a party wall assigned to the south facade, after the 80 degree rotation, those party walls would not be 10 degrees south of due west.

#### Template

The `Template` is used in OpenStudio measures and the OpenStudio standards gem to refer to a specific target building code standard or vintage. For example ASHRAE 90.1 2004 or the DOE Ref Pre-1980 vintage used for the DOE reference buildings. Template is used here along with the building types selected to define the list and ratios of space types in the model. While generally the space type ratio is the same across different templates there are exceptions to this such as the inclusion of data centers in newer LargeOffice buildings.

#### Building Shape

While the floor area and number of stories arguments determine the size of the building footprint, the `Ratio of North/South Facade Length Relative to East/West Facade Length` argument is the primary argument ot determine the shape of the footprint(s). If you enter a value of 2.0 then there wil be twice as much exterior wall area on the north and south facade as there is on the east and west facade; the footprint will be a rectangle twice as long as it is wide. I value of 0.5 would make a similar rectangle but oriented in the other direction and would have a similar outcome to rotating the building made iwth a value of 2.0 by 90 degrees. This is another argument with a **smart default** of 0.0. If you don't have a specific value you want to use, passing in the default value of 0 will pick a building type specific fault based on the prototype model description.

![Aspect Ratio 2.0](./docs/ar_2.png?raw=true)
*Aspect Ratio of 2.0*

![Aspect Ratio 1.0](./docs/ar_1.png?raw=true)
*Aspect Ratio of 1.0*

![Aspect Ratio 0.5](./docs/ar_05.png?raw=true)
*Aspect Ratio of 0.5*

The `Perimeter Multiplier` provides a mechanism to maintain your target footprint floor area and your target aspect ratio while increasing the total exterior perimeter exposure. The intent here is to support, in a simplified way, the impacts of non-rectangular buildings. This is accomplished by adding a second bar when necessary that is turned 90 degrees relative ot the primary bar but maintains the same width. These two building elements are separate from each other and from the custom height bar, if it exists. If you keep the default value of 0.0 the measure will lookup a value inferred from the prototype building. For most prototype building types this will result in a value of 1.0, because the buildings are rectangular, but for PrimarySchool, SecondarySchool, and Outpatient a value larger than 1.0 will be used. If you use or a smart default of 1.0 applied for this argument you get a single bar. As you increase the multiplier higher than 1.0 it will add a second bar to increase the building perimeter while maintaining both the floor area and the north south to east west aspect ratio. Below is an example of the calculation method. If you use a very small multiplier such as 1.05, a situation may exist that would have too small of a secondary bar to properly model. In this case the single bar will just be extended a bit to achieve the target total perimeter, but will sacrifice meeting the requested north/south versus east/west aspect ratio. In some cases with when the multiplier is not high enough above 1 the ends of the secondary bar may be made adiabatic to achieve target values.

![Perim Mult b](./docs/perim_mult_2012.png?raw=true)
*Aspect Ratio of 2.0 with a Perimeter Multiplier of 1.2. Able to add second bar but needs to have adiabatic ends. See earlier screenshot for Aspect Ratio of 2.0 with Perimeter Multiplier of 1.0*

![Perim Mult c](./docs/perim_mult_2020.png?raw=true)
*Aspect Ratio of 2.0 with a Perimeter Multiplier of 2.0.*

`Perimeter Multiplier` example use case:
- inputs are for a 10,000 ft^2 2 story building with an aspect ratio of
- With a `Perimeter Multiplier` of one this would result in a single bar that is 50' by 100'. This would have a total perimeter of 300'
- With a `Perimeter Multiplier` of 1.5 the target perimeter value will be increased to 450'. This would result in two bar elements that have a combined footprint area of 5000 ft^2 (per story) and a combined perimeter of 450'.

Representing for example a U shaped footprint (which might have a "Perimeter Multiplier" of 3 or higher) with a single bar would either under-estimate the perimeter if a reduced bounding box was fit around it to match the area, or over skew the orientation if a single long skinny bar was created with almost all of the exposure in only two opposing cardinal directions. A more common term that could have been used for an argument would have been a 1building perimeter to floor area ratio(P/A)`. That was considered but there could have been incompatible inputs passed in where the target P/A isn't possible for the given footprint area and aspect ratio. A 100' square has a P/A of 0.04 while a 1000' square has a ratio 0.004. In addition to changing with size it would also change with the shape of the rectangle. This measure in uses the multiplier to increase the perimeter of the foorprint(s) given a specified area and aspect ratio above the minim possible rectangular perimeter value. If you just want to make a single bar, you can leave the value at 1.0 and not worry about it. As a last note, if you also are using a custom height bar, that excluded from the calculation of the primary bar(s)

`Bar Width` can be specified. If you know due to the layout of your building or other factors that you have a specific building width you would like to have you can enter that in the `Bar Width` argument. `Perimeter Multiplier` will be ignored when a non-zero `Bar Width` is used. If you leave this argument value at the default value of 0 then it will not impact the model. A non-zero value here will also bypass the `Perimeter Multiplier` argument.

`Bar Separation Distance Multiplier` is used when there is more than one bar, to specify the distance between the bars as a multiple of the building height. The default value is 10 x the height of the building, but that can be customized here. Currently 0 is not a valid multiplier, since all the intersection, matching, and fenestration occur in the individual bar elements. When two bars are required to meet the `Perimeter Multiplier` target they are in configuration of an with the vertical leg to the right of the horizontal leg, like a backwards "L". If a custom height bar was used that will be positioned to the left of the other bars.

![Sept Dist 0.5](./docs/sep_05.png?raw=true)
*Bar Separation Distance Multiplier of 0.5*

![Sept Dist 3](./docs/sep_3.png?raw=true)
*Bar Separation Distance Multiplier of 3*

![Sept Dist 10](./docs/sep_10.png?raw=true)
*Bar Separation Distance Multiplier of 10*

The intent of the high default `Bar Separation Distance Multiplier` is to not make assumptions about self shading that may occur due to the specific configuration of the geometry, but that ends up wtih no or minimal self shading. You can lower this value and rotate to add in some self shading, but it will have limited flexibility. Future versions may better address self shading and context shading options, or these may be added as separate measures after this one.

#### Window to Wall Ratio

There is just a single `Window to Wall Ratio` argument for this measure. It has a **smart default** value of 0, that when selected applies a building type specific default window to wall ratio. The application of smart default or specified ratio is not orientation specific. Some building types (currently just Warehouse) have a space type specific override to the building level default; specifically for the office space type which has a window to wall ratio of 0.71 but represents only a small portion of the building.

![Custom WWR](./docs/wwr_custom.png?raw=true)
*Example of space type specific WWR for warehouse model, shown with custom space type height enabled*

Orientation specific characteristics and/or overhangs may be added to this in the future, but are currently possible by adding existing measure to the workflow after this measure. Currently skylights are not directly supported either, but can be added outside of this measure.

#### Party Walls and Exposure of Top and Bottom Stories

There are a number of measure arguments that enable portions of the building facade to be modeled as having adjacent structures with similar interior conditions instead of having exterior exposure on all sides. Additionally there are arguments that allow you to mimic a mid-floor(s) only model that disable ground exposure of the floor of the bottom story of your model and/or exterior exposure of the roof of the top floor.

There are two approaches for the party walls. One approach is a single fraction defined in the `Fraction of Exterior Wall Area with Adjacent Structure` will attempt to meet your target party wall fraction on multiple facades of the building without the modeler having to specific specific orientations. This should be set to 0 if you plan to use the orientation specfic arguments described below.

The second approach is ideal when you know more about the conditions of a specific condition and allows you to enter an integer for reach cardinal direction with arguments named `Number of North facing stories with party wall` etc. You can for example have a 5 story building that has a 1 story building to the west, a 3 story building to the east, a 2 story building to the north, and exterior exposure to the south. If you are using this as part of a parametric study where number of stories is changeling and you want all of a specific facade to be adjacent to a building that is taller, You can enter an large value such as 99 for the value; the overly large value relative to number of stories in the building will not create any issues.

The ability to model what is in essence party floor/ceilings in your model are controlled through two bool arguments named `Is the Bottom Story Exposed to Ground` and `Is the Top Story an Exterior Roof`

![Party Wall](./docs/party.png?raw=true)
*1 story Party wall on south, 2 story on east, story multipliers not enabled*

![Party Wall](./docs/party_multi.png?raw=true)
*1 story Party wall on south, 2 story on east, story multipliers enabled. Measure identifies non-unique stories to setup story multipliers.*

![Party Wall](./docs/no_party_mult.png?raw=true)
*For comparison this shows the same 6 story building with multipliers enabled without the party walls.*

#### Calculation Methods and Articulation Logic

- `Calculation Method for Story Multiplier` is used to determine if zone multipliers should be used as story multipliers on taller buildings. This reduces the number of zones and should speed up simulations, but may not be appropriate in all cases. If you select a value of `None` then no (non 1) zone multipliers will be used and zones will be modeled for all building stories. The other choice currently available is `Basements Ground Mid Top` which for a building without party walls will never have more than 4 modeled stories regardless of the number of stories. The measure will evaluate uniqueness of stories taking into account party walls and may add additional stories to accommodate this. Note that while the size of space types on a story with a multiplier greater than 1 may appear smaller, they may be larger once the multiplier is applied.  More options may be added in the future, specifically vacations of something like `Basements Ground Midx5 Top` that for very tall buildings would and multiple mid-stories vs. a single mid-story.

- `Make Mid Story Floor Surfaces Adiabatic` determines of surface intersection and surface matching should be applied between stories (when they are adjacent and capable of being matched e.g. story multiplier not used). Setting this to true can reduce the number of surfaces in the model

- `Division Method for Bar Space Types` is used to determine how the internal zoning and space type division of the bar(s) is approached. The default value of `Multiple Space Types - Individual Stories Sliced` is the primary method that should be used. It creates what looks like a core and perimeter model, but slices it up by space type, with large space types moving towards the upper stories. There is some basic logic to minimize small slivers of a space type being created that would create thermal zones with small volumes that may not simulate well. The two largest space types will be at the ends of the bars to minimize having excessive exterior exposure on a zone with a small floor area. `Multiple Space Types - Simple Sliced` is an earlier method that pre-dates the individual sliced stories. The logic is similar but it puts all space types on every building story and creates multiple stories with the exact same slicing logic. `Single Space Type - Core and Perimeter` is not sliced up by space type and isn't relevant for building types with multiple space types. If you do have a building type with only a single space type, like small or medium office, the multiple space type methods will still work. The single space type method will eventually be deprecated.

![Individualy Sliced](./docs/bar_sliced_indiv.png?raw=true)
*Multiple Space Types - Individual Stories Slice method that has been used through the examples above*

![Simple Sliced](./docs/bar_sliced_simple.png?raw=true)
*Multiple Space Types - Simple Sliced, every story has all space types*

![Single Core Perimeter](./docs/bar_single_core_perim.png?raw=true)
*Single Space Type - Core and Perimeter, should not be a reason to use this*

- The `Double Loaded Corridor` argument when enabled, take a circulation space type, when it exists, and pair it with the default space type, and will create a double loaded corridor for the space division and thermal zoning, instead of the typical sliced core and perimeter zoning. This not only alters the geometry of the internal division, but creates a hybrid slice that instead of being a single space type, contains a circulation space type running down the center of the bar, surrounded by spaces of another space type. For an example, with a primary school the corridor will be paired with the classrooms. The width of the corridor is determined to achieve the proper space type ratios of the two space types.
- Not enabled for the DEER Building Types as of OpenStudio 3.2.

![Double Loaded Corridor](./docs/bar_double_loaded.png?raw=true)
*Double Loaded Corridor with classroom and corridor for secondary school.*

- The `Choose Space Type Sorting Method` only comes into play when you are modeling a building with more than one building type. With `Size` selected the space types are organized by size, independent of the building type, with smaller space types on lower stories and larger space types on higher stories. `Building Type > Size` is a newer method that first sorts by building type, putting smaller building types on lower stories, and then within a building type will sort by size. An example use case for this method is when modeling a 4 story building with 25% office and 75% large hotel. With the `Building Type > Size` value the first story will be entirely office, with the rest of the stories large hotel. If the `Size` value is selected, building types wll be mixed together.

![Sort Space Types by Building Types](./docs/story_sort.png?raw=true)
*Building Type > Size used to sort by building types, and then by space types.*

- `Use Upstream Argument Values` can be left at the default value of true for most cases. This argument shows up in a number of measures, and then intent is to enable the synchronization of arguments that may be used as a variable in a parametric study across multiple measures. Generally a measures has unique argument names that don't exist in other measures, but some such ase `template` are used frequently. This argument, when set to true, will use the value of `template` from an earlier measure in the workflow, if found, in place of what is entered as the argument value for this measure.

## Development Comments

This measure relies on the openstudio-standards gem which is included in the OpenStudio CLI as well as a number of resources files listed below that are contained in the measure's `resources` directory. Other than the arguments almost none of the measure code is in the measure.rb file. The resource files are used by a number of measures and should be updated from the shared library and not within this measure.
- os_lib_model_generation.rb
- os_lib_model_simplification.rb
- os_lib_geometry.rb
- os_lib_helper_methods.rb

___

*(Automatically generated argument information follows)*

## Arguments

## Arguments


### Target Standard

**Name:** template,
**Type:** Choice,
**Units:** ,
**Required:** true,
**Model Dependent:** false

**Choice Display Names** ["DOE Ref Pre-1980", "DOE Ref 1980-2004", "90.1-2004", "90.1-2007", "90.1-2010", "90.1-2013", "90.1-2016", "90.1-2019", "ComStock DOE Ref Pre-1980", "ComStock DOE Ref 1980-2004", "ComStock 90.1-2004", "ComStock 90.1-2007", "ComStock 90.1-2010", "ComStock 90.1-2013", "ComStock 90.1-2016", "ComStock 90.1-2019", "NREL ZNE Ready 2017", "DEER Pre-1975", "DEER 1985", "DEER 1996", "DEER 2003", "DEER 2007", "DEER 2011", "DEER 2014", "DEER 2015", "DEER 2017", "DEER 2020", "DEER 2025", "DEER 2030", "DEER 2035", "DEER 2040", "DEER 2045", "DEER 2050", "DEER 2055", "DEER 2060", "DEER 2065", "DEER 2070", "DEER 2075", "ComStock DEER Pre-1975", "ComStock DEER 1985", "ComStock DEER 1996", "ComStock DEER 2003", "ComStock DEER 2007", "ComStock DEER 2011", "ComStock DEER 2014", "ComStock DEER 2015", "ComStock DEER 2017", "ComStock DEER 2020"]


### Space Type Ratios String
Hash of Space Types with Building and Space Type name as Key and Fraction as value. All space types should come from the selected OpenStudio Standards template. Example entry is (BuildingType | SpaceType => 0.25)
**Name:** space_type_hash_string,
**Type:** String,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Total Building Floor Area

**Name:** total_bldg_floor_area,
**Type:** Double,
**Units:** ft^2,
**Required:** true,
**Model Dependent:** false


### Single Floor Area
Non-zero value will fix the single floor area, overriding a user entry for Total Building Floor Area
**Name:** single_floor_area,
**Type:** Double,
**Units:** ft^2,
**Required:** true,
**Model Dependent:** false


### Typical Floor to FLoor Height
Selecting a typical floor height of 0 will trigger a smart building type default.
**Name:** floor_height,
**Type:** Double,
**Units:** ft,
**Required:** true,
**Model Dependent:** false


### Enable Custom Height Bar Application
This is argument value is only relevant when smart default floor to floor height is used for a building type that has spaces with custom heights.
**Name:** custom_height_bar,
**Type:** Boolean,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Number of Stories Above Grade

**Name:** num_stories_above_grade,
**Type:** Double,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Number of Stories Below Grade

**Name:** num_stories_below_grade,
**Type:** Integer,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Building Rotation
Set Building Rotation off of North (positive value is clockwise). Rotation applied after geometry generation. Values greater than +/- 45 will result in aspect ratio and party wall orientations that do not match cardinal directions of the inputs.
**Name:** building_rotation,
**Type:** Double,
**Units:** Degrees,
**Required:** true,
**Model Dependent:** false


### Ratio of North/South Facade Length Relative to East/West Facade Length
Selecting an aspect ratio of 0 will trigger a smart building type default. Aspect ratios less than one are not recommended for sliced bar geometry, instead rotate building and use a greater than 1 aspect ratio.
**Name:** ns_to_ew_ratio,
**Type:** Double,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Perimeter Multiplier
Selecting a value of 0 will trigger a smart building type default. This represents a multiplier for the building perimeter relative to the perimeter of a rectangular building that meets the area and aspect ratio inputs. Other than the smart default of 0.0 this argument should have a value of 1.0 or higher and is only applicable Multiple Space Types - Individual Stories Sliced division method.
**Name:** perim_mult,
**Type:** Double,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Bar Width
Non-zero value will fix the building width, overriding user entry for Perimeter Multiplier. NS/EW Aspect Ratio may be limited based on target width.
**Name:** bar_width,
**Type:** Double,
**Units:** ft,
**Required:** true,
**Model Dependent:** false


### Bar Separation Distance Multiplier
Multiplier of separation between bar elements relative to building height.
**Name:** bar_sep_dist_mult,
**Type:** Double,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Window to Wall Ratio
Selecting a window to wall ratio of 0 will trigger a smart building type default.
**Name:** wwr,
**Type:** Double,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Fraction of Exterior Wall Area with Adjacent Structure
This will impact how many above grade exterior walls are modeled with adiabatic boundary condition.
**Name:** party_wall_fraction,
**Type:** Double,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Number of North facing stories with party wall
This will impact how many above grade exterior north walls are modeled with adiabatic boundary condition. If this is less than the number of above grade stoes, upper flor will reamin exterior
**Name:** party_wall_stories_north,
**Type:** Integer,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Number of South facing stories with party wall
This will impact how many above grade exterior south walls are modeled with adiabatic boundary condition. If this is less than the number of above grade stoes, upper flor will reamin exterior
**Name:** party_wall_stories_south,
**Type:** Integer,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Number of East facing stories with party wall
This will impact how many above grade exterior east walls are modeled with adiabatic boundary condition. If this is less than the number of above grade stoes, upper flor will reamin exterior
**Name:** party_wall_stories_east,
**Type:** Integer,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Number of West facing stories with party wall
This will impact how many above grade exterior west walls are modeled with adiabatic boundary condition. If this is less than the number of above grade stoes, upper flor will reamin exterior
**Name:** party_wall_stories_west,
**Type:** Integer,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Is the Bottom Story Exposed to Ground
This should be true unless you are modeling a partial building which doesn't include the lowest story. The bottom story floor will have an adiabatic boundary condition when false.
**Name:** bottom_story_ground_exposed_floor,
**Type:** Boolean,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Is the Top Story an Exterior Roof
This should be true unless you are modeling a partial building which doesn't include the highest story. The top story ceiling will have an adiabatic boundary condition when false.
**Name:** top_story_exterior_exposed_roof,
**Type:** Boolean,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Calculation Method for Story Multiplier

**Name:** story_multiplier,
**Type:** Choice,
**Units:** ,
**Required:** true,
**Model Dependent:** false

**Choice Display Names** ["None", "Basements Ground Mid Top"]


### Make Mid Story Floor Surfaces Adiabatic
If set to true, this will skip surface intersection and make mid story floors and celings adiabatic, not just at multiplied gaps.
**Name:** make_mid_story_surfaces_adiabatic,
**Type:** Boolean,
**Units:** ,
**Required:** true,
**Model Dependent:** false


### Division Method for Bar Space Types
To use perimeter multiplier greater than 1 selected Multiple Space Types - Individual Stories Sliced.
**Name:** bar_division_method,
**Type:** Choice,
**Units:** ,
**Required:** true,
**Model Dependent:** false

**Choice Display Names** ["Multiple Space Types - Simple Sliced", "Multiple Space Types - Individual Stories Sliced", "Single Space Type - Core and Perimeter"]


### Double Loaded Corridor
Add double loaded corridor for building types that have a defined circulation space type, to the selected space types.
**Name:** double_loaded_corridor,
**Type:** Choice,
**Units:** ,
**Required:** true,
**Model Dependent:** false

**Choice Display Names** ["None", "Primary Space Type"]


### Choose Space Type Sorting Method

**Name:** space_type_sort_logic,
**Type:** Choice,
**Units:** ,
**Required:** true,
**Model Dependent:** false

**Choice Display Names** ["Size", "Building Type > Size"]


### Use Upstream Argument Values
When true this will look for arguments or registerValues in upstream measures that match arguments from this measure, and will use the value from the upstream measure in place of what is entered for this measure.
**Name:** use_upstream_args,
**Type:** Boolean,
**Units:** ,
**Required:** true,
**Model Dependent:** false





