# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

require 'openstudio'
require 'openstudio/measure/ShowRunnerOutput'
require 'minitest/autorun'
require_relative '../measure'
require 'fileutils'

class AddHVACSystemTest < MiniTest::Test
  def test_add_hvac_systems
    # Get the original working directory
    start_dir = Dir.pwd

    # Make the output directory if it doesn't exist
    output_dir = File.expand_path('output', File.dirname(__FILE__))
    FileUtils.mkdir output_dir unless Dir.exist? output_dir

    # List all the HVAC system types to test
    # [system_type, allowable_htg_unmet_hrs, allowable_clg_unmet_hrs]
    hvac_systems = [
      ['PTAC with hot water heat', 600, 600],
      ['PTAC with gas coil heat', 600, 600],
      ['PTAC with electric baseboard heat', 600, 600],
      ['PTAC with no heat', 8760, 600],
      ['PTAC with district hot water heat', 600, 600],
      ['PTHP', 600, 600],
      ['PSZ-AC with gas coil heat', 600, 600],
      ['PSZ-AC with electric baseboard heat', 600, 600],
      ['PSZ-AC with no heat', 8760, 600],
      ['PSZ-AC with district hot water heat', 600, 600],
      ['PSZ-HP', 600, 600],
      ['Fan coil district chilled water with no heat', 8760, 600],
      ['Fan coil district chilled water and boiler', 600, 600],
      ['Fan coil district chilled water unit heaters', 600, 600],
      ['Fan coil district chilled water electric baseboard heat', 600, 600],
      ['Fan coil district hot and chilled water', 600, 600],
      ['Fan coil district hot water and chiller', 600, 600],
      ['Fan coil chiller with no heat', 8760, 600],
      ['Baseboard district hot water heat', 600, 8760],
      ['Baseboard district hot water heat with direct evap coolers', 600, 600],
      ['Baseboard electric heat', 600, 8760],
      ['Baseboard electric heat with direct evap coolers', 600, 600],
      ['Baseboard hot water heat', 600, 8760],
      ['Baseboard hot water heat with direct evap coolers', 600, 600],
      ['Window AC with no heat', 8760, 600],
      ['Window AC with forced air furnace', 600, 600],
      ['Window AC with district hot water baseboard heat', 600, 600],
      ['Window AC with hot water baseboard heat', 600, 600],
      ['Window AC with electric baseboard heat', 600, 600],
      ['Window AC with unit heaters', 600, 600],
      ['Direct evap coolers', 8760, 600],
      ['Direct evap coolers with unit heaters', 600, 600],
      ['Unit heaters', 600, 8760],
      ['Heat pump heat with no cooling', 600, 8760],
      ['Heat pump heat with direct evap cooler', 600, 600],
      ['VAV with reheat', 600, 600],
      ['VAV with PFP boxes', 600, 600],
      ['VAV with gas reheat', 600, 600],
      ['VAV with zone unit heaters', 600, 600],
      ['VAV with electric baseboard heat', 600, 600],
      ['VAV cool with zone heat pump heat', 600, 600], # Double airloop failure
      ['PVAV with reheat', 600, 600],
      ['PVAV with PFP boxes', 600, 600],
      ['Residential forced air', 600, 8760],
      ['Residential forced air cooling hot water baseboard heat', 600, 600],
      ['Residential forced air with district hot water', 600, 600],
      ['Residential heat pump', 600, 600],
      ['Forced air furnace', 600, 8760],
      ['Forced air furnace district chilled water fan coil', 600, 600],
      ['Forced air furnace direct evap cooler', 600, 600], # Double airloop failure
      ['Residential AC with no heat', 8760, 600],
      ['Residential AC with electric baseboard heat', 600, 600]
    ]

    template = '90.1-2013'
    building_type = 'SmallOffice'
    climate_zone = 'ASHRAE 169-2013-2A'

    # Add each HVAC system to the test model
    # and run a sizing run to ensure it simulates.
    errs = []
    hvac_systems.each do |system_type, allowable_htg_unmet_hrs, allowable_clg_unmet_hrs|
      reset_log

      model_dir = "#{output_dir}/#{system_type.delete('/')}"
      FileUtils.mkdir model_dir unless Dir.exist? model_dir
      Dir.chdir(model_dir)

      # Load the model if already created
      if File.exist?("#{model_dir}/final.osm")

        puts "Already ran #{system_type}"
        model = OpenStudio::Model::Model.new
        sql = safe_load_sql("#{model_dir}/AR/run/eplusout.sql")
        model.setSqlFile(sql)

      # If not created, make and run annual simulation
      else
        puts "Running #{system_type}"

        # Load the test model
        model = safe_load_model("#{File.dirname(__FILE__)}/SmallOffice.osm")

        # Assign a weather file
        model.add_design_days_and_weather_file(building_type, template, climate_zone, '')

        # Modify the unmet hours tolerance to a reasonable value
        # of 1F
        unmet_hrs_tol_r = 1
        unmet_hrs_tol_k = OpenStudio.convert(unmet_hrs_tol_r, 'R', 'K').get
        tol = model.getOutputControlReportingTolerances
        tol.setToleranceforTimeHeatingSetpointNotMet(unmet_hrs_tol_k)
        tol.setToleranceforTimeCoolingSetpointNotMet(unmet_hrs_tol_k)

        # create an instance of the measure
        measure = CreateTypicalBuildingFromModel.new

        # create an instance of a runner
        runner = OpenStudio::Measure::OSRunner.new(OpenStudio::WorkflowJSON.new)

        # get arguments
        arguments = measure.arguments(model)
        argument_map = OpenStudio::Measure.convertOSArgumentVectorToMap(arguments)

        # create hash of argument values.
        args_hash = {}
        args_hash['system_type'] = system_type
        args_hash['building_type'] = building_type
        args_hash['template'] = template
        args_hash['climate_zone'] = climate_zone
        args_hash['add_elevators'] = false
        args_hash['add_internal_mass'] = false
        args_hash['add_exhaust'] = false
        args_hash['add_exterior_lights'] = false
        args_hash['add_swh'] = false

        # populate argument with specified hash value if specified
        arguments.each do |arg|
          temp_arg_var = arg.clone
          if args_hash[arg.name]
            assert(temp_arg_var.setValue(args_hash[arg.name]), "Could not set #{arg.name} to #{args_hash[arg.name]}")
          end
          argument_map[arg.name] = temp_arg_var
        end

        # run the measure
        measure.run(model, runner, argument_map)
        result = runner.result

        # show the output
        show_output(result)

        # assert that it ran correctly
        # errs << "Failed on #{system_type}" unless result.value.valueName == "Success"

        # Save the model
        model.save("#{model_dir}/final.osm", true)

        # Run the annual simulation
        annual_run_success = model.run_simulation_and_log_errors("#{model_dir}/AR")

        # Log the errors
        log_messages_to_file("#{model_dir}/openstudio-standards.log", debug = false)

        # Check that the annual simulation succeeded
        errs << "For #{system_type} annual run failed" unless annual_run_success

      end

      # Check the conditioned floor area
      errs << "For #{system_type} there was no conditioned area." if model.net_conditioned_floor_area == 0

      # Check the unmet heating hours
      unmet_htg_hrs = model.annual_occupied_unmet_heating_hours
      if unmet_htg_hrs
        errs << "For #{system_type} there were #{unmet_htg_hrs} unmet occupied heating hours, more than the limit of #{allowable_htg_unmet_hrs}." if unmet_htg_hrs > allowable_htg_unmet_hrs
      else
        errs << "For #{system_type} could not determine unmet heating hours; simulation may have failed."
      end

      # Check the unmet cooling hours
      unmet_clg_hrs = model.annual_occupied_unmet_cooling_hours
      if unmet_clg_hrs
        errs << "For #{system_type} there were #{unmet_clg_hrs} unmet occupied cooling hours, more than the limit of #{allowable_clg_unmet_hrs}." if unmet_clg_hrs > allowable_clg_unmet_hrs
      else
        errs << "For #{system_type} could not determine unmet cooling hours; simulation may have failed."
      end
    end

    Dir.chdir(start_dir)

    # Expected error "Cannot find current Workflow Step"
    assert(errs.size < 2, errs.join("\n"))

    return true
  end
end
