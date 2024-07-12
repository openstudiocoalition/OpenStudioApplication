# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

require 'openstudio'
require 'openstudio/measure/ShowRunnerOutput'
require 'fileutils'

require_relative '../measure.rb'
require 'minitest/autorun'

class OpenStudioResults_Test < Minitest::Test
  def model_in_path_default
    return "#{File.dirname(__FILE__)}/ExampleModel.osm"
  end

  def epw_path_default
    # make sure we have a weather data location
    epw = nil
    epw = OpenStudio::Path.new("#{File.dirname(__FILE__)}/USA_CO_Golden-NREL.724666_TMY3.epw")
    assert(File.exist?(epw.to_s))
    return epw.to_s
  end

  def run_dir(test_name)
    # always generate test output in specially named 'output' directory so result files are not made part of the measure
    "#{File.dirname(__FILE__)}/output/#{test_name}"
  end

  def model_out_path(test_name)
    "#{run_dir(test_name)}/TestOutput.osm"
  end

  def workspace_path(test_name)
    return "#{run_dir(test_name)}/run/in.idf"
  end

  def sql_path(test_name)
    return "#{run_dir(test_name)}/run/eplusout.sql"
  end

  def report_path(test_name)
    "#{run_dir(test_name)}/report.html"
  end

  # method for running the test simulation using OpenStudio 2.x API
  def setup_test_2(test_name, epw_path)
    if !File.exist?(sql_path(test_name))
      osw_path = File.join(run_dir(test_name), 'in.osw')
      osw_path = File.absolute_path(osw_path)

      # create workflow to for initial test model run
      workflow = OpenStudio::WorkflowJSON.new
      workflow.setSeedFile(File.absolute_path(model_out_path(test_name)))
      workflow.setWeatherFile(File.absolute_path(epw_path))
      workflow.saveAs(osw_path)

      cli_path = OpenStudio.getOpenStudioCLI
      cmd = "\"#{cli_path}\" run -w \"#{osw_path}\""
      puts cmd
      system(cmd)
    end
  end

  # create test files if they do not exist when the test first runs
  def setup_test(test_name, idf_output_requests, model_in_path = model_in_path_default, epw_path = epw_path_default)
    if !File.exist?(run_dir(test_name))
      FileUtils.mkdir_p(run_dir(test_name))
    end
    assert(File.exist?(run_dir(test_name)))

    if File.exist?(report_path(test_name))
      FileUtils.rm(report_path(test_name))
    end

    assert(File.exist?(model_in_path))

    if File.exist?(model_out_path(test_name))
      FileUtils.rm(model_out_path(test_name))
    end

    # convert output requests to OSM for testing, OS App and PAT will add these to the E+ Idf
    workspace = OpenStudio::Workspace.new('Draft'.to_StrictnessLevel, 'EnergyPlus'.to_IddFileType)
    workspace.addObjects(idf_output_requests) # TODO: - this isn't getting added when empty OSW
    rt = OpenStudio::EnergyPlus::ReverseTranslator.new
    request_model = rt.translateWorkspace(workspace)

    translator = OpenStudio::OSVersion::VersionTranslator.new
    model = translator.loadModel(model_in_path)
    assert(!model.empty?)
    model = model.get
    model.addObjects(request_model.objects)
    model.save(model_out_path(test_name), true)

    if ENV['OPENSTUDIO_TEST_NO_CACHE_SQLFILE']
      if File.exist?(sql_path(test_name))
        FileUtils.rm_f(sql_path(test_name))
      end
    end

    setup_test_2(test_name, epw_path)
  end

  # assert that no section errors were thrown
  def section_errors(runner)
    test_string = 'section failed and was skipped because'
    test_string_2 = 'returned false and was skipped'

    section_errors = []
    runner.result.stepWarnings.each do |warning|
      if warning.include?(test_string) || warning.include?(test_string_2)
        section_errors << warning
      end
    end
    assert(section_errors.empty?)

    return section_errors
  end

  def test_example_model
    # skip "Broken in 2.5.1, address immediately"

    test_name = 'test_example_model'
    model_in_path = "#{File.dirname(__FILE__)}/ExampleModel.osm"

    # create an instance of the measure
    measure = OpenStudioResults.new

    # create an instance of a runner
    runner = OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new)

    # get arguments
    arguments = measure.arguments
    argument_map = OpenStudio::Measure.convertOSArgumentVectorToMap(arguments)

    # create hash of argument values
    args_hash = {}

    # populate argument with specified hash value if specified
    arguments.each do |arg|
      temp_arg_var = arg.clone
      if args_hash[arg.name]
        assert(temp_arg_var.setValue(args_hash[arg.name]))
      end
      argument_map[arg.name] = temp_arg_var
    end

    # get the energyplus output requests, this will be done automatically by OS App and PAT
    idf_output_requests = measure.energyPlusOutputRequests(OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new), argument_map)
    # assert_equal(11, idf_output_requests.size)

    # mimic the process of running this measure in OS App or PAT
    epw_path = epw_path_default
    setup_test(test_name, idf_output_requests, model_in_path)

    assert(File.exist?(model_out_path(test_name)))
    assert(File.exist?(sql_path(test_name)))
    assert(File.exist?(epw_path))

    # set up runner, this will happen automatically when measure is run in PAT or OpenStudio
    runner.setLastOpenStudioModelPath(OpenStudio::Path.new(model_out_path(test_name)))
    runner.setLastEnergyPlusWorkspacePath(OpenStudio::Path.new(workspace_path(test_name)))
    runner.setLastEpwFilePath(epw_path)
    runner.setLastEnergyPlusSqlFilePath(OpenStudio::Path.new(sql_path(test_name)))

    # delete the output if it exists
    if File.exist?(report_path(test_name))
      FileUtils.rm(report_path(test_name))
    end
    assert(!File.exist?(report_path(test_name)))

    # temporarily change directory to the run directory and run the measure
    start_dir = Dir.pwd
    begin
      Dir.chdir(run_dir(test_name))

      # run the measure
      measure.run(runner, argument_map)
      result = runner.result
      show_output(result)
      assert_equal('Success', result.value.valueName)

      # look for section_errors
      assert(section_errors(runner).empty?)
    ensure
      Dir.chdir(start_dir)
    end

    # make sure the report file exists
    assert(File.exist?(report_path(test_name)))
  end

  def test_example_model_si
    test_name = 'test_example_model_si'
    model_in_path = "#{File.dirname(__FILE__)}/ExampleModel.osm"

    # create an instance of the measure
    measure = OpenStudioResults.new

    # create an instance of a runner
    runner = OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new)

    # get arguments
    arguments = measure.arguments
    argument_map = OpenStudio::Measure.convertOSArgumentVectorToMap(arguments)

    # create hash of argument values
    args_hash = {}
    args_hash['units'] = 'SI'

    # populate argument with specified hash value if specified
    arguments.each do |arg|
      temp_arg_var = arg.clone
      if args_hash[arg.name]
        assert(temp_arg_var.setValue(args_hash[arg.name]))
      end
      argument_map[arg.name] = temp_arg_var
    end

    # get the energyplus output requests, this will be done automatically by OS App and PAT
    idf_output_requests = measure.energyPlusOutputRequests(OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new), argument_map)
    assert_equal(11, idf_output_requests.size)

    # mimic the process of running this measure in OS App or PAT
    epw_path = epw_path_default
    setup_test(test_name, idf_output_requests, model_in_path)

    assert(File.exist?(model_out_path(test_name)))
    assert(File.exist?(sql_path(test_name)))
    assert(File.exist?(epw_path))

    # set up runner, this will happen automatically when measure is run in PAT or OpenStudio
    runner.setLastOpenStudioModelPath(OpenStudio::Path.new(model_out_path(test_name)))
    runner.setLastEnergyPlusWorkspacePath(OpenStudio::Path.new(workspace_path(test_name)))
    runner.setLastEpwFilePath(epw_path)
    runner.setLastEnergyPlusSqlFilePath(OpenStudio::Path.new(sql_path(test_name)))

    # delete the output if it exists
    if File.exist?(report_path(test_name))
      FileUtils.rm(report_path(test_name))
    end
    assert(!File.exist?(report_path(test_name)))

    # temporarily change directory to the run directory and run the measure
    start_dir = Dir.pwd
    begin
      Dir.chdir(run_dir(test_name))

      # run the measure
      measure.run(runner, argument_map)
      result = runner.result
      show_output(result)
      assert_equal('Success', result.value.valueName)

      # look for section_errors
      assert(section_errors(runner).empty?)

      # test for proper unit conversion
      expected_unit = 'kWh'

      sqlFile = OpenStudio::SqlFile.new(OpenStudio::Path.new(sql_path(test_name)))

      ["Electricity","Natural Gas"].each do |fuel_type|
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='AnnualBuildingUtilityPerformanceSummary' and TableName='End Uses' and RowName= 'Total End Uses' and ColumnName= '#{fuel_type}'"
        results = sqlFile.execAndReturnFirstDouble(query)
        expected_value = OpenStudio.convert(results.get, 'GJ', expected_unit).get
        result_hash = result.stepValues.select{|r| r.name == "fuel_#{fuel_type.downcase.gsub(' ','_')}"}.first
        result_unit = result_hash.units.get
        result_value = result_hash.valueAsDouble
        assert_equal(expected_unit, result_unit, "#{fuel_type} result unit: #{result_unit}, expected: #{expected_unit}")
        assert_equal(expected_value, result_value, "#{fuel_type} result value: #{result_value}, expected: #{expected_value}")
      end
    ensure
      Dir.chdir(start_dir)
    end

    # make sure the report file exists
    assert(File.exist?(report_path(test_name)))
  end

  def test_edge_model
    test_name = 'test_edge_model'
    model_in_path = "#{File.dirname(__FILE__)}/EdgeCaseModel.osm"

    # create an instance of the measure
    measure = OpenStudioResults.new

    # create an instance of a runner
    runner = OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new)

    # get arguments
    arguments = measure.arguments
    argument_map = OpenStudio::Measure.convertOSArgumentVectorToMap(arguments)

    # create hash of argument values
    args_hash = {}

    # populate argument with specified hash value if specified
    arguments.each do |arg|
      temp_arg_var = arg.clone
      if args_hash[arg.name]
        assert(temp_arg_var.setValue(args_hash[arg.name]))
      end
      argument_map[arg.name] = temp_arg_var
    end

    # get the energyplus output requests, this will be done automatically by OS App and PAT
    idf_output_requests = measure.energyPlusOutputRequests(OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new), argument_map)
    # assert_equal(5, idf_output_requests.size)

    # mimic the process of running this measure in OS App or PAT
    epw_path = epw_path_default
    setup_test(test_name, idf_output_requests, model_in_path)

    assert(File.exist?(model_out_path(test_name)))
    assert(File.exist?(sql_path(test_name)))
    assert(File.exist?(epw_path))

    # set up runner, this will happen automatically when measure is run in PAT or OpenStudio
    runner.setLastOpenStudioModelPath(OpenStudio::Path.new(model_out_path(test_name)))
    runner.setLastEnergyPlusWorkspacePath(OpenStudio::Path.new(workspace_path(test_name)))
    runner.setLastEpwFilePath(epw_path)
    runner.setLastEnergyPlusSqlFilePath(OpenStudio::Path.new(sql_path(test_name)))

    # delete the output if it exists
    if File.exist?(report_path(test_name))
      FileUtils.rm(report_path(test_name))
    end
    assert(!File.exist?(report_path(test_name)))

    # temporarily change directory to the run directory and run the measure
    start_dir = Dir.pwd
    begin
      Dir.chdir(run_dir(test_name))

      # run the measure
      measure.run(runner, argument_map)
      result = runner.result
      show_output(result)
      assert_equal('Success', result.value.valueName)

      # look for section_errors
      assert(section_errors(runner).empty?)

      # assert that additional fuel annual values were found
      # this is annual value pulled from pie chart
      # query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='AnnualBuildingUtilityPerformanceSummary' and TableName='End Uses' and RowName= 'Total End Uses' and ColumnName= 'Additional Fuel'"
      # results = sqlFile.execAndReturnFirstDouble(query)
      result.stepValues.each do |value|
        if value.name == 'fuel_additional_fuel'
          assert(value.valueAsDouble > 0)
        end
      end
      # assert that additional fuel monthly values were found
      # this is some of monthly values from from call below
      # sqlFile.energyConsumptionByMonth(OpenStudio::EndUseFuelType.new("Additional Fuel"), OpenStudio::EndUseCategoryType.new(0), OpenStudio::MonthOfYear.new(1)).get
      # now using a manual sqlFile.execAndReturnFirstDouble with specific additional fuel type names, but doesn't currently work on test with 2.7.1 because Output:Table:Monthy objects are lost of reverse translation of test setup.
      result.stepValues.each do |value|
        if value.name == 'fuel_oil_1_ip' # individual month would be fuel_oil_1_ip_jan
          # todo - enable this once test setup handles Output:Table:Monthly output requests properly.
          # assert(value.valueAsDouble > 0)
        end
      end
    ensure
      Dir.chdir(start_dir)
    end

    # make sure the report file exists
    assert(File.exist?(report_path(test_name)))
  end

  def test_empty_model
    # skip "Broken in 2.5.1, address immediately"

    test_name = 'test_empty_model'
    model_in_path = "#{File.dirname(__FILE__)}/EmptyModel.osm"

    # create an instance of the measure
    measure = OpenStudioResults.new

    # create an instance of a runner
    runner = OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new)

    # get arguments
    arguments = measure.arguments
    argument_map = OpenStudio::Measure.convertOSArgumentVectorToMap(arguments)

    # create hash of argument values
    args_hash = {}

    # populate argument with specified hash value if specified
    arguments.each do |arg|
      temp_arg_var = arg.clone
      if args_hash[arg.name]
        assert(temp_arg_var.setValue(args_hash[arg.name]))
      end
      argument_map[arg.name] = temp_arg_var
    end

    # get the energyplus output requests, this will be done automatically by OS App and PAT
    idf_output_requests = measure.energyPlusOutputRequests(OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new), argument_map)
    assert_equal(11, idf_output_requests.size)

    # mimic the process of running this measure in OS App or PAT
    epw_path = epw_path_default
    setup_test(test_name, idf_output_requests, model_in_path)

    assert(File.exist?(model_out_path(test_name)))
    assert(File.exist?(sql_path(test_name)))
    assert(File.exist?(epw_path))

    # set up runner, this will happen automatically when measure is run in PAT or OpenStudio
    runner.setLastOpenStudioModelPath(OpenStudio::Path.new(model_out_path(test_name)))
    runner.setLastEnergyPlusWorkspacePath(OpenStudio::Path.new(workspace_path(test_name)))
    runner.setLastEpwFilePath(epw_path)
    runner.setLastEnergyPlusSqlFilePath(OpenStudio::Path.new(sql_path(test_name)))

    # delete the output if it exists
    if File.exist?(report_path(test_name))
      FileUtils.rm(report_path(test_name))
    end
    assert(!File.exist?(report_path(test_name)))

    # temporarily change directory to the run directory and run the measure
    start_dir = Dir.pwd
    begin
      Dir.chdir(run_dir(test_name))

      # run the measure
      measure.run(runner, argument_map)
      result = runner.result
      show_output(result)
      assert_equal('Success', result.value.valueName)

      # look for section_errors
      assert(section_errors(runner).empty?)
    ensure
      Dir.chdir(start_dir)
    end

    # make sure the report file exists
    assert(File.exist?(report_path(test_name)))
  end

  def test_no_run_period
    test_name = 'no_run_period'
    model_in_path = "#{File.dirname(__FILE__)}/NoRunPeriod.osm"

    # create an instance of the measure
    measure = OpenStudioResults.new

    # create an instance of a runner
    runner = OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new)

    # get arguments
    arguments = measure.arguments
    argument_map = OpenStudio::Measure.convertOSArgumentVectorToMap(arguments)

    # create hash of argument values
    args_hash = {}

    # populate argument with specified hash value if specified
    arguments.each do |arg|
      temp_arg_var = arg.clone
      if args_hash[arg.name]
        assert(temp_arg_var.setValue(args_hash[arg.name]))
      end
      argument_map[arg.name] = temp_arg_var
    end

    # get the energyplus output requests, this will be done automatically by OS App and PAT
    idf_output_requests = measure.energyPlusOutputRequests(OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new), argument_map)
    assert_equal(11, idf_output_requests.size)

    # mimic the process of running this measure in OS App or PAT
    epw_path = epw_path_default
    setup_test(test_name, idf_output_requests, model_in_path)

    assert(File.exist?(model_out_path(test_name)))
    assert(File.exist?(sql_path(test_name)))
    # assert(File.exist?(''))

    # set up runner, this will happen automatically when measure is run in PAT or OpenStudio
    runner.setLastOpenStudioModelPath(OpenStudio::Path.new(model_out_path(test_name)))
    runner.setLastEnergyPlusWorkspacePath(OpenStudio::Path.new(workspace_path(test_name)))
    runner.setLastEpwFilePath('')
    runner.setLastEnergyPlusSqlFilePath(OpenStudio::Path.new(sql_path(test_name)))

    # delete the output if it exists
    if File.exist?(report_path(test_name))
      FileUtils.rm(report_path(test_name))
    end
    assert(!File.exist?(report_path(test_name)))

    # temporarily change directory to the run directory and run the measure
    start_dir = Dir.pwd
    begin
      Dir.chdir(run_dir(test_name))

      # run the measure
      measure.run(runner, argument_map)
      result = runner.result
      show_output(result)
      assert_equal('Fail', result.value.valueName)

      # look for section_errors
      assert(section_errors(runner).empty?)
    ensure
      Dir.chdir(start_dir)
    end

    # make sure the report file exists
    # assert(File.exist?(report_path(test_name)))
  end

  def test_sm_hotel
    # skip "Broken in 2.5.1, address immediately"

    test_name = 'sm_hotel'
    model_in_path = "#{File.dirname(__FILE__)}/1004_SmallHotel_a.osm"

    # create an instance of the measure
    measure = OpenStudioResults.new

    # create an instance of a runner
    runner = OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new)

    # get arguments
    arguments = measure.arguments
    argument_map = OpenStudio::Measure.convertOSArgumentVectorToMap(arguments)

    # create hash of argument values
    args_hash = {}

    # populate argument with specified hash value if specified
    arguments.each do |arg|
      temp_arg_var = arg.clone
      if args_hash[arg.name]
        assert(temp_arg_var.setValue(args_hash[arg.name]))
      end
      argument_map[arg.name] = temp_arg_var
    end

    # get the energyplus output requests, this will be done automatically by OS App and PAT
    idf_output_requests = measure.energyPlusOutputRequests(OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new), argument_map)
    assert_equal(11, idf_output_requests.size)

    # mimic the process of running this measure in OS App or PAT
    epw_path = epw_path_default
    setup_test(test_name, idf_output_requests, model_in_path)

    assert(File.exist?(model_out_path(test_name)))
    assert(File.exist?(sql_path(test_name)))
    # assert(File.exist?(''))

    # set up runner, this will happen automatically when measure is run in PAT or OpenStudio
    runner.setLastOpenStudioModelPath(OpenStudio::Path.new(model_out_path(test_name)))
    runner.setLastEnergyPlusWorkspacePath(OpenStudio::Path.new(workspace_path(test_name)))
    runner.setLastEpwFilePath('')
    runner.setLastEnergyPlusSqlFilePath(OpenStudio::Path.new(sql_path(test_name)))

    # delete the output if it exists
    if File.exist?(report_path(test_name))
      FileUtils.rm(report_path(test_name))
    end
    assert(!File.exist?(report_path(test_name)))

    # temporarily change directory to the run directory and run the measure
    start_dir = Dir.pwd
    begin
      Dir.chdir(run_dir(test_name))

      # run the measure
      measure.run(runner, argument_map)
      result = runner.result
      show_output(result)
      assert_equal('Success', result.value.valueName)

      # look for section_errors
      assert(section_errors(runner).empty?)
    ensure
      Dir.chdir(start_dir)
    end

    # make sure the report file exists
    assert(File.exist?(report_path(test_name)))
  end

  def test_period_in_const_name
    # skip "Broken in 2.5.1, address immediately"

    test_name = 'period_in_const_name'
    model_in_path = "#{File.dirname(__FILE__)}/PeriodInConstName.osm"

    # create an instance of the measure
    measure = OpenStudioResults.new

    # create an instance of a runner
    runner = OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new)

    # get arguments
    arguments = measure.arguments
    argument_map = OpenStudio::Measure.convertOSArgumentVectorToMap(arguments)

    # create hash of argument values
    args_hash = {}

    # populate argument with specified hash value if specified
    arguments.each do |arg|
      temp_arg_var = arg.clone
      if args_hash[arg.name]
        assert(temp_arg_var.setValue(args_hash[arg.name]))
      end
      argument_map[arg.name] = temp_arg_var
    end

    # get the energyplus output requests, this will be done automatically by OS App and PAT
    idf_output_requests = measure.energyPlusOutputRequests(OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new), argument_map)
    assert_equal(11, idf_output_requests.size)

    # mimic the process of running this measure in OS App or PAT
    epw_path = epw_path_default
    setup_test(test_name, idf_output_requests, model_in_path)

    assert(File.exist?(model_out_path(test_name)))
    assert(File.exist?(sql_path(test_name)))
    # assert(File.exist?(''))

    # set up runner, this will happen automatically when measure is run in PAT or OpenStudio
    runner.setLastOpenStudioModelPath(OpenStudio::Path.new(model_out_path(test_name)))
    runner.setLastEnergyPlusWorkspacePath(OpenStudio::Path.new(workspace_path(test_name)))
    runner.setLastEpwFilePath('')
    runner.setLastEnergyPlusSqlFilePath(OpenStudio::Path.new(sql_path(test_name)))

    # delete the output if it exists
    if File.exist?(report_path(test_name))
      FileUtils.rm(report_path(test_name))
    end
    assert(!File.exist?(report_path(test_name)))

    # temporarily change directory to the run directory and run the measure
    start_dir = Dir.pwd
    begin
      Dir.chdir(run_dir(test_name))

      # run the measure
      measure.run(runner, argument_map)
      result = runner.result
      show_output(result)
      assert_equal('Success', result.value.valueName)

      # look for section_errors
      assert(section_errors(runner).empty?)
    ensure
      Dir.chdir(start_dir)
    end

    # make sure the report file exists
    assert(File.exist?(report_path(test_name)))
  end

  def test_heating_only
    # skip "Broken in 2.5.1, address immediately"

    test_name = 'test_heating_only'
    model_in_path = "#{File.dirname(__FILE__)}/HeatingOnly.osm"

    # create an instance of the measure
    measure = OpenStudioResults.new

    # create an instance of a runner
    runner = OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new)

    # get arguments
    arguments = measure.arguments
    argument_map = OpenStudio::Measure.convertOSArgumentVectorToMap(arguments)

    # create hash of argument values
    args_hash = {}

    # populate argument with specified hash value if specified
    arguments.each do |arg|
      temp_arg_var = arg.clone
      if args_hash[arg.name]
        assert(temp_arg_var.setValue(args_hash[arg.name]))
      end
      argument_map[arg.name] = temp_arg_var
    end

    # get the energyplus output requests, this will be done automatically by OS App and PAT
    idf_output_requests = measure.energyPlusOutputRequests(OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new), argument_map)
    assert_equal(11, idf_output_requests.size)

    # mimic the process of running this measure in OS App or PAT
    epw_path = epw_path_default
    setup_test(test_name, idf_output_requests, model_in_path)

    assert(File.exist?(model_out_path(test_name)))
    assert(File.exist?(sql_path(test_name)))
    assert(File.exist?(epw_path))

    # set up runner, this will happen automatically when measure is run in PAT or OpenStudio
    runner.setLastOpenStudioModelPath(OpenStudio::Path.new(model_out_path(test_name)))
    runner.setLastEnergyPlusWorkspacePath(OpenStudio::Path.new(workspace_path(test_name)))
    runner.setLastEpwFilePath(epw_path)
    runner.setLastEnergyPlusSqlFilePath(OpenStudio::Path.new(sql_path(test_name)))

    # delete the output if it exists
    if File.exist?(report_path(test_name))
      FileUtils.rm(report_path(test_name))
    end
    assert(!File.exist?(report_path(test_name)))

    # temporarily change directory to the run directory and run the measure
    start_dir = Dir.pwd
    begin
      Dir.chdir(run_dir(test_name))

      # run the measure
      measure.run(runner, argument_map)
      result = runner.result
      show_output(result)
      assert_equal('Success', result.value.valueName)

      # look for section_errors
      assert(section_errors(runner).empty?)
    ensure
      Dir.chdir(start_dir)
    end

    # make sure the report file exists
    assert(File.exist?(report_path(test_name)))
  end

  def test_tariff
    # skip "Broken in 2.5.1, address immediately"

    test_name = 'tariff'
    model_in_path = "#{File.dirname(__FILE__)}/ExampleModel.osm"

    # create an instance of the measure
    measure = OpenStudioResults.new

    # create an instance of a runner
    # runner = OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new)

    # create an instance of a runner with OSW (pre-populated with result data)
    osw_path = OpenStudio::Path.new(File.dirname(__FILE__) + '/add_tariff.osw')
    osw = OpenStudio::WorkflowJSON.load(osw_path).get
    runner = OpenStudio::Measure::OSRunner.new(osw)

    # get arguments
    arguments = measure.arguments
    argument_map = OpenStudio::Measure.convertOSArgumentVectorToMap(arguments)

    # create hash of argument values
    args_hash = {}
    args_hash['reg_monthly_details'] = true # the OSW used for test should match this, as that is what is checked in the measure

    # populate argument with specified hash value if specified
    arguments.each do |arg|
      temp_arg_var = arg.clone
      if args_hash[arg.name]
        assert(temp_arg_var.setValue(args_hash[arg.name]))
      end
      argument_map[arg.name] = temp_arg_var
    end

    # mimic the process of running this measure in OS App or PAT
    epw_path = epw_path_default

    # temporarily change directory to the run directory and run the measure
    start_dir = Dir.pwd
    begin
      if !File.exist?(run_dir(test_name))
        FileUtils.mkdir_p(run_dir(test_name))
      end
      assert(File.exist?(run_dir(test_name)))
      Dir.chdir(run_dir(test_name))

      # copy osw, osm, and epw to test directory and run from there.
      source_osw = '../../add_tariff.osw'
      target_osw = 'in.osw'
      FileUtils.cp(source_osw, target_osw)
      target_osm = 'TestOutput.osm'

      translator = OpenStudio::OSVersion::VersionTranslator.new
      model = translator.loadModel(model_in_path)
      assert(!model.empty?)
      model = model.get
      model.save(target_osm, true)

      # create osw that calls tariff measure. That measure needs to sit adjacent ot this one
      cli_path = OpenStudio.getOpenStudioCLI
      cmd = "\"#{cli_path}\" run -w \"in.osw\""
      system(cmd)
    ensure
      Dir.chdir(start_dir)
    end

    assert(File.exist?(model_out_path(test_name)))
    assert(File.exist?(sql_path(test_name)))
    assert(File.exist?(epw_path))

    # set up runner, this will happen automatically when measure is run in PAT or OpenStudio
    runner.setLastOpenStudioModelPath(OpenStudio::Path.new(model_out_path(test_name)))
    runner.setLastEnergyPlusWorkspacePath(OpenStudio::Path.new(workspace_path(test_name)))
    runner.setLastEpwFilePath(epw_path)
    runner.setLastEnergyPlusSqlFilePath(OpenStudio::Path.new(sql_path(test_name)))

    # delete the output if it exists
    if File.exist?(report_path(test_name))
      FileUtils.rm(report_path(test_name))
    end
    assert(!File.exist?(report_path(test_name)))

    # temporarily change directory to the run directory and run the measure
    start_dir = Dir.pwd
    begin
      Dir.chdir(run_dir(test_name))

      # run the measure
      measure.run(runner, argument_map)
      result = runner.result
      show_output(result)
      assert_equal('Success', result.value.valueName)

      # look for section_errors
      assert(section_errors(runner).empty?)
    ensure
      Dir.chdir(start_dir)
    end

    # make sure the report file exists
    assert(File.exist?(report_path(test_name)))
  end
end
