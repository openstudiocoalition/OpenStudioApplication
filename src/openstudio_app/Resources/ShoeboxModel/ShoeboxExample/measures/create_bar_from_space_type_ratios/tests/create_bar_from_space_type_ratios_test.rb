# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

require 'openstudio'
require 'openstudio/measure/ShowRunnerOutput'
require 'minitest/autorun'
require_relative '../measure'
require 'fileutils'

class CreateBarFromSpaceTypeRatios_Test < Minitest::Test
  # method to apply arguments, run measure, and assert results (only populate args hash with non-default argument values)
  def apply_measure_to_model(test_name, args, model_name = nil, result_value = 'Success', warnings_count = 0, info_count = nil)
    # create an instance of the measure
    measure = CreateBarFromSpaceTypeRatios.new

    # create an instance of a runner
    runner = OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new)

    if model_name.nil?
      # make an empty model
      model = OpenStudio::Model::Model.new
    else
      # load the test model
      translator = OpenStudio::OSVersion::VersionTranslator.new
      path = OpenStudio::Path.new("#{File.dirname(__FILE__)}/#{model_name}")
      model = translator.loadModel(path)
      assert(!model.empty?)
      model = model.get
    end

    # get arguments
    arguments = measure.arguments(model)
    argument_map = OpenStudio::Measure.convertOSArgumentVectorToMap(arguments)

    # populate argument with specified hash value if specified
    arguments.each do |arg|
      temp_arg_var = arg.clone
      if args.key?(arg.name)
        assert(temp_arg_var.setValue(args[arg.name]), "could not set #{arg.name} to #{args[arg.name]}.")
      end
      argument_map[arg.name] = temp_arg_var
    end

    # run the measure
    measure.run(model, runner, argument_map)
    result = runner.result

    # show the output
    puts "measure results for #{test_name}"
    show_output(result)

    # save the model to test output directory
    output_file_path = OpenStudio::Path.new(File.dirname(__FILE__) + "/output/#{test_name}_test_output.osm")
    model.save(output_file_path, true)

    # assert that it ran correctly
    if result_value.nil? then result_value = 'Success' end
    assert_equal(result_value, result.value.valueName)

    # check count of warning and info messages
    unless info_count.nil? then assert(result.info.size == info_count) end
    unless warnings_count.nil? then assert(result.warnings.size == warnings_count, "warning count (#{result.warnings.size}) did not match expectation (#{warnings_count})") end

    # if 'Fail' passed in make sure at least one error message (while not typical there may be more than one message)
    if result_value == 'Fail' then assert(result.errors.size >= 1) end
  end

  def test_good_argument_values
    args = {}
    args['total_bldg_floor_area'] = 10000.0
    # args['space_type_hash_string'] = "MediumOffice | Conference => 0.2, PrimarySchool | Corridor => 0.125, PrimarySchool | Classroom => 0.175, Warehouse | Office => 0.5"

    apply_measure_to_model(__method__.to_s.gsub('test_', ''), args, nil, nil, nil)
  end

  def test_alt_string_spaces
    args = {}
    args['total_bldg_floor_area'] = 10000.0
    args['space_type_hash_string'] = 'MediumOffice|Conference=>0.2, PrimarySchool|Corridor=>0.125, PrimarySchool|Classroom=>0.175, Warehouse|Office=>0.5'

    apply_measure_to_model(__method__.to_s.gsub('test_', ''), args, nil, nil, nil)
  end

  def test_high_fraction
    skip # does not pass yet need to address this in ext gem
    args = {}
    args['total_bldg_floor_area'] = 10000.0
    args['space_type_hash_string'] = 'MediumOffice|Conference=>0.2, PrimarySchool|Corridor=>0.125, PrimarySchool|Classroom=>0.175, Warehouse|Office=>0.8'

    apply_measure_to_model(__method__.to_s.gsub('test_', ''), args, nil, nil, nil)
  end

  def test_low_fraction
    skip # does not pass yet need to address this in ext gem
    args = {}
    args['total_bldg_floor_area'] = 10000.0
    args['space_type_hash_string'] = 'MediumOffice|Conference=>0.2, PrimarySchool|Corridor=>0.125, PrimarySchool|Classroom=>0.175, Warehouse|Office=>0.4'

    apply_measure_to_model(__method__.to_s.gsub('test_', ''), args, nil, nil, nil)
  end

  # test  double_loaded_corridor true and passes in 2 instead of 1 corridor space type and or 2 instead of 1 primary space type
  def test_mult_primary_and_corridor
    skip # does not pass yet need to address this in ext gem
    args = {}
    args['total_bldg_floor_area'] = 10000.0
    args['double_loaded_corridor'] = 'Primary Space Type'
    args['space_type_hash_string'] = 'PrimarySchool|Corridor=>0.1, PrimarySchool|Classroom=>0.15, SmallHotel|Corridor=>0.2, SmallHotel|GuestRoom =>0.25, Warehouse|Office=>0.4'

    apply_measure_to_model(__method__.to_s.gsub('test_', ''), args, nil, nil, nil)
  end

  def test_madeup_space_type
    skip # does not pass yet need to address this in ext gem. Custom space type passes but not building type
    args = {}
    args['total_bldg_floor_area'] = 10000.0
    args['space_type_hash_string'] = 'CustomBuildingType |CustomSpaceType =>0.2, PrimarySchool|Corridor=>0.125, PrimarySchool|Classroom=>0.175, Warehouse|Office=>0.5'

    apply_measure_to_model(__method__.to_s.gsub('test_', ''), args, nil, nil, nil)
  end

  def test_custom_a
    args = {}
    args['space_type_hash_string'] = 'SmallHotel | Exercise => 0.03960510364059971, Warehouse | Bulk => 0.5921979411807214, LargeOffice | Restroom => 0.021789247463482743, LargeOffice | OpenOffice => 0.19976483978, SecondarySchool | Library => 0.0023030009054764247, LargeOffice | Stair => 0.07560259606968275, LargeOffice | BreakRoom => 0.029289089032903216, LargeOffice | Conference => 0.037007636274519605, LargeOffice | Elec/MechRoom => 0.002440545649210396'
    args['total_bldg_floor_area'] = 69642.0
    args['floor_height'] = 20
    args['num_stories_above_grade'] = 2
    args['template'] = 'DOE Ref Pre-1980'
    args['bar_division_method'] = 'Multiple Space Types - Individual Stories Sliced'
    args['ns_to_ew_ratio'] = 1.54
    args['perim_mult'] = 1
    args['custom_height_bar'] = true
    args['double_loaded_corridor'] = 'Primary Space Type'
    args['building_rotation'] = 0
    args['make_mid_story_surfaces_adiabatic'] = true
    args['bar_sep_dist_mult'] = 3
    args['story_multiplier'] = 'None'
    args['space_type_sort_logic'] = 'Building Type > Size'
    args['story_multiplier'] = 'None'

    apply_measure_to_model(__method__.to_s.gsub('test_', ''), args, nil, nil, nil)
  end

  # TODO: - add in test with invalid string and see that it is handled well. (it is not handled well, will need to fix in ext gem and add test there)
end
