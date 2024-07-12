# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

# see the URL below for information on how to write OpenStudio measures
# http://openstudio.nrel.gov/openstudio-measure-writing-guide

# see the URL below for information on using life cycle cost objects in OpenStudio
# http://openstudio.nrel.gov/openstudio-life-cycle-examples

# see the URL below for access to C++ documentation on model objects (click on "model" in the main window to view model objects)
# http://openstudio.nrel.gov/sites/openstudio.nrel.gov/files/nv_data/cpp_documentation_it/model/html/namespaces.html

require 'openstudio-standards'

# start the measure
class CreateBarFromSpaceTypeRatios < OpenStudio::Measure::ModelMeasure

  # define the name that a user will see, this method may be deprecated as
  # the display name in PAT comes from the name field in measure.xml
  def name
    return 'Create Bar From Space Type Ratios'
  end

  # human readable description
  def description
    return 'Creates one or more rectangular building elements based on space type ratios of selected mix of building types, along with user arguments that describe the desired geometry characteristics.'
  end

  # human readable description of modeling approach
  def modeler_description
    return 'The building floor area can be described as a footprint size or as a total building area. The shape can be described by its aspect ratio or can be defined as a set width. Because this measure contains both DOE and DEER inputs, care needs to be taken to choose a template compatable with the selected building types. See readme document for additional guidance.'
  end

  # used to populate taxonomy in readme.md
  def taxonomy
    return 'Envelope.Form'
  end

    # remove existing non resource objects from the model
  # technically thermostats and building stories are resources but still want to remove them.
  def remove_non_resource_objects(runner, model, options = nil)
    if options.nil?
      options = {}
      options[:remove_building_stories] = true
      options[:remove_thermostats] = true
      options[:remove_air_loops] = true
      options[:remove_non_swh_plant_loops] = true

      # leave these in by default unless requsted when method called
      options[:remove_swh_plant_loops] = false
      options[:remove_exterior_lights] = false
      options[:remove_site_shading] = false
    end

    num_model_objects = model.objects.size

    # remove non-resource objects not removed by removing the building
    if options[:remove_building_stories] then model.getBuildingStorys.each(&:remove) end
    if options[:remove_thermostats] then model.getThermostats.each(&:remove) end
    if options[:remove_air_loops] then model.getAirLoopHVACs.each(&:remove) end
    if options[:remove_exterior_lights] then model.getFacility.exteriorLights.each(&:remove) end
    if options[:remove_site_shading] then model.getSite.shadingSurfaceGroups.each(&:remove) end

    # see if plant loop is swh or not and take proper action (booter loop doesn't have water use equipment)
    model.getPlantLoops.each do |plant_loop|
      is_swh_loop = false
      plant_loop.supplyComponents.each do |component|
        if component.to_WaterHeaterMixed.is_initialized
          is_swh_loop = true
          next
        end
      end

      if is_swh_loop
        if options[:remove_swh_plant_loops] then plant_loop.remove end
      else
        if options[:remove_non_swh_plant_loops] then plant_loop.remove end
      end
    end

    # remove water use connections (may be removed when loop is removed)
    if options[:remove_swh_plant_loops] then model.getWaterConnectionss.each(&:remove) end
    if options[:remove_swh_plant_loops] then model.getWaterUseEquipments.each(&:remove) end

    # remove building but reset fields on new building object.
    building_fields = []
    building = model.getBuilding
    num_fields = building.numFields
    num_fields.times.each do |i|
      building_fields << building.getString(i).get
    end
    # removes spaces, space's child objects, thermal zones, zone equipment, non site surfaces, building stories and water use connections.
    model.getBuilding.remove
    building = model.getBuilding
    num_fields.times.each do |i|
      next if i == 0 # don't try and set handle
      building_fields << building.setString(i, building_fields[i])
    end

    # other than optionally site shading and exterior lights not messing with site characteristics

    if num_model_objects - model.objects.size > 0
      runner.registerInfo("Removed #{num_model_objects - model.objects.size} non resource objects from the model.")
    end

    return true
  end

  def arguments(model)
    args = OpenStudio::Measure::OSArgumentVector.new

    # Make argument for template
    template = OpenStudio::Measure::OSArgument.makeChoiceArgument('template', OpenstudioStandards::CreateTypical.get_templates(true), true) # setting up measure to list all templates, but space types in string should all come from one
    template.setDisplayName('Target Standard')
    template.setDefaultValue('90.1-2004')
    args << template

    # make an argument for space type ratios
    space_type_hash_string = OpenStudio::Measure::OSArgument.makeStringArgument('space_type_hash_string', true)
    space_type_hash_string.setDisplayName('Space Type Ratios String')
    space_type_hash_string.setDescription('Hash of Space Types with Building and Space Type name as Key and Fraction as value. All space types should come from the selected OpenStudio Standards template. Example entry is (BuildingType | SpaceType => 0.25)')
    space_type_hash_string.setDefaultValue('MediumOffice | Conference => 0.2, PrimarySchool | Corridor => 0.125, PrimarySchool | Classroom => 0.175, Warehouse | Office => 0.5')
    args << space_type_hash_string

    # Make argument for total_bldg_floor_area
    total_bldg_floor_area = OpenStudio::Measure::OSArgument.makeDoubleArgument('total_bldg_floor_area', true)
    total_bldg_floor_area.setDisplayName('Total Building Floor Area')
    total_bldg_floor_area.setUnits('ft^2')
    total_bldg_floor_area.setDefaultValue(10000.0)
    args << total_bldg_floor_area

    # Make argument for single_floor_area
    single_floor_area = OpenStudio::Measure::OSArgument.makeDoubleArgument('single_floor_area', true)
    single_floor_area.setDisplayName('Single Floor Area')
    single_floor_area.setDescription('Non-zero value will fix the single floor area, overriding a user entry for Total Building Floor Area')
    single_floor_area.setUnits('ft^2')
    single_floor_area.setDefaultValue(0.0)
    args << single_floor_area

    # Make argument for floor_height
    floor_height = OpenStudio::Measure::OSArgument.makeDoubleArgument('floor_height', true)
    floor_height.setDisplayName('Typical Floor to FLoor Height')
    floor_height.setDescription('Selecting a typical floor height of 0 will trigger a smart building type default.')
    floor_height.setUnits('ft')
    floor_height.setDefaultValue(0.0)
    args << floor_height

    # add argument to enable/disable multi custom space height bar
    custom_height_bar = OpenStudio::Measure::OSArgument.makeBoolArgument('custom_height_bar', true)
    custom_height_bar.setDisplayName('Enable Custom Height Bar Application')
    custom_height_bar.setDescription('This is argument value is only relevant when smart default floor to floor height is used for a building type that has spaces with custom heights.')
    custom_height_bar.setDefaultValue(true)
    args << custom_height_bar

    # Make argument for num_stories_above_grade
    num_stories_above_grade = OpenStudio::Measure::OSArgument.makeDoubleArgument('num_stories_above_grade', true)
    num_stories_above_grade.setDisplayName('Number of Stories Above Grade')
    num_stories_above_grade.setDefaultValue(1.0)
    args << num_stories_above_grade

    # Make argument for num_stories_below_grade
    num_stories_below_grade = OpenStudio::Measure::OSArgument.makeIntegerArgument('num_stories_below_grade', true)
    num_stories_below_grade.setDisplayName('Number of Stories Below Grade')
    num_stories_below_grade.setDefaultValue(0)
    args << num_stories_below_grade

    # Make argument for building_rotation
    building_rotation = OpenStudio::Measure::OSArgument.makeDoubleArgument('building_rotation', true)
    building_rotation.setDisplayName('Building Rotation')
    building_rotation.setDescription('Set Building Rotation off of North (positive value is clockwise). Rotation applied after geometry generation. Values greater than +/- 45 will result in aspect ratio and party wall orientations that do not match cardinal directions of the inputs.')
    building_rotation.setUnits('Degrees')
    building_rotation.setDefaultValue(0.0)
    args << building_rotation

    # Make argument for ns_to_ew_ratio
    ns_to_ew_ratio = OpenStudio::Measure::OSArgument.makeDoubleArgument('ns_to_ew_ratio', true)
    ns_to_ew_ratio.setDisplayName('Ratio of North/South Facade Length Relative to East/West Facade Length')
    ns_to_ew_ratio.setDescription('Selecting an aspect ratio of 0 will trigger a smart building type default. Aspect ratios less than one are not recommended for sliced bar geometry, instead rotate building and use a greater than 1 aspect ratio.')
    ns_to_ew_ratio.setDefaultValue(0.0)
    ns_to_ew_ratio.setMinValue(0.0)
    args << ns_to_ew_ratio

    # Make argument for perim_mult
    perim_mult = OpenStudio::Measure::OSArgument.makeDoubleArgument('perim_mult', true)
    perim_mult.setDisplayName('Perimeter Multiplier')
    perim_mult.setDescription('Selecting a value of 0 will trigger a smart building type default. This represents a multiplier for the building perimeter relative to the perimeter of a rectangular building that meets the area and aspect ratio inputs. Other than the smart default of 0.0 this argument should have a value of 1.0 or higher and is only applicable Multiple Space Types - Individual Stories Sliced division method.')
    perim_mult.setDefaultValue(0.0)
    args << perim_mult

    # Make argument for bar_width
    bar_width = OpenStudio::Measure::OSArgument.makeDoubleArgument('bar_width', true)
    bar_width.setDisplayName('Bar Width')
    bar_width.setDescription('Non-zero value will fix the building width, overriding user entry for Perimeter Multiplier. NS/EW Aspect Ratio may be limited based on target width.')
    bar_width.setUnits('ft')
    bar_width.setDefaultValue(0.0)
    args << bar_width

    # Make argument for bar_sep_dist_mult
    bar_sep_dist_mult = OpenStudio::Measure::OSArgument.makeDoubleArgument('bar_sep_dist_mult', true)
    bar_sep_dist_mult.setDisplayName('Bar Separation Distance Multiplier')
    bar_sep_dist_mult.setDescription('Multiplier of separation between bar elements relative to building height.')
    bar_sep_dist_mult.setDefaultValue(10.0)
    args << bar_sep_dist_mult

    # Make argument for wwr (in future add lookup for smart default)
    wwr = OpenStudio::Measure::OSArgument.makeDoubleArgument('wwr', true)
    wwr.setDisplayName('Window to Wall Ratio')
    wwr.setDescription('Selecting a window to wall ratio of 0 will trigger a smart building type default.')
    wwr.setDefaultValue(0.0)
    args << wwr

    # Make argument for party_wall_fraction
    party_wall_fraction = OpenStudio::Measure::OSArgument.makeDoubleArgument('party_wall_fraction', true)
    party_wall_fraction.setDisplayName('Fraction of Exterior Wall Area with Adjacent Structure')
    party_wall_fraction.setDescription('This will impact how many above grade exterior walls are modeled with adiabatic boundary condition.')
    party_wall_fraction.setDefaultValue(0.0)
    args << party_wall_fraction

    # party_wall_fraction was used where we wanted to represent some party walls but didn't know where they are, it ends up using methods to make whole surfaces adiabiatc by story and orientaiton to try to come close to requested fraction

    # Make argument for party_wall_stories_north
    party_wall_stories_north = OpenStudio::Measure::OSArgument.makeIntegerArgument('party_wall_stories_north', true)
    party_wall_stories_north.setDisplayName('Number of North facing stories with party wall')
    party_wall_stories_north.setDescription('This will impact how many above grade exterior north walls are modeled with adiabatic boundary condition. If this is less than the number of above grade stoes, upper flor will reamin exterior')
    party_wall_stories_north.setDefaultValue(0)
    args << party_wall_stories_north

    # Make argument for party_wall_stories_south
    party_wall_stories_south = OpenStudio::Measure::OSArgument.makeIntegerArgument('party_wall_stories_south', true)
    party_wall_stories_south.setDisplayName('Number of South facing stories with party wall')
    party_wall_stories_south.setDescription('This will impact how many above grade exterior south walls are modeled with adiabatic boundary condition. If this is less than the number of above grade stoes, upper flor will reamin exterior')
    party_wall_stories_south.setDefaultValue(0)
    args << party_wall_stories_south

    # Make argument for party_wall_stories_east
    party_wall_stories_east = OpenStudio::Measure::OSArgument.makeIntegerArgument('party_wall_stories_east', true)
    party_wall_stories_east.setDisplayName('Number of East facing stories with party wall')
    party_wall_stories_east.setDescription('This will impact how many above grade exterior east walls are modeled with adiabatic boundary condition. If this is less than the number of above grade stoes, upper flor will reamin exterior')
    party_wall_stories_east.setDefaultValue(0)
    args << party_wall_stories_east

    # Make argument for party_wall_stories_west
    party_wall_stories_west = OpenStudio::Measure::OSArgument.makeIntegerArgument('party_wall_stories_west', true)
    party_wall_stories_west.setDisplayName('Number of West facing stories with party wall')
    party_wall_stories_west.setDescription('This will impact how many above grade exterior west walls are modeled with adiabatic boundary condition. If this is less than the number of above grade stoes, upper flor will reamin exterior')
    party_wall_stories_west.setDefaultValue(0)
    args << party_wall_stories_west

    # make an argument for bottom_story_ground_exposed_floor
    bottom_story_ground_exposed_floor = OpenStudio::Measure::OSArgument.makeBoolArgument('bottom_story_ground_exposed_floor', true)
    bottom_story_ground_exposed_floor.setDisplayName('Is the Bottom Story Exposed to Ground')
    bottom_story_ground_exposed_floor.setDescription("This should be true unless you are modeling a partial building which doesn't include the lowest story. The bottom story floor will have an adiabatic boundary condition when false.")
    bottom_story_ground_exposed_floor.setDefaultValue(true)
    args << bottom_story_ground_exposed_floor

    # make an argument for top_story_exterior_exposed_roof
    top_story_exterior_exposed_roof = OpenStudio::Measure::OSArgument.makeBoolArgument('top_story_exterior_exposed_roof', true)
    top_story_exterior_exposed_roof.setDisplayName('Is the Top Story an Exterior Roof')
    top_story_exterior_exposed_roof.setDescription("This should be true unless you are modeling a partial building which doesn't include the highest story. The top story ceiling will have an adiabatic boundary condition when false.")
    top_story_exterior_exposed_roof.setDefaultValue(true)
    args << top_story_exterior_exposed_roof

    # Make argument for story_multiplier
    choices = OpenStudio::StringVector.new
    choices << 'None'
    choices << 'Basements Ground Mid Top'
    # choices << "Basements Ground Midx5 Top"
    story_multiplier = OpenStudio::Measure::OSArgument.makeChoiceArgument('story_multiplier', choices, true)
    story_multiplier.setDisplayName('Calculation Method for Story Multiplier')
    story_multiplier.setDefaultValue('Basements Ground Mid Top')
    args << story_multiplier

    # make an argument for make_mid_story_surfaces_adiabatic (added to avoid issues with intersect and to lower surface count when using individual stories sliced)
    make_mid_story_surfaces_adiabatic = OpenStudio::Measure::OSArgument.makeBoolArgument('make_mid_story_surfaces_adiabatic', true)
    make_mid_story_surfaces_adiabatic.setDisplayName('Make Mid Story Floor Surfaces Adiabatic')
    make_mid_story_surfaces_adiabatic.setDescription('If set to true, this will skip surface intersection and make mid story floors and celings adiabatic, not just at multiplied gaps.')
    make_mid_story_surfaces_adiabatic.setDefaultValue(false)
    args << make_mid_story_surfaces_adiabatic

    # make an argument for bar sub-division approach
    choices = OpenStudio::StringVector.new
    choices << 'Multiple Space Types - Simple Sliced'
    choices << 'Multiple Space Types - Individual Stories Sliced'
    choices << 'Single Space Type - Core and Perimeter' # not useful for most use cases
    # choices << "Multiple Space Types - Individual Stories Sliced Keep Building Types Together"
    # choices << "Building Type Specific Smart Division"
    bar_division_method = OpenStudio::Measure::OSArgument.makeChoiceArgument('bar_division_method', choices, true)
    bar_division_method.setDisplayName('Division Method for Bar Space Types')
    bar_division_method.setDescription('To use perimeter multiplier greater than 1 selected Multiple Space Types - Individual Stories Sliced.')
    bar_division_method.setDefaultValue('Multiple Space Types - Individual Stories Sliced')
    args << bar_division_method

    # double_loaded_corridor
    choices = OpenStudio::StringVector.new
    choices << 'None'
    choices << 'Primary Space Type'
    # choices << 'All Space Types' # possible future option
    double_loaded_corridor = OpenStudio::Measure::OSArgument.makeChoiceArgument('double_loaded_corridor', choices, true)
    double_loaded_corridor.setDisplayName('Double Loaded Corridor')
    double_loaded_corridor.setDescription('Add double loaded corridor for building types that have a defined circulation space type, to the selected space types.')
    double_loaded_corridor.setDefaultValue('Primary Space Type')
    args << double_loaded_corridor

    # Make argument for space_type_sort_logic
    choices = OpenStudio::StringVector.new
    choices << 'Size'
    choices << 'Building Type > Size'
    # choices << "Basements Ground Midx5 Top"
    space_type_sort_logic = OpenStudio::Measure::OSArgument.makeChoiceArgument('space_type_sort_logic', choices, true)
    space_type_sort_logic.setDisplayName('Choose Space Type Sorting Method')
    space_type_sort_logic.setDefaultValue('Building Type > Size')
    args << space_type_sort_logic

    # make an argument for use_upstream_args
    use_upstream_args = OpenStudio::Measure::OSArgument.makeBoolArgument('use_upstream_args', true)
    use_upstream_args.setDisplayName('Use Upstream Argument Values')
    use_upstream_args.setDescription('When true this will look for arguments or registerValues in upstream measures that match arguments from this measure, and will use the value from the upstream measure in place of what is entered for this measure.')
    use_upstream_args.setDefaultValue(true)
    args << use_upstream_args

    # TODO: - expose perimeter depth as an argument

    return args
  end

  # define what happens when the measure is run
  def run(model, runner, user_arguments)
    super(model, runner, user_arguments)

    # assign the user inputs to variables
    args = runner.getArgumentValues(arguments(model), user_arguments)
    args = Hash[args.collect{ |k, v| [k.to_sym, v] }]
    if !args then return false end

    # todo - need to make use of this before pass to standards
    use_upstream_args = args['use_upstream_args']
      
    # open channel to log messages
    reset_log

    # Turn debugging output on/off
    debug = false

    # remove_non_resource_objects (this was not moved to standards, so added method in measure for now)
    remove_non_resource_objects(runner, model)

    # method run from os_lib_model_generation.rb
    result = OpenstudioStandards::Geometry.create_bar_from_space_type_ratios(model, args)

    # gather log
    log_messages_to_runner(runner, debug)
    reset_log

    if result == false
      return false
    else
      return true
    end
  end
end
# this allows the measure to be use by the application
CreateBarFromSpaceTypeRatios.new.registerWithApplication
