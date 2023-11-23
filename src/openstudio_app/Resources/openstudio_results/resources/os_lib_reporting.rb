# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

require 'json'
require 'openstudio-standards'

module OsLib_Reporting
  # setup - get model, sql, and setup web assets path
  def self.setup(runner)
    results = {}

    # get the last model
    model = runner.lastOpenStudioModel
    if model.empty?
      runner.registerError('Cannot find last model.')
      return false
    end
    model = model.get

    # get the last idf
    workspace = runner.lastEnergyPlusWorkspace
    if workspace.empty?
      runner.registerError('Cannot find last idf file.')
      return false
    end
    workspace = workspace.get

    # get the last sql file
    sqlFile = runner.lastEnergyPlusSqlFile
    if sqlFile.empty?
      runner.registerError('Cannot find last sql file.')
      return false
    end
    sqlFile = sqlFile.get
    model.setSqlFile(sqlFile)

    # populate hash to pass to measure
    results[:model] = model
    # results[:workspace] = workspace
    results[:sqlFile] = sqlFile

    return results
  end

  def self.ann_env_pd(sqlFile)
    # get the weather file run period (as opposed to design day run period)
    ann_env_pd = nil
    sqlFile.availableEnvPeriods.each do |env_pd|
      env_type = sqlFile.environmentType(env_pd)
      if env_type.is_initialized
        if env_type.get == OpenStudio::EnvironmentType.new('WeatherRunPeriod')
          ann_env_pd = env_pd
        end
      end
    end

    return ann_env_pd
  end

  def self.create_xls
    require 'rubyXL'
    book = ::RubyXL::Workbook.new

    # delete initial worksheet

    return book
  end

  def self.save_xls(book)
    file = book.write 'excel-file.xlsx'

    return file
  end

  # write an Excel file from table data
  # the Excel Functions are not currently being used, left in as example
  # Requires ruby Gem which isn't currently supported in OpenStudio GUIs.
  # Current setup is simple, creates new workbook for each table
  # Could be updated to have one section per workbook
  def self.write_xls(table_data, book)
    worksheet = book.add_worksheet table_data[:title]

    row_cnt = 0
    # write the header row
    header = table_data[:header]
    header.each_with_index do |h, i|
      worksheet.add_cell(row_cnt, i, h)
    end
    worksheet.change_row_fill(row_cnt, '0ba53d')

    # loop over data rows
    data = table_data[:data]
    data.each do |d|
      row_cnt += 1
      d.each_with_index do |c, i|
        worksheet.add_cell(row_cnt, i, c)
      end
    end

    return book
  end

  # cleanup - prep html and close sql
  def self.cleanup(html_in_path)
    # TODO: - would like to move code here, but couldn't get it working. May look at it again later on.

    return html_out_path
  end

  # clean up unkown strings used for runner.registerValue names
  def self.reg_val_string_prep(string)
    # replace non alpha-numberic characters with an underscore
    string = string.gsub(/[^0-9a-z]/i, '_')

    # snake case string
    string = OpenStudio.toUnderscoreCase(string)

    return string
  end

  # hard code fuel types (for E+ 9.4 shouldn't have it twice, should eventually come form OS)
  def self.fuel_type_names(extended = false)
    # get short or extended list (not using now)
    fuel_types = []
    OpenStudio::EndUseFuelType.getValues.each do |fuel_type|
      # convert integer to string
      fuel_name = OpenStudio::EndUseFuelType.new(fuel_type).valueDescription
      next if fuel_name == 'Water'
      fuel_types << fuel_name
    end
    return fuel_types
  end

  # create template section
  def self.template_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    template_tables = []

    # gather data for section
    @template_section = {}
    @template_section[:title] = 'Tasty Treats'
    @template_section[:tables] = template_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @template_section
    end

    # notes:
    # The data below would typically come from the model or simulation results
    # You can loop through objects to make a table for each item of that type, such as air loops
    # If a section will only have one table you can leave the table title blank and just rely on the section title
    # these will be updated later to support graphs

    # create table
    template_table_01 = {}
    template_table_01[:title] = 'Fruit'
    template_table_01[:header] = ['Definition', 'Value']
    template_table_01[:units] = ['', '$/pound']
    template_table_01[:data] = []

    # add rows to table
    template_table_01[:data] << ['Banana', 0.23]
    template_table_01[:data] << ['Apple', 0.75]
    template_table_01[:data] << ['Orange', 0.50]

    # add table to array of tables
    template_tables << template_table_01

    # using helper method that generates table for second example
    template_tables << OsLib_Reporting.template_table(model, sqlFile, runner, is_ip_units = true)

    return @template_section
  end

  # create template section
  def self.template_table(model, sqlFile, runner, is_ip_units = true)
    # create a second table
    template_table = {}
    template_table[:title] = 'Ice Cream'
    template_table[:header] = ['Definition', 'Base Flavor', 'Toppings', 'Value']
    template_table[:units] = ['', '', '', 'scoop']
    template_table[:data] = []

    # add rows to table
    template_table[:data] << ['Vanilla', 'Vanilla', 'NA', 1.5]
    template_table[:data] << ['Rocky Road', 'Chocolate', 'Nuts', 1.5]
    template_table[:data] << ['Mint Chip', 'Mint', 'Chocolate Chips', 1.5]

    return template_table
  end

  # building_summary section
  def self.building_summary_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    general_tables = []

    # gather data for section
    @building_summary_section = {}
    @building_summary_section[:title] = 'Model Summary'
    @building_summary_section[:tables] = general_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @building_summary_section
    end

    # add in general information from method
    general_tables << OsLib_Reporting.general_building_information_table(model, sqlFile, runner, is_ip_units)
    general_tables << OsLib_Reporting.weather_summary_table(model, sqlFile, runner, is_ip_units)
    general_tables << OsLib_Reporting.design_day_table(model, sqlFile, runner, is_ip_units)
    general_tables << OsLib_Reporting.setpoint_not_met_summary_table(model, sqlFile, runner, is_ip_units)
    general_tables << OsLib_Reporting.setpoint_not_met_criteria_table(model, sqlFile, runner, is_ip_units)
    # general_tables << OsLib_Reporting.site_performance_table(model,sqlFile,runner)
    site_power_generation_table = OsLib_Reporting.site_power_generation_table(model, sqlFile, runner, is_ip_units)
    if site_power_generation_table
      general_tables << OsLib_Reporting.site_power_generation_table(model, sqlFile, runner, is_ip_units)
    end

    return @building_summary_section
  end

  # annual_overview section
  def self.annual_overview_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    annual_tables = []

    # gather data for section
    @annual_overview_section = {}
    @annual_overview_section[:title] = 'Annual Overview'
    @annual_overview_section[:tables] = annual_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @annual_overview_section
    end

    # add in annual overview from method
    annual_tables << OsLib_Reporting.output_data_end_use_table(model, sqlFile, runner, is_ip_units)
    annual_tables << OsLib_Reporting.output_data_energy_use_table(model, sqlFile, runner, is_ip_units)
    annual_tables << OsLib_Reporting.output_data_end_use_electricity_table(model, sqlFile, runner, is_ip_units)
    annual_tables << OsLib_Reporting.output_data_end_use_gas_table(model, sqlFile, runner, is_ip_units)

    return @annual_overview_section
  end

  # create table with general building information
  # this just makes a table, and not a full section. It feeds into another method that makes a full section
  def self.general_building_information_table(model, sqlFile, runner, is_ip_units = true)
    # general building information type data output
    general_building_information = {}
    general_building_information[:title] = 'Building Summary' # name will be with section
    general_building_information[:header] = ['Data', 'Value']
    general_building_information[:units] = [] # won't populate for this table since each row has different units
    general_building_information[:data] = []

    # structure ID / building name
    display = 'Building Name'
    target_units = 'n/a'
    value = model.getBuilding.name.to_s
    general_building_information[:data] << [display, value]
    runner.registerValue(OsLib_Reporting.reg_val_string_prep(display), value, target_units)

    # total site energy
    display = 'Total Site Energy'
    source_units = 'GJ'
    if is_ip_units
      target_units = 'kBtu'
    else
      target_units = 'kWh'
    end
    value = OpenStudio.convert(sqlFile.totalSiteEnergy.get, source_units, target_units).get
    value_neat = "#{OpenStudio.toNeatString(value, 0, true)} #{target_units}"
    runner.registerValue(OsLib_Reporting.reg_val_string_prep(display), value, target_units)
    general_building_information[:data] << [display, value_neat]

    # net site energy
    display = 'Net Site Energy'
    source_units = 'GJ'
    if is_ip_units
      target_units = 'kBtu'
    else
      target_units = 'kWh'
    end
    value = OpenStudio.convert(sqlFile.netSiteEnergy.get, source_units, target_units).get
    value_neat = "#{OpenStudio.toNeatString(value, 0, true)} #{target_units}"
    runner.registerValue(OsLib_Reporting.reg_val_string_prep(display), value, target_units)
    # always register value, but only add to table if net is different than total
    if sqlFile.totalSiteEnergy.get != sqlFile.netSiteEnergy.get
      general_building_information[:data] << [display, value_neat]
    end

    # total building area
    query = 'SELECT Value FROM tabulardatawithstrings WHERE '
    query << "ReportName='AnnualBuildingUtilityPerformanceSummary' and "
    query << "ReportForString='Entire Facility' and "
    query << "TableName='Building Area' and "
    query << "RowName='Total Building Area' and "
    query << "ColumnName='Area' and "
    query << "Units='m2';"
    query_results = sqlFile.execAndReturnFirstDouble(query)
    if query_results.empty?
      runner.registerWarning('Did not find value for total building area.')
      return false
    else
      display = 'Total Building Area'
      source_units = 'm^2'
      if is_ip_units
        target_units = 'ft^2'
      else
        target_units = source_units
      end
      value = OpenStudio.convert(query_results.get, source_units, target_units).get
      value_neat = "#{OpenStudio.toNeatString(value, 0, true)} #{target_units}"
      general_building_information[:data] << [display, value_neat]
      runner.registerValue(OsLib_Reporting.reg_val_string_prep(display), value, target_units)
    end

    # code to check OS vs. E+ area
    energy_plus_area = query_results.get
    open_studio_area = model.getBuilding.floorArea
    if (energy_plus_area - open_studio_area).abs >= 1.0
      runner.registerWarning("EnergyPlus reported area is #{query_results.get.round} (m^2). OpenStudio reported area is #{model.getBuilding.floorArea.round} (m^2).")
    end

    # total EUI
    eui = sqlFile.totalSiteEnergy.get / energy_plus_area
    display = 'Total Site EUI'
    source_units = 'GJ/m^2'
    if is_ip_units
      target_units = 'kBtu/ft^2'
    else
      target_units = 'kWh/m^2'
    end
    if query_results.get > 0.0 # don't calculate EUI if building doesn't have any area
      value = OpenStudio.convert(eui, source_units, target_units).get
      value_neat = "#{OpenStudio.toNeatString(value, 2, true)} #{target_units}"
      runner.registerValue(OsLib_Reporting.reg_val_string_prep(display), value, target_units) # is it ok not to calc EUI if no area in model
    else
      value_neat = "can't calculate Total EUI."
    end
    general_building_information[:data] << [display.to_s, value_neat]

    # net EUI
    eui = sqlFile.netSiteEnergy.get / energy_plus_area
    display = 'EUI'
    if query_results.get > 0.0 # don't calculate EUI if building doesn't have any area
      value = OpenStudio.convert(eui, source_units, target_units).get
      value_neat = "#{OpenStudio.toNeatString(value, 2, true)} #{target_units}"
      runner.registerValue(OsLib_Reporting.reg_val_string_prep(display), value, target_units) # is it ok not to calc EUI if no area in model
    else
      value_neat = 'Net EUI could not be calculated.'
    end
    # always register value, but only add to table if net is different than total
    if sqlFile.totalSiteEnergy.get != sqlFile.netSiteEnergy.get
      general_building_information[:data] << ['Net Site EUI', value_neat]
    end

    # TODO: - add total EUI for conditioned floor area if not the same as total. Doesn't seem necessary to also calculate net conditioned EUI if it exists as a unique value.

    # conditioned building area
    query = 'SELECT Value FROM tabulardatawithstrings WHERE '
    query << "ReportName='AnnualBuildingUtilityPerformanceSummary' and "
    query << "ReportForString='Entire Facility' and "
    query << "TableName='Building Area' and "
    query << "RowName='Net Conditioned Building Area' and "
    query << "ColumnName='Area' and "
    query << "Units='m2';"
    query_results = sqlFile.execAndReturnFirstDouble(query)
    if query_results.empty?
      runner.registerWarning('Did not find value for net conditioned building area.')
      return false
    elsif query_results.get == 0.0
      display_a = 'Conditioned Building Area'
      source_units_a = 'm^2'
      if is_ip_units
        target_units_a = 'ft^2'
      else
        target_units_a = source_units_a
      end
      value_a = OpenStudio.convert(query_results.get, source_units_a, target_units_a).get
      value_neat_a = "#{OpenStudio.toNeatString(value_a, 0, true)} #{target_units_a}"
      runner.registerValue(OsLib_Reporting.reg_val_string_prep(display_a), value_a, target_units_a)

      # conditioned EUI
      eui = sqlFile.totalSiteEnergy.get / energy_plus_area
      display_e = 'Conditioned Site EUI'
      source_units_e = 'GJ/m^2'
      if is_ip_units
        target_units_e = 'kBtu/ft^2'
      else
        target_units_e = 'kWh/m^2'
      end
      value_e = OpenStudio.convert(eui, source_units_e, target_units_e).get
      value_neat_e = "#{OpenStudio.toNeatString(value_e, 2, true)} #{target_units_e}"
      runner.registerValue(OsLib_Reporting.reg_val_string_prep(display_e), value_e, target_units_e)

      # always register value, but only add to table if net is different than total
      if energy_plus_area - query_results.get >= 1.0
        general_building_information[:data] << [display_a, value_neat_a]
        general_building_information[:data] << [display_e, value_neat_e]
      end
    end

    # get standards building type
    building_type = 'n/a'
    if model.getBuilding.standardsBuildingType.is_initialized
      building_type = model.getBuilding.standardsBuildingType.get
    end
    general_building_information[:data] << ['OpenStudio Standards Building Type', building_type]

    return general_building_information
  end

  # create table of space type breakdown
  def self.space_type_breakdown_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # space type data output
    output_data_space_type_breakdown = {}
    output_data_space_type_breakdown[:title] = ''
    output_data_space_type_breakdown[:header] = ['Space Type Name', 'Floor Area', 'Standards Building Type', 'Standards Space Type']
    if is_ip_units
      target_units = 'ft^2'
    else
      target_units = 'm^2'
    end
    output_data_space_type_breakdown[:units] = ['', target_units, '', '']
    output_data_space_type_breakdown[:data] = []
    output_data_space_type_breakdown[:chart_type] = 'simple_pie'
    output_data_space_type_breakdown[:chart] = []

    # gather data for section
    @output_data_space_type_breakdown_section = {}
    @output_data_space_type_breakdown_section[:title] = 'Space Type Breakdown'
    @output_data_space_type_breakdown_section[:tables] = [output_data_space_type_breakdown] # only one table for this section

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @output_data_space_type_breakdown_section
    end

    space_types = model.getSpaceTypes

    space_types.sort.each do |spaceType|
      next if spaceType.floorArea == 0

      # get color
      color = spaceType.renderingColor
      if !color.empty?
        color = color.get
        red = color.renderingRedValue
        green = color.renderingGreenValue
        blue = color.renderingBlueValue
        color = "rgb(#{red},#{green},#{blue})"
      else
        # TODO: - this should set red green and blue as separate values
        color = 'rgb(20,20,20)' # maybe do random or let d3 pick color instead of this?
      end

      # data for space type breakdown
      display = spaceType.name.get
      floor_area_si = spaceType.floorArea
      floor_area_si = 0
      # loop through spaces so I can skip if not included in floor area
      spaceType.spaces.each do |space|
        next if !space.partofTotalFloorArea
        floor_area_si += space.floorArea * space.multiplier
      end

      # julien
      source_units = 'm^2'
      if is_ip_units
        target_units = 'ft^2'
      else
        target_units = source_units
      end

      value = OpenStudio.convert(floor_area_si, 'm^2', target_units).get
      num_people = nil
      value_neat = OpenStudio.toNeatString(value, 0, true)

      # get standards information
      if spaceType.standardsBuildingType.is_initialized
        standards_building_type = spaceType.standardsBuildingType.get
      else
        standards_building_type = ''
      end
      if spaceType.standardsSpaceType.is_initialized
        standards_space_type = spaceType.standardsSpaceType.get
      else
        standards_space_type = ''
      end

      output_data_space_type_breakdown[:data] << [display, value_neat, standards_building_type, standards_space_type]
      runner.registerValue("space_type_#{OsLib_Reporting.reg_val_string_prep(display)}", value, target_units)

      # data for graph
      output_data_space_type_breakdown[:chart] << JSON.generate(label: display, value: value, color: color)
    end

    spaces = model.getSpaces

    # count area of spaces that have no space type
    no_space_type_area_counter = 0

    spaces.each do |space|
      if space.spaceType.empty?
        next if !space.partofTotalFloorArea
        no_space_type_area_counter += space.floorArea * space.multiplier
      end
    end

    if no_space_type_area_counter > 0
      display = 'No Space Type'
      # julien
      source_units = 'm^2'
      if is_ip_units
        target_units = 'ft^2'
      else
        target_units = source_units
      end
      value = OpenStudio.convert(no_space_type_area_counter, 'm^2', target_units).get
      value_neat = OpenStudio.toNeatString(value, 0, true)
      output_data_space_type_breakdown[:data] << [display, value_neat]
      runner.registerValue("space_type_#{OsLib_Reporting.reg_val_string_prep(display)}", value, target_units)

      # data for graph
      color = 'rgb(20,20,20)' # maybe do random or let d3 pick color instead of this?
      output_data_space_type_breakdown[:chart] << JSON.generate(label: 'No SpaceType Assigned',
                                                                value: OpenStudio.convert(no_space_type_area_counter, 'm^2', target_units),
                                                                color: color)
    end

    return @output_data_space_type_breakdown_section
  end

  # create table with general building information
  # this just makes a table, and not a full section. It feeds into another method that makes a full section
  def self.output_data_end_use_table(model, sqlFile, runner, is_ip_units = true)
    # end use data output
    output_data_end_use = {}
    output_data_end_use[:title] = 'End Use'
    output_data_end_use[:header] = ['End Use', 'Consumption']
    if is_ip_units
      target_units = 'kBtu'
    else
      target_units = 'kWh'
    end
    output_data_end_use[:units] = ['', target_units]
    output_data_end_use[:data] = []
    output_data_end_use[:chart_type] = 'simple_pie'
    output_data_end_use[:chart] = []

    end_use_colors = ['#EF1C21', '#0071BD', '#F7DF10', '#DEC310', '#4A4D4A', '#B5B2B5', '#FF79AD', '#632C94', '#F75921', '#293094', '#CE5921', '#FFB239', '#29AAE7', '#8CC739']

    # loop through fuels for consumption tables
    counter = 0
    OpenStudio::EndUseCategoryType.getValues.each do |end_use|
      # get end uses
      end_use = OpenStudio::EndUseCategoryType.new(end_use).valueDescription

      # loop through fuels
      total_end_use = 0.0
      fuel_type_names.each do |fuel_type|
        query_fuel = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='AnnualBuildingUtilityPerformanceSummary' and TableName='End Uses' and RowName= '#{end_use}' and ColumnName= '#{fuel_type}'"
        results_fuel = sqlFile.execAndReturnFirstDouble(query_fuel).get
        total_end_use += results_fuel
      end

      # convert value and populate table
      value = OpenStudio.convert(total_end_use, 'GJ', target_units).get
      value_neat = OpenStudio.toNeatString(value, 0, true)
      output_data_end_use[:data] << [end_use, value_neat]
      runner.registerValue("end_use_#{end_use.downcase.tr(' ', '_')}", value, target_units)
      if value > 0
        output_data_end_use[:chart] << JSON.generate(label: end_use, value: value, color: end_use_colors[counter])
      end

      counter += 1
    end

    return output_data_end_use
  end

  # create table with general building information
  # this just makes a table, and not a full section. It feeds into another method that makes a full section
  def self.output_data_end_use_electricity_table(model, sqlFile, runner, is_ip_units = true)
    # end use data output
    output_data_end_use_electricity = {}
    output_data_end_use_electricity[:title] = 'EUI - Electricity'
    output_data_end_use_electricity[:header] = ['End Use', 'Consumption']
    target_units = 'kWh'
    output_data_end_use_electricity[:units] = ['', target_units]
    output_data_end_use_electricity[:data] = []
    output_data_end_use_electricity[:chart_type] = 'simple_pie'
    output_data_end_use_electricity[:chart] = []

    end_use_colors = ['#EF1C21', '#0071BD', '#F7DF10', '#DEC310', '#4A4D4A', '#B5B2B5', '#FF79AD', '#632C94', '#F75921', '#293094', '#CE5921', '#FFB239', '#29AAE7', '#8CC739']

    # loop through fuels for consumption tables
    counter = 0
    OpenStudio::EndUseCategoryType.getValues.each do |end_use|
      # get end uses
      end_use = OpenStudio::EndUseCategoryType.new(end_use).valueDescription
      query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='AnnualBuildingUtilityPerformanceSummary' and TableName='End Uses' and RowName= '#{end_use}' and ColumnName= 'Electricity'"
      results = sqlFile.execAndReturnFirstDouble(query)
      value = OpenStudio.convert(results.get, 'GJ', target_units).get
      value_neat = OpenStudio.toNeatString(value, 0, true)
      output_data_end_use_electricity[:data] << [end_use, value_neat]
      runner.registerValue("end_use_electricity_#{end_use.downcase.tr(' ', '_')}", value, target_units)
      if value > 0
        output_data_end_use_electricity[:chart] << JSON.generate(label: end_use, value: value, color: end_use_colors[counter])
      end

      counter += 1
    end

    return output_data_end_use_electricity
  end

  # create table with general building information
  # this just makes a table, and not a full section. It feeds into another method that makes a full section
  def self.output_data_end_use_gas_table(model, sqlFile, runner, is_ip_units = true)
    # end use data output
    output_data_end_use_gas = {}
    output_data_end_use_gas[:title] = 'EUI - Gas'
    output_data_end_use_gas[:header] = ['End Use', 'Consumption']
    if is_ip_units
      target_units = 'therm'
      target_display_unit = 'therms'
    else
      target_units = 'kWh'
      target_display_unit = 'kWh'
    end
    output_data_end_use_gas[:units] = ['', target_display_unit]
    output_data_end_use_gas[:data] = []
    output_data_end_use_gas[:chart_type] = 'simple_pie'
    output_data_end_use_gas[:chart] = []
    output_data_end_use_gas[:chart_type] = 'simple_pie'
    output_data_end_use_gas[:chart] = []

    end_use_colors = ['#EF1C21', '#0071BD', '#F7DF10', '#DEC310', '#4A4D4A', '#B5B2B5', '#FF79AD', '#632C94', '#F75921', '#293094', '#CE5921', '#FFB239', '#29AAE7', '#8CC739']

    # loop through fuels for consumption tables
    counter = 0
    OpenStudio::EndUseCategoryType.getValues.each do |end_use|
      # get end uses
      end_use = OpenStudio::EndUseCategoryType.new(end_use).valueDescription
      query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='AnnualBuildingUtilityPerformanceSummary' and TableName='End Uses' and RowName= '#{end_use}' and ColumnName= 'Natural Gas'"
      results = sqlFile.execAndReturnFirstDouble(query)
      value = OpenStudio.convert(results.get, 'GJ', target_units).get
      value_neat = OpenStudio.toNeatString(value, 0, true)
      output_data_end_use_gas[:data] << [end_use, value_neat]
      runner.registerValue("end_use_natural_gas_#{end_use.downcase.tr(' ', '_')}", value, target_units)
      if value > 0
        output_data_end_use_gas[:chart] << JSON.generate(label: end_use, value: value, color: end_use_colors[counter])
      end

      counter += 1
    end

    return output_data_end_use_gas
  end

  # create table with general building information
  # this just makes a table, and not a full section. It feeds into another method that makes a full section
  def self.output_data_energy_use_table(model, sqlFile, runner, is_ip_units = true)
    # energy use data output
    output_data_energy_use = {}
    output_data_energy_use[:title] = 'Energy Use'
    output_data_energy_use[:header] = ['Fuel', 'Consumption']
    if is_ip_units
      target_units = 'kBtu'
    else
      target_units = 'kWh'
    end
    output_data_energy_use[:units] = ['', target_units]
    output_data_energy_use[:data] = []
    output_data_energy_use[:chart_type] = 'simple_pie'
    output_data_energy_use[:chart] = []

    # list of colors for fuel. Also used for cash flow chart
    color = []
    color << '#DDCC77' # Electricity
    color << '#999933' # Natural Gas

    # not used for 9.4
    # color << '#AA4499' # Additional Fuel

    # TODO: - new colors to add for 9.4 (for nwo using color of Additional Fuel)
    color << '#AA4499'
    color << '#AA4499'
    color << '#AA4499'
    color << '#AA4499'
    color << '#AA4499'
    color << '#AA4499'
    color << '#AA4499'

    color << '#88CCEE' # District Cooling
    color << '#CC6677' # District Heating
    # color << "#332288" # Water (not used here but is in cash flow chart)
    # color << "#117733" # Capital and O&M (not used here but is in cash flow chart)

    # loop through fuels for consumption tables
    counter = 0
    fuel_type_names.each do |fuel_type| # OpenStudio::EndUseFuelType.getValues
      # get fuel type and units
      # fuel_type = OpenStudio::EndUseFuelType.new(fuel_type).valueDescription
      next if fuel_type == 'Water'
      query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='AnnualBuildingUtilityPerformanceSummary' and TableName='End Uses' and RowName= 'Total End Uses' and ColumnName= '#{fuel_type}'"
      results = sqlFile.execAndReturnFirstDouble(query)

      # julien
      source_units = 'kWh'
      if is_ip_units
        target_units = 'kBtu'
      else
        target_units = source_units
      end

      target_units = 'kBtu'
      value = OpenStudio.convert(results.get, 'GJ', target_units).get
      value_neat = OpenStudio.toNeatString(value, 0, true)
      output_data_energy_use[:data] << [fuel_type, value_neat]
      runner.registerValue("fuel_#{fuel_type.downcase.tr(' ', '_')}", value, target_units)
      if value > 0
        output_data_energy_use[:chart] << JSON.generate(label: fuel_type, value: value, color: color[counter])
      end

      counter += 1
    end

    return output_data_energy_use
  end

  # create table for advisory messages
  def self.setpoint_not_met_summary_table(model, sqlFile, runner, is_ip_units = true)
    # unmet hours data output
    setpoint_not_met_summary = {}
    setpoint_not_met_summary[:title] = 'Unmet Hours Summary'
    setpoint_not_met_summary[:header] = ['Time Setpoint Not Met', 'Time']
    target_units = 'hr'
    setpoint_not_met_summary[:units] = ['', target_units]
    setpoint_not_met_summary[:data] = []

    # create string for rows (transposing from what is in tabular data)
    setpoint_not_met_cat = []
    setpoint_not_met_cat << 'During Heating'
    setpoint_not_met_cat << 'During Cooling'
    setpoint_not_met_cat << 'During Occupied Heating'
    setpoint_not_met_cat << 'During Occupied Cooling'

    # loop through  messages
    setpoint_not_met_cat.each do |cat|
      # Retrieve end use percentages from  table
      query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='SystemSummary' and TableName = 'Time Setpoint Not Met' and RowName= 'Facility' and ColumnName='#{cat}';"
      setpoint_not_met_cat_value = sqlFile.execAndReturnFirstDouble(query)
      if setpoint_not_met_cat_value.empty?
        runner.registerWarning("Did not find value for #{cat}.")
        return false
      else
        # net site energy
        display = cat
        source_units = 'hr'
        value = setpoint_not_met_cat_value.get
        value_neat = value # OpenStudio::toNeatString(value,0,true)
        setpoint_not_met_summary[:data] << [display, value_neat]
        runner.registerValue("unmet_hours_#{OsLib_Reporting.reg_val_string_prep(display)}", value, target_units)

      end
    end # setpoint_not_met_cat.each do

    return setpoint_not_met_summary
  end

  # create table for setpoint_not_met_criteria
  def self.setpoint_not_met_criteria_table(model, sqlFile, runner, is_ip_units = true)
    # unmet hours data output
    tolerance_summary = {}
    tolerance_summary[:title] = 'Unmet Hours Tolerance'
    tolerance_summary[:header] = ['Tolerance for Time Setpoint Not Met', 'Temperature']

    # julien
    source_units = 'C'
    if is_ip_units
      target_units = 'F'
    else
      target_units = source_units
    end

    tolerance_summary[:units] = ['', target_units]
    tolerance_summary[:data] = []

    # create string for rows (transposing from what is in tabular data)
    setpoint_not_met_cat = []
    setpoint_not_met_cat << 'Heating'
    setpoint_not_met_cat << 'Cooling'

    # loop through  messages
    setpoint_not_met_cat.each do |cat|
      # Retrieve end use percentages from  table
      query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='AnnualBuildingUtilityPerformanceSummary' and TableName = 'Setpoint Not Met Criteria' and RowName= 'Tolerance for Zone #{cat} Setpoint Not Met Time' and ColumnName='Degrees';"
      setpoint_not_met_cat_value = sqlFile.execAndReturnFirstDouble(query)
      if setpoint_not_met_cat_value.empty?
        runner.registerWarning("Did not find value for #{cat}.")
        return false
      else
        # net site energy
        display = cat

        # julien
        source_units = 'K'
        if is_ip_units
          target_units = 'R'
        else
          target_units = source_units
        end

        value = OpenStudio.convert(setpoint_not_met_cat_value.get.to_f, source_units, target_units).get
        value_neat = value.round(2)
        tolerance_summary[:data] << [display, value_neat]
        runner.registerValue("unmet_hours_tolerance_#{cat.downcase}", value, target_units)
      end
    end # setpoint_not_met_cat.each do

    return tolerance_summary
  end

  # Generalized method for getting component performance characteristics
  def self.general_component_summary_logic(component, is_ip_units)
    data_arrays = []

    # Convert to HVAC Component
    component = component.to_HVACComponent
    return data_arrays if component.empty?
    component = component.get

    # Skip object types that are not informative
    types_to_skip = ['SetpointManager:MixedAir', 'Node']
    idd_obj_type = component.iddObject.name.gsub('OS:', '')
    return data_arrays if types_to_skip.include?(idd_obj_type)

    # Only report the component type once
    comp_name_used = false

    # Airflow, heating and cooling capacity, and water flow
    summary_types = []
    summary_types << ['Air Flow Rate', 'maxAirFlowRate', 'm^3/s', 4, 'cfm', 0]
    summary_types << ['Heating Capacity', 'maxHeatingCapacity', 'W', 1, 'Btu/hr', 1]
    summary_types << ['Cooling Capacity', 'maxCoolingCapacity', 'W', 1, 'ton', 1]
    summary_types << ['Water Flow Rate', 'maxWaterFlowRate', 'm^3/s', 4, 'gal/min', 2]
    summary_types << ['Rated Power', 'ratedPower', 'W', 1, 'W', 1]
    summary_types.each do |s|
      val_name = s[0]
      val_method = s[1]

      units_si = s[2]
      decimal_places_si = s[3]

      units_ip = s[4]
      decimal_places_ip = s[5]

      # Get the value and skip if not available
      val_si = component.public_send(val_method)
      next if val_si.empty?
      # Determine if the value was autosized or hard sized
      siz = 'Hard Sized'
      if component.public_send("#{val_method}Autosized").is_initialized
        if component.public_send("#{val_method}Autosized").get
          siz = 'Autosized'
        end
      end
      # Convert and report the value

      source_units = units_si
      if is_ip_units
        target_units = units_ip
        decimal_places = decimal_places_ip
      else
        target_units = source_units
        decimal_places = decimal_places_si
      end

      val_ip = OpenStudio.convert(val_si.get, source_units, target_units).get
      val_ip_neat = OpenStudio.toNeatString(val_ip, decimal_places, true)
      if is_ip_units
        data_arrays << ['', val_name, "#{val_ip_neat} #{units_ip}", siz, '']
      else
        data_arrays << ['', val_name, "#{val_ip_neat} #{units_si}", siz, '']
      end
    end

    # Performance characteristics (specific to each component type)
    perf_chars = component.performanceCharacteristics.each do |char|
      perf_val = char[0].to_s.to_f
      perf_name = char[1]
      display_units = '' # For Display
      # Unit conversion for pressure rise and pump head
      if perf_name.downcase.include?('pressure rise')
        source_units = 'Pa'
        if is_ip_units
          target_units = 'inH_{2}O'
          display_units = 'in w.g.'
        else
          target_units = source_units
          display_units = source_units
        end
        perf_val = OpenStudio.convert(perf_val, source_units, target_units).get
        perf_val = OpenStudio.toNeatString(perf_val, 2, true)
      elsif perf_name.downcase.include?('pump head')
        source_units = 'Pa'
        if is_ip_units
          target_units = 'ftH_{2}O'
          display_units = 'ft H2O'
          n_decimals = 1
        else
          target_units = source_units
          display_units = source_units
          n_decimals = 0
        end
        perf_val = OpenStudio.convert(perf_val, source_units, target_units).get
        perf_val = OpenStudio.toNeatString(perf_val, n_decimals, true)
      elsif perf_name.downcase.include?('efficiency') || perf_name.downcase.include?('effectiveness')
        display_units = '%'
        perf_val *= 100
        perf_val = OpenStudio.toNeatString(perf_val, 1, true)
      end
      # Report the value
      data_arrays << ['', perf_name, "#{perf_val} #{display_units}", '', '']
    end

    return data_arrays
  end

  # Gives the Plant Loop connection information for an HVAC Component
  def self.water_component_logic(component, is_ip_units)
    data_arrays = []

    component = component.to_HVACComponent
    return data_arrays if component.empty?
    component = component.get

    # Only deal with plant-connected components
    return data_arrays unless component.respond_to?('plantLoop')

    # Report the plant loop name
    if component.plantLoop.is_initialized
      data_arrays << ['', 'Plant Loop', component.plantLoop.get.name, '', '']
    end

    return data_arrays
  end

  # Shows the calculated brake horsepower for fans and pumps
  def self.motor_component_logic(component, is_ip_units)
    data_arrays = []

    # Skip exhaust fans for now because of bug in openstudio-standards
    return data_arrays if component.to_FanZoneExhaust.is_initialized

    concrete_comp = component.cast_to_concrete_type
    component = concrete_comp unless component.nil?

    # Only deal with plant-connected components
    return data_arrays unless component.respond_to?('brake_horsepower')

    # Report the plant loop name
    bhp = component.brake_horsepower
    bhp_neat = OpenStudio.toNeatString(bhp, 2, true)
    data_arrays << ['', 'Brake Horsepower', "#{bhp_neat} HP", '', '']

    return data_arrays
  end

  # Shows the setpoint manager details depending on type
  def self.spm_logic(component, is_ip_units)
    data_arrays = []

    case component.iddObject.name
    when 'OS:SetpointManager:Scheduled'
      # Constrol type and temperature range
      setpoint = component.to_SetpointManagerScheduled.get
      supply_air_temp_schedule = setpoint.schedule
      schedule_values = OsLib_Schedules.getMinMaxAnnualProfileValue(component.model, supply_air_temp_schedule)
      if schedule_values.nil?
        schedule_values_pretty = "can't inspect schedule"
        target_units = ''
      else
        if setpoint.controlVariable.to_s == 'Temperature'
          # julien
          source_units = 'C'
          if is_ip_units
            target_units = 'F'
          else
            target_units = source_units
          end

          schedule_values_pretty = "#{OpenStudio.convert(schedule_values['min'], source_units, target_units).get.round(1)} to #{OpenStudio.convert(schedule_values['max'], source_units, target_units).get.round(1)}"
        else # TODO: - add support for other control variables
          schedule_values_pretty = "#{schedule_values['min']} to #{schedule_values['max']}"
          target_units = 'raw si values'
        end
      end
      data_arrays << ['', "Control Variable - #{setpoint.controlVariable}", "#{schedule_values_pretty} #{target_units}", '', '']

    when 'OS:SetpointManager:SingleZone:Reheat'
      # Control Zone
      setpoint = component.to_SetpointManagerSingleZoneReheat.get
      control_zone = setpoint.controlZone
      if control_zone.is_initialized
        control_zone_name = control_zone.get.name
      else
        control_zone_name = ''
      end
      data_arrays << ['', 'Control Zone', control_zone_name, '', '']

    when 'OS:SetpointManager:FollowOutdoorAirTemperature'
      setpoint = component.to_SetpointManagerFollowOutdoorAirTemperature.get
      ref_temp_type = setpoint.referenceTemperatureType
      data_arrays << [setpoint.iddObject.name, 'Reference Temperature Type', ref_temp_type, '', '']

    when 'OS:SetpointManager:OutdoorAirReset'
      setpoint = component.to_SetpointManagerOutdoorAirReset.get
      source_units = 'C'
      if is_ip_units
        target_units = 'F'
      else
        target_units = source_units
      end
      wt_at_hi_oat_f = OpenStudio.convert(setpoint.setpointatOutdoorHighTemperature, source_units, target_units).get.round(1)
      wt_at_lo_oat_f = OpenStudio.convert(setpoint.setpointatOutdoorLowTemperature, source_units, target_units).get.round(1)
      hi_oat_f = OpenStudio.convert(setpoint.outdoorHighTemperature, source_units, target_units).get.round(1)
      lo_oat_f = OpenStudio.convert(setpoint.outdoorLowTemperature, source_units, target_units).get.round(1)
      # julien
      if is_ip_units
        desc = "#{wt_at_lo_oat_f} F to #{wt_at_hi_oat_f.round} F btwn OAT #{lo_oat_f} F to #{hi_oat_f} F."
      else
        desc = "#{wt_at_lo_oat_f} C to #{wt_at_hi_oat_f.round} C btwn OAT #{lo_oat_f} C to #{hi_oat_f} C."
      end

      data_arrays << [setpoint.iddObject.name, 'Reset', desc, '', '']

    when 'OS:SetpointManager:Warmest'
      setpoint = component.to_SetpointManagerWarmest.get
      source_units = 'C'
      if is_ip_units
        target_units = 'F'
      else
        target_units = source_units
      end
      min_sat_f = OpenStudio.convert(setpoint.minimumSetpointTemperature, source_units, target_units).get.round(1)
      max_sat_f = OpenStudio.convert(setpoint.maximumSetpointTemperature, source_units, target_units).get.round(1)
      desc = "#{min_sat_f} #{target_units} to #{max_sat_f.round} #{target_units}"
      data_arrays << [setpoint.iddObject.name, 'Reset SAT per Worst Zone', desc, '', '']

    when 'OS:SetpointManager:WarmestTemperatureFlow'
      setpoint = component.to_SetpointManagerWarmestTemperatureFlow.get
      source_units = 'C'
      if is_ip_units
        target_units = 'F'
      else
        target_units = source_units
      end
      min_sat_f = OpenStudio.convert(setpoint.minimumSetpointTemperature, source_units, target_units).get.round(1)
      max_sat_f = OpenStudio.convert(setpoint.maximumSetpointTemperature, source_units, target_units).get.round(1)
      desc = "#{min_sat_f} #{target_units} to #{max_sat_f.round} #{target_units}, #{setpoint.strategy}"
      data_arrays << [setpoint.iddObject.name, 'Reset SAT & Flow per Worst Zone', desc, '', '']
    end

    return data_arrays
  end

  # summary of what to show for each type of air loop component
  def self.air_loop_component_summary_logic(component, model, is_ip_units)
    # Generic component logic first
    data_arrays = general_component_summary_logic(component, is_ip_units)

    # Water component logic
    data_arrays += water_component_logic(component, is_ip_units)

    # Motor component logic
    data_arrays += motor_component_logic(component, is_ip_units)

    # Setpoint manager logic
    data_arrays += spm_logic(component, is_ip_units)

    # Unique logic for subset of components
    case component.iddObject.name
    when 'OS:AirLoopHVAC:OutdoorAirSystem'
      component = component.to_AirLoopHVACOutdoorAirSystem.get
      controller_oa = component.getControllerOutdoorAir

      # Min OA
      # julien
      source_units = 'm^3/s'
      if is_ip_units
        target_units = 'cfm'
        n_decimals = 0
      else
        target_units = 'm^3/h'
        n_decimals = 0
      end

      if controller_oa.minimumOutdoorAirFlowRate.is_initialized
        value = OpenStudio.convert(controller_oa.minimumOutdoorAirFlowRate.get, source_units, target_units).get
        value_neat = OpenStudio.toNeatString(value, n_decimals, true)
        siz = 'Hard Sized'
      elsif controller_oa.autosizedMinimumOutdoorAirFlowRate.is_initialized
        value = OpenStudio.convert(controller_oa.autosizedMinimumOutdoorAirFlowRate.get, source_units, target_units).get
        value_neat = OpenStudio.toNeatString(value, n_decimals, true)
        siz = 'Autosized'
      else
        value_neat = 'Autosized'
      end
      data_arrays << ['', 'Minimum Outdoor Air Flow Rate', "#{value_neat} #{target_units}", siz, '']

      # Max OA
      if controller_oa.maximumOutdoorAirFlowRate.is_initialized
        value_ip = OpenStudio.convert(controller_oa.maximumOutdoorAirFlowRate.get, source_units, target_units).get
        value_ip_neat = OpenStudio.toNeatString(value_ip, n_decimals, true)
        siz = 'Hard Sized'
      elsif controller_oa.autosizedMaximumOutdoorAirFlowRate.is_initialized
        value_ip = OpenStudio.convert(controller_oa.autosizedMaximumOutdoorAirFlowRate.get, source_units, target_units).get
        value_ip_neat = OpenStudio.toNeatString(value_ip, n_decimals, true)
        siz = 'Autosized'
      else
        value_ip_neat = 'Autosized'
      end
      data_arrays << ['', 'Maximum Outdoor Air Flow Rate', "#{value_ip_neat} #{target_units}", siz, '']
    end

    # Make the component type the first element of the first row
    if !data_arrays.empty?
      data_arrays[0][0] = component.iddObject.name.gsub('OS:', '')
    end

    return data_arrays
  end

  # create table air loop summary
  def self.air_loops_detail_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    output_data_air_loop_tables = []

    # gather data for section
    @output_data_air_loop_section = {}
    @output_data_air_loop_section[:title] = 'Air Loops Detail'
    @output_data_air_loop_section[:tables] = output_data_air_loop_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @output_data_air_loop_section
    end

    # loop through air loops
    model.getAirLoopHVACs.sort.each do |air_loop|
      # air loop data output
      output_data_air_loops = {}
      output_data_air_loops[:title] = air_loop.name.get # TODO: - confirm first that it has name
      output_data_air_loops[:header] = ['Object', 'Description', 'Value', 'Sizing', 'Count']
      output_data_air_loops[:units] = [] # not using units for these tables
      output_data_air_loops[:data] = []

      output_data_air_loops[:data] << [{ sub_header: 'supply' }, '', '', '', '']

      # hold values for later use
      dcv_setting = 'na' # should hit this if there isn't an outdoor air object on the loop
      economizer_setting = 'na' # should hit this if there isn't an outdoor air object on the loop

      # loop through components
      air_loop.supplyComponents.each do |component|
        # skip some object types, but look for node with setpoint manager
        if component.to_Node.is_initialized
          setpoint_managers = component.to_Node.get.setpointManagers
          if !setpoint_managers.empty?
            # setpoint type
            setpoint = setpoint_managers[0] # TODO: - could have more than one in some situations
            data_arrays = OsLib_Reporting.air_loop_component_summary_logic(setpoint, model, is_ip_units)
            data_arrays.each do |data_array|
              output_data_air_loops[:data] << data_array
            end
          end
        else
          # populate table for everything but setpoint managers, which are added above.
          data_arrays = OsLib_Reporting.air_loop_component_summary_logic(component, model, is_ip_units)
          data_arrays.each do |data_array|
            output_data_air_loops[:data] << data_array
          end

        end

        # gather controls information to use later
        if component.to_AirLoopHVACOutdoorAirSystem.is_initialized
          hVACComponent = component.to_AirLoopHVACOutdoorAirSystem.get

          # get ControllerOutdoorAir
          controller_oa = hVACComponent.getControllerOutdoorAir
          # get ControllerMechanicalVentilation
          controller_mv = controller_oa.controllerMechanicalVentilation
          # get dcv value
          dcv_setting = controller_mv.demandControlledVentilation
          if dcv_setting
            dcv_setting = 'On'
          else
            dcv_setting = 'Off'
          end
          # get economizer setting
          economizer_setting = controller_oa.getEconomizerControlType
        end
      end

      output_data_air_loops[:data] << [{ sub_header: 'demand' }, '', '', '', '']
      # demand side summary, list of terminal types used, and number of zones
      thermal_zones = []
      terminals = []
      cooling_temp_ranges = []
      heating_temps_ranges = []
      air_loop.demandComponents.each do |component|
        # gather array of thermal zones and terminals
        if component.to_ThermalZone.is_initialized
          thermal_zone = component.to_ThermalZone.get
          thermal_zones << thermal_zone
          thermal_zone.equipment.each do |zone_equip|
            next if zone_equip.to_ZoneHVACComponent.is_initialized # should only find terminals
            terminals << zone_equip.iddObject.name.gsub('OS:', '')
          end

          # populate thermostat ranges
          if thermal_zone.thermostatSetpointDualSetpoint.is_initialized
            thermostat = thermal_zone.thermostatSetpointDualSetpoint.get
            if thermostat.coolingSetpointTemperatureSchedule.is_initialized
              schedule_values = OsLib_Schedules.getMinMaxAnnualProfileValue(model, thermostat.coolingSetpointTemperatureSchedule.get)
              unless schedule_values.nil?
                cooling_temp_ranges << schedule_values['min']
                cooling_temp_ranges << schedule_values['max']
              end
            end
            if thermostat.heatingSetpointTemperatureSchedule.is_initialized
              schedule_values = OsLib_Schedules.getMinMaxAnnualProfileValue(model, thermostat.heatingSetpointTemperatureSchedule.get)
              unless schedule_values.nil?
                heating_temps_ranges << schedule_values['min']
                heating_temps_ranges << schedule_values['max']
              end
            end
          end

        end
      end

      # get floor area of thermal zones
      total_loop_floor_area = 0
      thermal_zones.each do |zone|
        total_loop_floor_area += zone.floorArea
      end

      source_units = 'm^2'
      if is_ip_units
        target_units = 'ft^2'
      else
        target_units = source_units
      end
      total_loop_floor_area = OpenStudio.convert(total_loop_floor_area, source_units, target_units).get
      total_loop_floor_area_neat = OpenStudio.toNeatString(total_loop_floor_area, 0, true)

      # output zone and terminal data
      if is_ip_units
        output_data_air_loops[:data] << ['Thermal Zones', 'Total Floor Area', "#{total_loop_floor_area_neat} ft^2", '', thermal_zones.size]
      else
        output_data_air_loops[:data] << ['Thermal Zones', 'Total Floor Area', "#{total_loop_floor_area_neat} m^2", '', thermal_zones.size]
      end

      # heating and cooling temperature range data
      source_units = 'C'
      if is_ip_units
        target_units = 'F'
        target_units_display = 'F'
      else
        target_units = source_units
        target_units_display = 'C'
      end
      if cooling_temp_ranges.empty?
        cooling_temp_ranges_pretty = "can't inspect schedules"
      else
        cooling_temp_ranges_pretty = "#{OpenStudio.convert(cooling_temp_ranges.min, source_units, target_units).get.round(1)} to #{OpenStudio.convert(cooling_temp_ranges.max, source_units, target_units).get.round(1)}"
      end
      if heating_temps_ranges.empty?
        heating_temps_ranges_pretty = "can't inspect schedules"
      else
        heating_temps_ranges_pretty = "#{OpenStudio.convert(heating_temps_ranges.min, source_units, target_units).get.round(1)} to #{OpenStudio.convert(heating_temps_ranges.max, source_units, target_units).get.round(1)}"
      end
      output_data_air_loops[:data] << ['Thermal Zones', 'Cooling Setpoint Range', "#{cooling_temp_ranges_pretty} #{target_units_display}", '', '']
      output_data_air_loops[:data] << ['Thermal Zones', 'Heating Setpoint Range', "#{heating_temps_ranges_pretty} #{target_units_display}", '', '']
      output_data_air_loops[:data] << ['Terminal Types Used', terminals.uniq.sort.join(', '), '', '', terminals.size]

      # controls summary
      # julien
      source_units = 'C'
      if is_ip_units
        target_units = 'F'
        target_units_display = 'F'
      else
        target_units = source_units
        target_units_display = 'C'
      end
      output_data_air_loops[:data] << [{ sub_header: 'controls' }, '', '', '', '']
      output_data_air_loops[:data] << ['HVAC Operation Schedule', '', air_loop.availabilitySchedule.name, '', ''] # I think this is a bool
      output_data_air_loops[:data] << ['Night Cycle Setting', '', air_loop.nightCycleControlType, '', '']
      output_data_air_loops[:data] << ['Economizer Setting', '', economizer_setting, '', '']
      output_data_air_loops[:data] << ['Demand Controlled Ventilation Status', '', dcv_setting, '', '']
      htg_sat_si = air_loop.sizingSystem.centralHeatingDesignSupplyAirTemperature
      htg_sat_ip = OpenStudio.toNeatString(OpenStudio.convert(htg_sat_si, source_units, target_units).get, 1, true)
      output_data_air_loops[:data] << ['Central Heating Design Supply Air Temperature', '', "#{htg_sat_ip} #{target_units_display}", '', '']
      clg_sat_si = air_loop.sizingSystem.centralCoolingDesignSupplyAirTemperature
      clg_sat_ip = OpenStudio.toNeatString(OpenStudio.convert(clg_sat_si, source_units, target_units).get, 1, true)
      output_data_air_loops[:data] << ['Central Cooling Design Supply Air Temperature', '', "#{clg_sat_ip} #{target_units_display}", '', '']
      output_data_air_loops[:data] << ['Load to Size On', '', air_loop.sizingSystem.typeofLoadtoSizeOn, '', '']

      # populate tables for section
      output_data_air_loop_tables << output_data_air_loops
    end

    return @output_data_air_loop_section
  end

  # summary of what to show for each type of plant loop component
  def self.plant_loop_component_summary_logic(component, model, is_ip_units)
    # Generic component logic first
    data_arrays = general_component_summary_logic(component, is_ip_units)

    # Motor component logic
    data_arrays += motor_component_logic(component, is_ip_units)

    # Setpoint manager logic
    data_arrays += spm_logic(component, is_ip_units)

    # Make the component type the first element of the first row
    if !data_arrays.empty?
      data_arrays[0][0] = component.iddObject.name.gsub('OS:', '')
    end

    return data_arrays
  end

  # create table plant loop summary
  def self.plant_loops_detail_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    output_data_plant_loop_tables = []

    # gather data for section
    @output_data_plant_loop_section = {}
    @output_data_plant_loop_section[:title] = 'Plant Loops Detail'
    @output_data_plant_loop_section[:tables] = output_data_plant_loop_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @output_data_plant_loop_section
    end

    # loop through plant loops
    model.getPlantLoops.sort.each do |plant_loop|
      # plant loop data output
      output_data_plant_loops = {}
      output_data_plant_loops[:title] = plant_loop.name.get # TODO: - confirm first that it has name
      output_data_plant_loops[:header] = ['Object', 'Description', 'Value', 'Sizing', 'Count']
      output_data_plant_loops[:units] = [] # not using units for these tables
      output_data_plant_loops[:data] = []

      output_data_plant_loops[:data] << [{ sub_header: 'supply' }, '', '', '', '']

      plant_loop.supplyComponents.each do |component|
        if component.to_ThermalZone.is_initialized
        end
        # skip some object types
        next if component.to_PipeAdiabatic.is_initialized
        next if component.to_Splitter.is_initialized
        next if component.to_Mixer.is_initialized
        if component.to_Node.is_initialized
          setpoint_managers = component.to_Node.get.setpointManagers
          if !setpoint_managers.empty?
            # setpoint type
            setpoint = setpoint_managers[0] # TODO: - could have more than one in some situations
            data_arrays = OsLib_Reporting.plant_loop_component_summary_logic(setpoint, model, is_ip_units)
            data_arrays.each do |data_array| # typically just one, but in some cases there are a few
              output_data_plant_loops[:data] << data_array
            end
          end
        else
          # populate table for everything but setpoint managers, which are added above.
          data_arrays = OsLib_Reporting.plant_loop_component_summary_logic(component, model, is_ip_units)
          data_arrays.each do |data_array| # typically just one, but in some cases there are a few
            output_data_plant_loops[:data] << data_array
          end
        end
      end

      # loop through demand components
      output_data_plant_loops[:data] << [{ sub_header: 'demand' }, '', '', '', '']

      # keep track of terminal count to report later
      terminal_connections = [] # Not sure how I want to list in display

      # loop through plant demand components
      plant_loop.demandComponents.each do |component|
        # flag for terminal connecxtions
        terminal_connection = false

        # skip some object types
        next if component.to_PipeAdiabatic.is_initialized
        next if component.to_Splitter.is_initialized
        next if component.to_Mixer.is_initialized
        next if component.to_Node.is_initialized

        # determine if water to air
        if component.to_WaterToAirComponent.is_initialized
          component = component.to_WaterToAirComponent.get
          if component.airLoopHVAC.is_initialized
            description = 'Air Loop'
            value = component.airLoopHVAC.get.name
          else
            # this is a terminal connection
            terminal_connection = true
            terminal_connections << component
          end
        elsif component.to_WaterToWaterComponent.is_initialized
          description = 'Plant Loop'
          component = component.to_WaterToWaterComponent.get
          ww_loop = component.plantLoop
          if ww_loop.is_initialized
            value = ww_loop.get.name
          else
            value = ''
          end
        else # water use connections would go here
          description = component.name
          value = ''
        end

        # don't report here if this component is connected to a terminal
        next if terminal_connection == true

        output_data_plant_loops[:data] << [component.iddObject.name.gsub('OS:', ''), description, value, '', '']
      end

      # report terminal connections
      if !terminal_connections.empty?
        output_data_plant_loops[:data] << ['Air Terminal Connections', '', '', '', terminal_connections.size]
      end

      output_data_plant_loops[:data] << [{ sub_header: 'controls' }, '', '', '', '']

      # Min loop flow rate
      source_units = 'm^3/s'
      if is_ip_units
        target_units = 'gal/min'
        n_decimals = 2
      else
        target_units = 'm^3/h'
        n_decimals = 0
      end
      if plant_loop.minimumLoopFlowRate.is_initialized
        value = OpenStudio.convert(plant_loop.minimumLoopFlowRate.get, source_units, target_units).get
        value_neat = OpenStudio.toNeatString(value, n_decimals, true)
        siz = 'Hard Sized'
      elsif plant_loop.autosizedMinimumLoopFlowRate.is_initialized
        value = OpenStudio.convert(plant_loop.autosizedMinimumLoopFlowRate.get, source_units, target_units).get
        value_neat = OpenStudio.toNeatString(value, n_decimals, true)
        siz = 'Autosized'
      else
        value_neat = 'Autosized'
      end
      output_data_plant_loops[:data] << ['Loop Flow Rate Range', 'Minimum Loop Flow Rate', "#{value_neat} #{target_units}", siz, '']

      # Max loop flow rate
      if plant_loop.maximumLoopFlowRate.is_initialized
        value = OpenStudio.convert(plant_loop.maximumLoopFlowRate.get, source_units, target_units).get
        value_neat = OpenStudio.toNeatString(value, n_decimals, true)
        siz = 'Hard Sized'
      elsif
        plant_loop.autosizedMaximumLoopFlowRate.is_initialized
        value = OpenStudio.convert(plant_loop.autosizedMaximumLoopFlowRate.get, source_units, target_units).get
        value_neat = OpenStudio.toNeatString(value, n_decimals, true)
        siz = 'Autosized'
      else
        value_neat = 'Autosized'
      end
      output_data_plant_loops[:data] << ['Loop Flow Rate Range', 'Maximum Loop Flow Rate', "#{value_neat} #{target_units}", siz, '']

      # loop temperatures
      source_units = 'C'
      if is_ip_units
        target_units = 'F'
      else
        target_units = 'C'
      end
      min_temp = plant_loop.minimumLoopTemperature
      max_temp = plant_loop.maximumLoopTemperature
      value_neat = "#{OpenStudio.convert(min_temp, source_units, target_units).get.round(1)} to #{OpenStudio.convert(max_temp, source_units, target_units).get.round(1)}"
      output_data_plant_loops[:data] << ['Loop Temperature Range', '', "#{value_neat} #{target_units}", '', '']

      # get values out of sizing plant
      sizing_plant = plant_loop.sizingPlant
      source_units = 'C'
      if is_ip_units
        target_units = 'F'
      else
        target_units = 'C'
      end
      loop_exit_temp = sizing_plant.designLoopExitTemperature
      value_neat = OpenStudio.toNeatString(OpenStudio.convert(loop_exit_temp, source_units, target_units).get, 1, true)

      output_data_plant_loops[:data] << ['Loop Design Exit Temperature', '', "#{value_neat} #{target_units}", '', '']
      source_units = 'K'
      if is_ip_units
        target_units = 'R'
      else
        target_units = 'K'
      end
      loop_design_temp_diff = sizing_plant.loopDesignTemperatureDifference
      value_neat = OpenStudio.toNeatString(OpenStudio.convert(loop_design_temp_diff, source_units, target_units).get, 1, true)
      output_data_plant_loops[:data] << ['Loop Design Temperature Difference', '', "#{value_neat} #{target_units}", '', '']

      # Equipment staging
      output_data_plant_loops[:data] << ['Equipment Loading/Staging', '', plant_loop.loadDistributionScheme, '', '']

      # push tables
      output_data_plant_loop_tables << output_data_plant_loops
    end

    return @output_data_plant_loop_section
  end

  # summary of what to show for each type of zone equipment component
  def self.zone_equipment_component_summary_logic(component, model, is_ip_units)
    # Generic component logic first
    data_arrays = general_component_summary_logic(component, is_ip_units)

    # Motor component logic
    data_arrays += motor_component_logic(component, is_ip_units)

    return data_arrays
  end

  # create table plant loop summary
  def self.zone_equipment_detail_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    output_data_zone_equipment = []

    # gather data for section
    @output_data_zone_equipment_section = {}
    @output_data_zone_equipment_section[:title] = 'Zone Equipment Detail'
    @output_data_zone_equipment_section[:tables] = output_data_zone_equipment

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @output_data_zone_equipment_section
    end

    # loop through thermal zones
    model.getThermalZones.sort.each do |zone|
      # plant loop data output
      output_data_zone_equipment = {}
      output_data_zone_equipment[:title] = zone.name.get # TODO: - confirm that zone has a name
      output_data_zone_equipment[:header] = ['Object', 'Description', 'Value', 'Sizing', 'Count']
      output_data_zone_equipment[:units] = [] # not using units for these tables
      output_data_zone_equipment[:data] = []

      zone.equipment.sort.each do |zone_equip|
        next unless zone_equip.to_ZoneHVACComponent.is_initialized # skip any terminals
        data_arrays = OsLib_Reporting.zone_equipment_component_summary_logic(zone_equip, model, is_ip_units)
        data_arrays.each do |data_array| # typically just one, but in some cases there are a few
          output_data_zone_equipment[:data] << data_array
        end

        # Make the component type the first element of the first row
        if !data_arrays.empty?
          data_arrays[0][0] = zone_equip.iddObject.name.gsub('OS:', '')
        end
      end

      # push table to array
      if !output_data_zone_equipment[:data].empty?
        @output_data_zone_equipment_section[:tables] << output_data_zone_equipment
      end
    end

    return @output_data_zone_equipment_section
  end

  # create table for constructions
  def self.envelope_section_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # Versions of OpenStudio greater than 2.4.0 use a modified version of
    # openstudio-standards with different method calls.  These methods
    # require a "Standard" object instead of the standard being passed into method calls.
    # This Standard object is used throughout the QAQC check.
    if OpenStudio::VersionString.new(OpenStudio.openStudioVersion) < OpenStudio::VersionString.new('2.4.2')
      use_old_gem_code = true
    else
      use_old_gem_code = false
      std = Standard.build('90.1-2013') # actual standard doesn't matter in this case
    end

    # array to hold tables
    envelope_tables = []

    # gather data for section
    @envelope_section = {}
    @envelope_section[:title] = 'Envelope Summary'
    @envelope_section[:tables] = envelope_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @envelope_section
    end

    # summary of exterior constructions used in the model for base surfaces
    surface_data = {}
    surface_data[:title] = 'Base Surface Constructions'
    surface_data[:header] = ['Construction', 'Net Area', 'Surface Count', 'R Value']

    source_area_units = 'm^2'
    source_rvalue_units = 'm^2*K/W'
    source_ufactor_units = 'W/m^2*K'

    if is_ip_units
      target_area_units = 'ft^2'
      target_rvalue_units = 'ft^2*h*R/Btu'
      target_ufactor_units = 'Btu/ft^2*h*R'
      n_decimals_area = 0
      n_decimals_rvalue = 2
    else
      target_area_units = source_area_units
      target_rvalue_units = source_rvalue_units
      target_ufactor_units = source_ufactor_units
      n_decimals_area = 1
      n_decimals_rvalue = 2
    end

    surface_data[:units] = ['', target_area_units, '', target_rvalue_units]
    surface_data[:data] = []

    # construction details
    # TODO
    # add table with subheads for each construction, and rows for each material layer
    # construction_details = {}
    # construction_details[:title] = 'Construction Details'
    # construction_details[:header] = ['Material']
    # construction_details[:data] = []

    # loop through surfaces to get constructions
    ext_const_base = {}
    model.getSurfaces.each do |surface|
      next if surface.outsideBoundaryCondition != 'Outdoors'
      if ext_const_base.include? surface.construction.get
        ext_const_base[surface.construction.get] += 1
      else
        ext_const_base[surface.construction.get] = 1
      end
    end
    ext_const_base.sort.each do |construction, count|
      net_area = construction.getNetArea
      net_area_conv = OpenStudio.convert(net_area, source_area_units, target_area_units).get
      net_area_neat = OpenStudio.toNeatString(net_area_conv, n_decimals_area, true)
      surface_count = count
      if construction.thermalConductance.is_initialized
        thermal_conductance = construction.thermalConductance.get
        r_value = OpenStudio.convert(1 / thermal_conductance, source_rvalue_units, target_rvalue_units).get
        r_value_neat = OpenStudio.toNeatString(r_value, n_decimals_rvalue, true)
      else
        r_value_neat = ''
      end
      surface_data[:data] << [construction.name, net_area_neat, surface_count, r_value_neat]
      runner.registerValue(OsLib_Reporting.reg_val_string_prep(construction.name.to_s), net_area_conv, target_area_units)

      # TODO: Get this working like subsurfaces
      # construction_details[:data] << [{ sub_header: "Material Layers in Construction '#{construction.name}':"}]
      # construction.layers.each do |l|
      #  construction_details[:data] << [l.name]
      # end
    end
    envelope_tables << surface_data

    # summary of exterior constructions used in the model for sub surfaces
    sub_surface_data = {}
    sub_surface_data[:title] = 'Sub Surface Constructions'
    sub_surface_data[:header] = ['Construction', 'Net Area', 'Surface Count', 'U-factor', 'SHGC', 'VLT']

    sub_surface_data[:units] = ['', target_area_units, '', target_ufactor_units]
    sub_surface_data[:data] = []

    construction_details = {}
    construction_details[:title] = 'Sub Surface Construction Details (Material Layers)'
    construction_details[:header] = ['Material Name']
    construction_details[:data] = []

    # loop through sub_surfaces to get constructions
    ext_const_sub = {}
    model.getSubSurfaces.each do |sub_surface|
      next if sub_surface.outsideBoundaryCondition != 'Outdoors'
      if ext_const_sub.include? sub_surface.construction.get
        ext_const_sub[sub_surface.construction.get] += 1
      else
        ext_const_sub[sub_surface.construction.get] = 1
      end
    end
    ext_const_sub.sort.each do |construction, count|
      net_area = construction.getNetArea
      net_area_conv = OpenStudio.convert(net_area, source_area_units, target_area_units).get
      net_area_neat = OpenStudio.toNeatString(net_area_conv, n_decimals_area, true)
      surface_count = count
      vlt_neat = 'n/a'
      shgc_neat = 'n/a'
      u_factor_neat = ''
      if construction.to_Construction.is_initialized
        construction_root = construction.to_Construction.get
        if construction_root.isFenestration
          if use_old_gem_code
            shgc = construction_root.calculated_solar_heat_gain_coefficient
          else
            shgc = std.construction_calculated_solar_heat_gain_coefficient(construction_root)
          end
          shgc_neat = OpenStudio.toNeatString(shgc, n_decimals_rvalue, false)
          if use_old_gem_code
            vlt = construction_root.calculated_visible_transmittance
          else
            vlt = std.construction_calculated_visible_transmittance(construction_root)
          end
          vlt_neat = OpenStudio.toNeatString(vlt, n_decimals_rvalue, false)
          if use_old_gem_code
            u_factor = construction_root.calculated_u_factor
          else
            u_factor = std.construction_calculated_u_factor(construction_root)
          end
          ufactor_conv = OpenStudio.convert(u_factor, source_ufactor_units, target_ufactor_units).get
          ufactor_neat = OpenStudio.toNeatString(ufactor_conv, n_decimals_rvalue, false)
        else
          u_factor = construction.thermalConductance.get
          u_factor_conv = OpenStudio.convert(u_factor, source_ufactor_units, target_ufactor_units).get
          u_factor_neat = OpenStudio.toNeatString(u_factor_conv, n_decimals_rvalue, false)
        end
        # add layer details for each construction
        construction_details[:data] << [{ sub_header: "Material Layers in Construction '#{construction.name}':" }]
        construction_root.layers.each do |l|
          construction_details[:data] << [l.name]
        end

      end

      sub_surface_data[:data] << [construction.name, net_area_neat, surface_count, ufactor_neat, shgc_neat, vlt_neat]
      runner.registerValue(OsLib_Reporting.reg_val_string_prep(construction.name.to_s), net_area_conv, target_area_units)
    end
    envelope_tables << sub_surface_data
    envelope_tables << construction_details

    # Conditioned Window-Wall Ratio and Skylight-Roof Ratio
    fenestration_data = {}
    fenestration_data[:title] = 'Window-to-Wall and Skylight-to-Roof area Ratios'
    fenestration_data[:header] = ['Description', 'Total', 'North', 'East', 'South', 'West']
    target_units = '%' # ratios reported as percentages
    fenestration_data[:units] = ['', target_units, target_units, target_units, target_units, target_units]
    fenestration_data[:data] = []

    # create string for rows
    fenestrations = []
    fenestrations << 'Gross Window-Wall Ratio' # [%]

    # loop rows
    fenestrations.each do |fenestration|
      query0 = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='InputVerificationandResultsSummary' and TableName='Window-Wall Ratio' and RowName='#{fenestration}' and ColumnName='Total'"
      query1 = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='InputVerificationandResultsSummary' and TableName='Window-Wall Ratio' and RowName='#{fenestration}' and ColumnName='North (315 to 45 deg)'"
      query2 = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='InputVerificationandResultsSummary' and TableName='Window-Wall Ratio' and RowName='#{fenestration}' and ColumnName='East (45 to 135 deg)'"
      query3 = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='InputVerificationandResultsSummary' and TableName='Window-Wall Ratio' and RowName='#{fenestration}' and ColumnName='South (135 to 225 deg)'"
      query4 = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='InputVerificationandResultsSummary' and TableName='Window-Wall Ratio' and RowName='#{fenestration}' and ColumnName='West (225 to 315 deg)'"
      query5 = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='InputVerificationandResultsSummary' and TableName='Skylight-Roof Ratio'  and RowName='Skylight-Roof Ratio'"
      query6 = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='InputVerificationandResultsSummary' and TableName='Conditioned Window-Wall Ratio' and RowName='#{fenestration}' and ColumnName='Total'"
      query7 = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='InputVerificationandResultsSummary' and TableName='Conditioned Window-Wall Ratio' and RowName='#{fenestration}' and ColumnName='North (315 to 45 deg)'"
      query8 = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='InputVerificationandResultsSummary' and TableName='Conditioned Window-Wall Ratio' and RowName='#{fenestration}' and ColumnName='East (45 to 135 deg)'"
      query9 = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='InputVerificationandResultsSummary' and TableName='Conditioned Window-Wall Ratio' and RowName='#{fenestration}' and ColumnName='South (135 to 225 deg)'"
      query10 = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='InputVerificationandResultsSummary' and TableName='Conditioned Window-Wall Ratio' and RowName='#{fenestration}' and ColumnName='West (225 to 315 deg)'"

      total = sqlFile.execAndReturnFirstDouble(query0)
      north = sqlFile.execAndReturnFirstDouble(query1)
      east = sqlFile.execAndReturnFirstDouble(query2)
      south = sqlFile.execAndReturnFirstDouble(query3)
      west = sqlFile.execAndReturnFirstDouble(query4)
      skylight = sqlFile.execAndReturnFirstDouble(query5)
      total_cond = sqlFile.execAndReturnFirstDouble(query6)
      north_cond = sqlFile.execAndReturnFirstDouble(query7)
      east_cond = sqlFile.execAndReturnFirstDouble(query8)
      south_cond = sqlFile.execAndReturnFirstDouble(query9)
      west_cond = sqlFile.execAndReturnFirstDouble(query10)
      if total.empty? || north.empty? || east.empty? || south.empty? || west.empty? || total_cond.empty? || north_cond.empty? || east.empty? || south_cond.empty? || west_cond.empty? || skylight.empty?
        runner.registerWarning('Did not find value for Window or Skylight Ratio')
        return false
      else
        # add data
        display = fenestration
        fenestration_data[:data] << [display, total.get, north.get, east.get, south.get, west.get]
        fenestration_data[:data] << ["#{display} (Conditioned)", total_cond.get, north_cond.get, east_cond.get, south_cond.get, west_cond.get]
        runner.registerValue(OsLib_Reporting.reg_val_string_prep(display).to_s, total.get, target_units)
        runner.registerValue("#{OsLib_Reporting.reg_val_string_prep(display)}_conditioned", total_cond.get, target_units)

        # skylight
        # skylight seems to provide back percentage vs. fraction. Changing to fraction to match vertical fenestration.
        fenestration_data[:data] << ['Skylight-Roof Ratio', skylight.get, '', '', '', '']
        runner.registerValue('skylight_roof_ratio', skylight.get, target_units)

      end
    end

    envelope_tables << fenestration_data

    return @envelope_section
  end

  # create table for service water heating
  def self.water_use_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # water use equipment from model
    water_use_data = {}
    water_use_data[:title] = ''
    water_use_data[:header] = ['Instance', 'Plant Loop', 'Definition', 'Space', 'Peak Flow Rate', 'Flow Rate Schedule', 'Target Temp Range']

    source_units = 'm^3/s'
    source_units_temp = 'C'
    if is_ip_units
      target_units = 'gal/min'
      n_decimals = 2

      target_units_temp = 'F'
    else
      target_units = 'm^3/h'
      n_decimals = 2

      target_units_temp = 'C'
    end

    water_use_data[:units] = ['', '', '', '', target_units, '', target_units_temp]
    water_use_data[:data] = []

    # gather data for section
    @water_use_data_section = {}
    @water_use_data_section[:title] = 'Water Use Equipment'

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @water_use_data_section
    end

    # loop through water use equipment
    water_use_equipment = model.getWaterUseEquipments
    water_use_equipment.sort.each do |instance|
      water_use_equipment_def = instance.waterUseEquipmentDefinition
      if instance.waterUseConnections.is_initialized && instance.waterUseConnections.get.plantLoop.is_initialized
        plant_loop = instance.waterUseConnections.get.plantLoop.get.name
      else
        plant_loop = ''
      end
      if instance.flowRateFractionSchedule.is_initialized
        water_use_equipment_flow_rate_sch = instance.flowRateFractionSchedule.get.name
      else
        water_use_equipment_flow_rate_sch = ''
      end
      if instance.space.is_initialized
        space = instance.space.get.name
      else
        space = ''
      end
      peak_flow_rate = water_use_equipment_def.peakFlowRate
      peak_flow_rate_conv = OpenStudio.convert(peak_flow_rate, source_units, target_units).get
      peak_flow_rate_neat = OpenStudio.toNeatString(peak_flow_rate_conv, n_decimals, true)
      if water_use_equipment_def.targetTemperatureSchedule.is_initialized
        target_temp_sch = water_use_equipment_def.targetTemperatureSchedule.get
        schedule_values = OsLib_Schedules.getMinMaxAnnualProfileValue(model, target_temp_sch)
        if !schedule_values.nil?
          min_conv = OpenStudio.convert(schedule_values['min'], source_units_temp, target_units_temp).get
          max_conv = OpenStudio.convert(schedule_values['max'], source_units_temp, target_units_temp).get
          target_temp_range = "#{min_conv.round(1)} to #{max_conv.round(1)}"
        else
          target_temp_range = "can't inspect schedule."
        end
      else
        target_temp_range = ''
      end
      water_use_data[:data] << [instance.name, plant_loop, water_use_equipment_def.name, space, peak_flow_rate_neat, water_use_equipment_flow_rate_sch, target_temp_range]
      runner.registerValue(OsLib_Reporting.reg_val_string_prep(instance.name.to_s), peak_flow_rate_conv, target_units)
    end

    # don't create empty table
    if !water_use_data[:data].empty?
      @water_use_data_section[:tables] = [water_use_data] # only one table for this section
    else
      @water_use_data_section[:tables] = []
    end

    return @water_use_data_section
  end

  # create table for exterior lights
  def self.exterior_light_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # Exterior Lighting from output

    # gather data for section
    @ext_light_data_section = {}
    @ext_light_data_section[:title] = 'Exterior Lighting'

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @ext_light_data_section
    end

    # data for query
    report_name = 'LightingSummary'
    table_name = 'Exterior Lighting'
    columns = ['Description', 'Total Power', 'Astronomical', 'Schedule Name', 'Annual Consumption']
    columns_query = ['', 'Total Watts', 'Astronomical Clock/Schedule', 'Schedule Name', 'Consumption']

    # populate dynamic rows
    rows_name_query = "SELECT DISTINCT  RowName FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}'"
    row_names = sqlFile.execAndReturnVectorOfString(rows_name_query).get
    rows = []
    row_names.each do |row_name|
      rows << row_name
    end

    # Zone-level Lighting Summary
    table = {}
    table[:title] = 'Exterior Lighting'
    table[:header] = columns
    table[:source_units] = ['', 'W', '', '', 'GJ'] # used for conversion, not needed for rendering.
    table[:units] = ['', 'W', '', '', 'kWh']
    table[:data] = []

    # run query and populate table
    rows.each do |row|
      row_data = [row]
      column_counter = -1
      columns_query.each do |header|
        column_counter += 1
        next if header == ''
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row}' and ColumnName= '#{header}'"
        if table[:source_units][column_counter] != ''
          results = sqlFile.execAndReturnFirstDouble(query)
          row_data_ip = OpenStudio.convert(results.to_f, table[:source_units][column_counter], table[:units][column_counter]).get
          row_data << row_data_ip.round(2)
        else
          results = sqlFile.execAndReturnFirstString(query)
          row_data << results
        end
      end

      table[:data] << row_data
    end

    # don't create empty table
    if table[:data].size > 1 # did > 1 instead of > 0 because I thought empty one might still have one row
      @ext_light_data_section[:tables] = [table] # only one table for this section
    else
      @ext_light_data_section[:tables] = []
    end

    return @ext_light_data_section
  end

  # create table for elevators
  # TODO: - update this to be custom load with user supplied string (or strings)
  def self.elevator_data_table(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # elevators from model
    @elevator_data = {}
    @elevator_data[:title] = 'Elevators'
    @elevator_data[:header] = ['Instance', 'Definition', 'Thermal Zone', 'Power Per Elevator', 'Units', 'Count']
    @elevator_data[:data] = []
    elec_equip_instances = model.getElectricEquipments
    elec_equip_instances.sort.each do |instance|
      elec_equip_def = instance.electricEquipmentDefinition

      # see if it is expected and valid object
      next if elec_equip_def.name.to_s != 'ElevatorElecEquipDef'
      unless instance.space.is_initialized
        runner.registerWarning("#{instance.name} doesn't have a space.")
        next
      end

      # get other data
      elev_space = instance.space.get
      elev_zone = elev_space.thermalZone.get # should check this
      elev_power = elec_equip_def.designLevel.get # should check this
      elev_power_neat = OpenStudio.toNeatString(elev_power, 0, true)
      units = 'W'
      count = instance.multiplier

      @elevator_data[:data] << [instance.name.to_s, elec_equip_def.name, elev_zone.name.get, elev_power_neat, units, OpenStudio.toNeatString(count, 2, true)]
      runner.registerValue(OsLib_Reporting.reg_val_string_prep(instance.name.to_s), elev_power, units)
    end

    return @elevator_data
  end

  # create table of space type details
  def self.space_type_details_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    output_data_space_type_detail_tables = []

    # gather data for section
    @output_data_space_type_section = {}
    @output_data_space_type_section[:title] = 'Space Type Summary'
    @output_data_space_type_section[:tables] = output_data_space_type_detail_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @output_data_space_type_section
    end

    source_units_surf = 'm^2'
    source_units_temp = 'C'
    source_units_FlowRate = 'm^3/s'
    source_units_flowperSpaceFloorArea = 'm/s'

    if is_ip_units
      target_units_surf = 'ft^2'
      target_units_temp = 'F'
      target_units_FlowRate = 'ft^3/min'
      def_units_powerPerFloorArea = 'W/ft^2'
      inst_units_FlowRate = 'cfm'
      target_units_flowperSpaceFloorArea = 'ft/min'
      inst_units_outdoorAirFlowperPerson = 'cfm/person'
      inst_units_flowperSpaceFloorArea = 'cfm/ floor area ft^2'
      inst_units_flowperExteriorSurfaceArea = 'cfm/ext surf area ft^2'
      inst_units_flowperExteriorWallArea = 'cfm/ext wall area ft^2'
      def_units_surfaceAreaPerFloorArea = 'ft^2/floor area ft^2'
      def_units_surfaceAreaPerPerson = 'ft^2/person'
      def_units_peoplePerFloorArea = 'people/ft^2'
    else
      target_units_surf = source_units_surf
      target_units_temp = source_units_temp
      target_units_FlowRate = 'm^3/h'
      def_units_powerPerFloorArea = 'W/m^2'
      inst_units_FlowRate = 'm^3/h'
      target_units_flowperSpaceFloorArea = 'm/s'
      inst_units_outdoorAirFlowperPerson = 'm^3/h/person'
      inst_units_flowperSpaceFloorArea = 'm^3/h/ floor area m^2'
      inst_units_flowperExteriorSurfaceArea = 'm^3/h/ext surf area m^2'
      inst_units_flowperExteriorWallArea = 'm^3/h/ext wall area m^2'
      def_units_surfaceAreaPerFloorArea = 'm^2/floor area m^2'
      def_units_surfaceAreaPerPerson = 'm^2/person'
      def_units_peoplePerFloorArea = 'people/m^2'
    end

    # loop through space types
    model.getSpaceTypes.sort.each do |spaceType|
      next if spaceType.floorArea == 0

      # get floor area
      floor_area_si = spaceType.floorArea

      # create variable for number of people
      num_people = nil

      # gather list of spaces and zones in space type
      zone_name_list = []
      space_name_list = []
      spaceType.spaces.each do |space|
        # grabspace and zone names
        space_name_list << space.name.to_s
        if space.thermalZone.is_initialized
          zone_name_list << space.thermalZone.get.name.to_s
        end
      end
      # output_data_space_type_details[:data] << [space_name_list.uniq.join(","),space_name_list.uniq.size,"Spaces",""]
      # output_data_space_type_details[:data] << [zone_name_list.uniq.join(","),zone_name_list.uniq.size,"Thermal Zones",""]

      # space type details data output
      output_data_space_type_details = {}
      output_data_space_type_details[:title] = "#{spaceType.name}<br>(#{space_name_list.uniq.size} spaces and #{zone_name_list.uniq.size} thermal zones)"
      output_data_space_type_details[:header] = ['Definition', 'Value', 'Unit', 'Inst. Multiplier']
      output_data_space_type_details[:units] = [] # won't use this for these tables since units change
      output_data_space_type_details[:data] = []

      # data for space type details
      instances = spaceType.internalMass
      instances.each do |instance|
        def_display = instance.definition.name
        if instance.surfaceArea.is_initialized && instance.surfaceArea.get > 0
          def_value = OpenStudio.convert(instance.surfaceArea.get, source_units_surf, target_units_surf).get
          def_value_neat = OpenStudio.toNeatString(def_value, 0, true)
          def_units = target_units_surf
        elsif instance.surfaceAreaPerFloorArea.is_initialized && instance.surfaceAreaPerFloorArea.get > 0
          def_value = instance.surfaceAreaPerFloorArea.get
          def_value_neat = OpenStudio.toNeatString(def_value, 0, true)
          def_units = def_units_surfaceAreaPerFloorArea
        elsif instance.surfaceAreaPerPerson.is_initialized && instance.surfaceAreaPerPerson.get > 0
          def_value = OpenStudio.convert(instance.surfaceAreaPerPerson.get, source_units_surf, target_units_surf).get
          def_value_neat = OpenStudio.toNeatString(def_value, 0, true)
          def_units = def_units_surfaceAreaPerPerson
        end
        count = instance.multiplier
        output_data_space_type_details[:data] << [def_display, def_value_neat, def_units, count]
      end

      instances = spaceType.people
      instances.each do |instance|
        def_display = instance.definition.name
        if instance.numberOfPeople.is_initialized && instance.numberOfPeople.get > 0
          def_value = instance.numberOfPeople.get
          def_value_neat = OpenStudio.toNeatString(def_value, 0, true)
          def_units = 'people'
        elsif instance.peoplePerFloorArea.is_initialized && instance.peoplePerFloorArea.get > 0
          def_value = instance.peoplePerFloorArea.get / OpenStudio.convert(1, source_units_surf, target_units_surf).get
          def_value_neat = OpenStudio.toNeatString(def_value, 4, true)
          def_units = def_units_peoplePerFloorArea
        elsif instance.spaceFloorAreaPerPerson.is_initialized && instance.spaceFloorAreaPerPerson.get > 0
          def_value = OpenStudio.convert(instance.spaceFloorAreaPerPerson.get, source_units_surf, target_units_surf).get
          def_value_neat = OpenStudio.toNeatString(def_value, 0, true)
          def_units = def_units_surfaceAreaPerPerson
        end
        count = instance.multiplier
        output_data_space_type_details[:data] << [def_display, def_value_neat, def_units, count]
      end

      instances = spaceType.electricEquipment
      instances.each do |instance|
        def_display = instance.definition.name
        if instance.designLevel.is_initialized && instance.designLevel.get > 0
          def_value = instance.designLevel.get
          def_value_neat = OpenStudio.toNeatString(def_value, 0, true)
          def_units = 'W'
        elsif instance.powerPerFloorArea.is_initialized && instance.powerPerFloorArea.get > 0
          def_value = instance.powerPerFloorArea.get / OpenStudio.convert(1, source_units_surf, target_units_surf).get
          def_value_neat = OpenStudio.toNeatString(def_value, 4, true)
          def_units = def_units_powerPerFloorArea
        elsif instance.powerPerPerson .is_initialized && instance.powerPerPerson .get > 0
          def_value = OpenStudio.convert(instance.powerPerPerson .get, source_units_surf, target_units_surf).get
          def_value_neat = OpenStudio.toNeatString(def_value, 0, true)
          def_units = 'W/person'
        end
        count = instance.multiplier
        output_data_space_type_details[:data] << [def_display, def_value_neat, def_units, count]
      end

      instances = spaceType.gasEquipment
      instances.each do |instance|
        def_display = instance.definition.name
        if instance.designLevel.is_initialized && instance.designLevel.get > 0
          def_value = instance.designLevel.get
          def_value_neat = OpenStudio.toNeatString(def_value, 0, true)
          def_units = 'W'
        elsif instance.powerPerFloorArea.is_initialized && instance.powerPerFloorArea.get > 0
          def_value = instance.powerPerFloorArea.get / OpenStudio.convert(1, source_units_surf, target_units_surf).get
          def_value_neat = OpenStudio.toNeatString(def_value, 4, true)
          def_units = def_units_powerPerFloorArea
        elsif instance.powerPerPerson .is_initialized && instance.powerPerPerson .get > 0
          def_value = OpenStudio.convert(instance.powerPerPerson .get, source_units_surf, target_units_surf).get
          def_value_neat = OpenStudio.toNeatString(def_value, 0, true)
          def_units = 'W/person'
        end
        count = instance.multiplier
        output_data_space_type_details[:data] << [def_display, def_value_neat, def_units, count]
      end

      instances = spaceType.lights
      instances.each do |instance|
        def_display = instance.definition.name
        if instance.lightingLevel.is_initialized && instance.lightingLevel.get > 0
          def_value = instance.lightingLevel.get
          def_value_neat = OpenStudio.toNeatString(def_value, 0, true)
          def_units = 'W'
        elsif instance.powerPerFloorArea.is_initialized && instance.powerPerFloorArea.get > 0
          def_value = instance.powerPerFloorArea.get / OpenStudio.convert(1, source_units_surf, target_units_surf).get
          def_value_neat = OpenStudio.toNeatString(def_value, 4, true)
          def_units = def_units_powerPerFloorArea
        elsif instance.powerPerPerson .is_initialized && instance.powerPerPerson .get > 0
          def_value = OpenStudio.convert(instance.powerPerPerson .get, source_units_surf, target_units_surf).get
          def_value_neat = OpenStudio.toNeatString(def_value, 0, true)
          def_units = 'W/person'
        end
        count = instance.multiplier
        output_data_space_type_details[:data] << [def_display, def_value_neat, def_units, count]
      end

      instances = spaceType.spaceInfiltrationDesignFlowRates
      instances.each do |instance|
        instance_display = instance.name
        if instance.designFlowRate.is_initialized
          inst_value = OpenStudio.convert(instance.designFlowRate.get, source_units_FlowRate, target_units_FlowRate).get
          inst_value_neat = OpenStudio.toNeatString(inst_value, 4, true)
          inst_units = inst_units_FlowRate
          count = ''
          output_data_space_type_details[:data] << [instance_display, inst_value_neat, inst_units, count]
        end
        if instance.flowperSpaceFloorArea.is_initialized
          inst_value = OpenStudio.convert(instance.flowperSpaceFloorArea.get, source_units_flowperSpaceFloorArea, target_units_flowperSpaceFloorArea).get
          inst_value_neat = OpenStudio.toNeatString(inst_value, 4, true)
          inst_units = inst_units_flowperSpaceFloorArea
          count = ''
          output_data_space_type_details[:data] << [instance_display, inst_value_neat, inst_units, count]
        end
        if instance.flowperExteriorSurfaceArea.is_initialized
          inst_value = OpenStudio.convert(instance.flowperExteriorSurfaceArea.get, source_units_flowperSpaceFloorArea, target_units_flowperSpaceFloorArea).get
          inst_value_neat = OpenStudio.toNeatString(inst_value, 4, true)
          inst_units = inst_units_flowperExteriorSurfaceArea
          count = ''
          output_data_space_type_details[:data] << [instance_display, inst_value_neat, inst_units, count]
        end
        if instance.flowperExteriorWallArea.is_initialized # uses same input as exterior surface area but different calc method
          inst_value = OpenStudio.convert(instance.flowperExteriorWallArea.get, source_units_flowperSpaceFloorArea, target_units_flowperSpaceFloorArea).get
          inst_value_neat = OpenStudio.toNeatString(inst_value, 4, true)
          inst_units = inst_units_flowperExteriorWallArea
          count = ''
          output_data_space_type_details[:data] << [instance_display, inst_value_neat, inst_units, count]
        end
        if instance.airChangesperHour.is_initialized
          inst_value = instance.airChangesperHour.get
          inst_value_neat = OpenStudio.toNeatString(inst_value, 4, true)
          inst_units = 'ach'
          count = ''
          output_data_space_type_details[:data] << [instance_display, inst_value_neat, inst_units, count]
        end
      end

      if spaceType.designSpecificationOutdoorAir.is_initialized
        instance = spaceType.designSpecificationOutdoorAir.get
        instance_display = instance.name
        if instance.to_DesignSpecificationOutdoorAir.is_initialized
          instance = instance.to_DesignSpecificationOutdoorAir.get
          outdoor_air_method = instance.outdoorAirMethod
          count = ''

          # calculate and report various methods
          if instance.outdoorAirFlowperPerson > 0
            inst_value = OpenStudio.convert(instance.outdoorAirFlowperPerson, source_units_FlowRate, target_units_FlowRate).get
            inst_value_neat = OpenStudio.toNeatString(inst_value, 4, true)
            inst_units = inst_units_outdoorAirFlowperPerson
            output_data_space_type_details[:data] << ["#{instance_display} (outdoor air method #{outdoor_air_method})", inst_value_neat, inst_units, count]
          end
          if instance.outdoorAirFlowperFloorArea > 0
            inst_value = OpenStudio.convert(instance.outdoorAirFlowperFloorArea, source_units_flowperSpaceFloorArea, target_units_flowperSpaceFloorArea).get
            inst_value_neat = OpenStudio.toNeatString(inst_value, 4, true)
            inst_units = inst_units_flowperSpaceFloorArea
            output_data_space_type_details[:data] << ["#{instance_display} (outdoor air method #{outdoor_air_method})", inst_value_neat, inst_units, count]
          end
          if instance.outdoorAirFlowRate > 0
            inst_value = OpenStudio.convert(instance.outdoorAirFlowRate, source_units_FlowRate, target_units_FlowRate).get
            inst_value_neat = OpenStudio.toNeatString(inst_value, 4, true)
            inst_units = inst_units_FlowRate
            output_data_space_type_details[:data] << ["#{instance_display} (outdoor air method #{outdoor_air_method})", inst_value_neat, inst_units, count]
          end
          if instance.outdoorAirFlowAirChangesperHour > 0
            inst_value = instance.outdoorAirFlowAirChangesperHour
            inst_value_neat = OpenStudio.toNeatString(inst_value, 4, true)
            inst_units = 'ach'
            output_data_space_type_details[:data] << ["#{instance_display} (outdoor air method #{outdoor_air_method})", inst_value_neat, inst_units, count]
          end

        end
      end

      # add table to array of tables
      output_data_space_type_detail_tables << output_data_space_type_details
    end

    return @output_data_space_type_section
  end

  # create template section
  def self.weather_summary_table(model, sqlFile, runner, is_ip_units)
    # data for query
    report_name = 'InputVerificationandResultsSummary'
    table_name = 'General'
    columns = ['', 'Value']
    rows = ['Weather File', 'Latitude', 'Longitude', 'Elevation', 'Time Zone', 'North Axis Angle']

    # create table
    table = {}
    table[:title] = 'Weather Summary'
    table[:header] = columns
    table[:units] = []
    table[:data] = []

    source_units = 'm'
    if is_ip_units
      target_units = 'ft'
    else
      target_units = 'm'
    end

    # run query and populate table
    rows.each do |row|
      row_data = [row]
      column_counter = -1
      table[:header].each do |header|
        column_counter += 1
        next if header == ''
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row}' and ColumnName= '#{header}'"
        results = sqlFile.execAndReturnFirstString(query) # this is first time I needed string vs. double for weather file
        # TODO: - would be nice to get units from first column
        if row == 'Elevation' then results = "#{OpenStudio.convert(results.get.to_f, source_units, target_units).get.round} #{target_units}" end
        # if row == "Elevation" then results = "#{results.class} (f)" end
        row_data << results
      end

      table[:data] << row_data
    end

    # add in climate zone from OpenStudio model
    # get ashrae climate zone from model
    climate_zone = ''
    climateZones = model.getClimateZones
    climateZones.climateZones.each do |climateZone|
      if climateZone.institution == 'ASHRAE'
        climate_zone = climateZone.value
        next
      end
    end

    table[:data] << ['ASHRAE Climate Zone', climate_zone]

    return table
  end

  # create design_day_table
  def self.design_day_table(model, sqlFile, runner, is_ip_units)
    # data for query
    report_name = 'ClimaticDataSummary'
    table_name = 'SizingPeriod:DesignDay'
    columns = ['', 'Maximum Dry Bulb', 'Daily Temperature Range', 'Humidity Value', 'Humidity Type', 'Wind Speed', 'Wind Direction']

    # populate dynamic rows
    rows_name_query = "SELECT DISTINCT  RowName FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}'"
    row_names = sqlFile.execAndReturnVectorOfString(rows_name_query).get
    rows = []
    row_names.each do |row_name|
      rows << row_name
    end

    # create table
    table = {}
    table[:title] = 'Sizing Period Design Days'
    table[:header] = columns
    table[:source_units] = ['', 'C', 'K', '', '', 'm/s', '']
    if is_ip_units
      table[:units] = ['', 'F', 'R', '', '', 'mph', '']
    else
      table[:units] = table[:source_units]
    end

    table[:data] = []

    # run query and populate table
    rows.each do |row|
      row_data = [row]
      column_counter = -1
      table[:header].each do |header|
        column_counter += 1
        next if header == ''
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row.gsub("'", "''")}' and ColumnName= '#{header}'"
        if header == 'Humidity Type'
          results = sqlFile.execAndReturnFirstString(query).get
          if is_ip_units
            #  TODO:  <11-12-18, jmarrec> # That doesn't seem right to me
            results = results.gsub('[C]', '[F]')
            results = results.gsub('[J/kg]', '[Btu/lb]')
          end
          # any other types?
        elsif header == 'Humidity Value'
          results = sqlFile.execAndReturnFirstDouble(query).get
          # get humidity units for conversion
          query_humidity_type = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row.gsub("'", "''")}' and ColumnName= 'Humidity Type'"
          results_units = sqlFile.execAndReturnFirstString(query_humidity_type).get
          if is_ip_units
            if results_units.include?('[C]')
              results = OpenStudio.convert(results, 'C', 'F').get.round(2)
            elsif results_units.include?('[J/kg]')
              results = OpenStudio.convert(results, 'J/kg', 'Btu/lb').get.round(2)
              # any other types?
            end
          end
        else
          results_si = sqlFile.execAndReturnFirstString(query).get.to_s.delete(' ').to_f
          if is_ip_units
            results = OpenStudio.convert(results_si, table[:source_units][column_counter], table[:units][column_counter]).get.round(2)
          else
            results = results_si
          end
        end
        row_data << results
      end

      table[:data] << row_data
    end

    return table
  end

  # create template section
  def self.building_performance_table(model, sqlFile, runner, is_ip_units = true)
    # create a second table
    building_performance_table = {}
    building_performance_table[:title] = 'Building Performance'
    building_performance_table[:header] = ['Description', 'Value']
    building_performance_table[:units] = []
    building_performance_table[:data] = []

    # add rows to table
    # building_performance_table[:data] << ["Vanilla",1.5]

    return building_performance_table
  end

  # create template section
  def self.site_performance_table(model, sqlFile, runner, is_ip_units = true)
    # create a second table
    site_performance_table = {}
    site_performance_table[:title] = 'Site Performance'
    site_performance_table[:header] = ['Description', 'Value']
    site_performance_table[:units] = []
    site_performance_table[:data] = []

    # add rows to table
    # site_performance_table[:data] << ["Vanilla",1.5]

    return site_performance_table
  end

  # create template section
  def self.site_power_generation_table(model, sqlFile, runner, is_ip_units = true)
    # create a second table
    site_power_generation_table = {}
    site_power_generation_table[:title] = 'Renewable Energy Source Summary'
    site_power_generation_table[:header] = ['', 'Rated Capacity', 'Annual Energy Generated']
    site_power_generation_table[:source_units] = ['', 'kW', 'GJ']
    site_power_generation_table[:units] = ['', 'kW', 'kWh']
    site_power_generation_table[:data] = []

    # create string for LEED advisories
    rows = []
    rows << 'Photovoltaic'
    rows << 'Wind'

    # loop through advisory messages
    value_found = false
    rows.each do |row|
      row_data = [row]
      column_counter = -1
      site_power_generation_table[:header].each do |header|
        column_counter += 1
        next if column_counter == 0
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='LEEDsummary' and RowName= '#{row}' and ColumnName='#{header}';"
        data = sqlFile.execAndReturnFirstDouble(query).get
        data_ip = OpenStudio.convert(data, site_power_generation_table[:source_units][column_counter], site_power_generation_table[:units][column_counter]).get
        if data > 0 then value_found = true end

        row_data << data_ip.round(2)
      end
      site_power_generation_table[:data] << row_data
    end

    if value_found
      return site_power_generation_table
    else
      return false
    end
  end

  # create template section
  def self.monthly_overview_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    monthly_tables = []

    # gather data for section
    @monthly_overview_section = {}
    @monthly_overview_section[:title] = 'Monthly Overview'
    @monthly_overview_section[:tables] = monthly_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @monthly_overview_section
    end

    # check value of reg_monthly_details in measures
    report_detailed = false
    runner.workflow.workflowSteps.each do |step|
      next if !step.to_MeasureStep.is_initialized
      measure_step = step.to_MeasureStep.get
      next if measure_step.result.is_initialized # don't look at upstream measures that have already run
      arg_test = measure_step.getArgument('reg_monthly_details')
      if arg_test.is_initialized
        report_detailed = arg_test.get.valueAsBoolean
      end
      break # only want to check the first measure that doesn't have results yet
    end

    # alert user if report_detailed is requested
    if report_detailed then runner.registerInfo('Monthly End Use by Fuel registerValues have been requested.') end

    # end use colors by index
    end_use_colors = ['#EF1C21', '#0071BD', '#F7DF10', '#DEC310', '#4A4D4A', '#B5B2B5', '#FF79AD', '#632C94', '#F75921', '#293094', '#CE5921', '#FFB239', '#29AAE7', '#8CC739']

    # sorted end use array to pass in for stacked bar chart order
    end_use_order = []
    month_order = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']

    # loop through fuels for consumption tables
    fuel_type_names.each do |fuel_type|
      # get fuel type and units
      if fuel_type == 'Electricity'
        units = '"kWh"'
        unit_str = 'kWh'
      else
        if is_ip_units
          units = '"Million Btu"'
          unit_str = 'MBtu'
        else
          units = '"kWh"'
          unit_str = 'kWh'
        end
      end

      # create table
      monthly_fuel = {}
      monthly_fuel[:title] = "#{fuel_type} Consumption (#{unit_str})"
      monthly_fuel[:header] = ['', 'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec', 'Total']
      monthly_fuel[:units] = []
      monthly_fuel[:data] = []
      monthly_fuel[:chart_type] = 'vertical_stacked_bar'
      monthly_fuel[:chart_attributes] = { value: monthly_fuel[:title], label_x: 'Month', sort_yaxis: end_use_order, sort_xaxis: month_order }
      monthly_fuel[:chart] = []

      # has to hold monthly totals for fuel
      monthly_total = {}

      # rest counter for each fuel type
      site_energy_use = 0.0
      fuel_type_aggregation = 0.0

      # loop through end uses
      OpenStudio::EndUseCategoryType.getValues.each do |category_type|
        category_str = OpenStudio::EndUseCategoryType.new(category_type).valueDescription
        end_use_order << category_str
        row_data = [category_str]
        fuel_and_category_aggregation = 0.0

        OpenStudio::MonthOfYear.getValues.each do |month|
          if month >= 1 && month <= 12
            valInJ  = nil
            if fuel_type_names.include?(fuel_type)
              if !sqlFile.energyConsumptionByMonth(OpenStudio::EndUseFuelType.new(fuel_type),
                                                   OpenStudio::EndUseCategoryType.new(category_type),
                                                   OpenStudio::MonthOfYear.new(month)).empty?
                valInJ = sqlFile.energyConsumptionByMonth(OpenStudio::EndUseFuelType.new(fuel_type),
                                                          OpenStudio::EndUseCategoryType.new(category_type),
                                                          OpenStudio::MonthOfYear.new(month)).get
              end
            else
              # manual sql query for additional fuel types
              report_name = "BUILDING ENERGY PERFORMANCE - #{fuel_type.upcase}"
              row_name = OpenStudio::MonthOfYear.new(month).valueDescription
              column_name = "#{category_str.upcase}:#{fuel_type.upcase}"
              query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and ReportForString='Meter' and RowName= '#{row_name}' and ColumnName='#{column_name}' and Units='J';"
              if !sqlFile.execAndReturnFirstDouble(query).empty?
                valInJ = sqlFile.execAndReturnFirstDouble(query).get
              end
            end

            if !valInJ.nil?
              fuel_and_category_aggregation += valInJ
              valInUnits = OpenStudio.convert(valInJ, 'J', unit_str).get

              # report values for monthly tables
              if report_detailed
                # return just first three characters of month
                month_str = OpenStudio::MonthOfYear.new(month).valueDescription[0..2]
                # this specific string chosen to match design case for a specific project
                prefix_str = OpenStudio.toUnderscoreCase("end_use_#{fuel_type}_#{category_str}_#{month_str}")
                runner.registerValue(prefix_str.downcase.gsub(' ', '_'), valInUnits, unit_str)
              end

              # populate hash for monthly totals
              month = monthly_fuel[:header][month]
              if monthly_total[month]
                monthly_total[month] += valInJ
              else
                monthly_total[month] = valInJ
              end

              monthly_fuel[:chart] << JSON.generate(label: category_str, label_x: month, value: valInUnits, color: end_use_colors[category_type])
              # for some reason sometimes 0 comes through here, show as blank of 0
              if valInUnits > 0
                row_data << valInUnits.round(2)
              else
                row_data << ''
              end

            else
              row_data << ''
              # populate hash for monthly totals
              month = monthly_fuel[:header][month]
              if monthly_total[month]
                # do nothing
              else
                monthly_total[month] = 0.0
              end

            end
          end
        end

        prefix_str = OpenStudio.toUnderscoreCase("#{fuel_type}_#{category_str}")
        runner.registerValue("#{prefix_str}_ip", OpenStudio.convert(fuel_and_category_aggregation, 'J', unit_str).get, unit_str)

        fuel_type_aggregation += fuel_and_category_aggregation
        row_total = OpenStudio.convert(fuel_and_category_aggregation, 'J', unit_str).get
        if row_total == 0
          row_data << ''
        else
          row_data << row_total.round(2)
        end
        monthly_fuel[:data] << row_data
      end

      runner.registerValue(OpenStudio.toUnderscoreCase("#{fuel_type}_ip"),
                           OpenStudio.convert(fuel_type_aggregation, 'J', unit_str).get,
                           unit_str)
      site_energy_use += fuel_type_aggregation

      # add row for totals
      row_data = ['Total']
      monthly_total.each do |k, v|
        if OpenStudio.convert(v, 'J', unit_str).get == 0
          row_data << ''
        else
          row_data << OpenStudio.convert(v, 'J', unit_str).get.round(2)
        end

        # add monthly consumption by fuel from table to runner.registerValues
        if report_detailed
          # return jsut first three characters of month
          month_str = k[0..2]
          prefix_str = OpenStudio.toUnderscoreCase("#{fuel_type}_ip_#{month_str}")
          runner.registerValue(prefix_str.downcase.gsub(' ', '_'), OpenStudio.convert(v, 'J', unit_str).get, unit_str)
        end
      end

      table_total = OpenStudio.convert(site_energy_use, 'J', unit_str).get
      row_data << table_total.round(2)
      monthly_fuel[:data] << row_data

      # add table to array of tables if table total is > 0
      if table_total > 0
        monthly_tables << monthly_fuel
      end
    end

    # loop through fuels for peak demand tables
    fuel_type_names.each do |fuel_type|
      # get fuel type and units
      if fuel_type == 'Electricity'
        unit_str = 'kW'
      else
        if is_ip_units
          unit_str = 'kBtu/hr' # TODO: - update units ?
        else
          unit_str = 'W'
        end
      end

      # create table
      monthly_fuel = {}
      monthly_fuel[:title] = "#{fuel_type} Peak Demand (#{unit_str})"
      monthly_fuel[:header] = ['', 'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
      monthly_fuel[:units] = []
      monthly_fuel[:data] = []
      monthly_fuel[:chart_type] = 'vertical_stacked_bar'
      monthly_fuel[:chart_attributes] = { value: monthly_fuel[:title], label_x: 'Month', sort_yaxis: end_use_order, sort_xaxis: month_order }
      monthly_fuel[:chart] = []

      # has to hold monthly totals for fuel
      monthly_total = {}

      # test for non 0 value in table
      value_found = false

      # loop through end uses
      OpenStudio::EndUseCategoryType.getValues.each do |category_type|
        category_str = OpenStudio::EndUseCategoryType.new(category_type).valueDescription
        row_data = [category_str]

        OpenStudio::MonthOfYear.getValues.each do |month|
          if month >= 1 && month <= 12
            if !sqlFile.peakEnergyDemandByMonth(OpenStudio::EndUseFuelType.new(fuel_type),
                                                OpenStudio::EndUseCategoryType.new(category_type),
                                                OpenStudio::MonthOfYear.new(month)).empty?
              valInJ = sqlFile.peakEnergyDemandByMonth(OpenStudio::EndUseFuelType.new(fuel_type),
                                                       OpenStudio::EndUseCategoryType.new(category_type),
                                                       OpenStudio::MonthOfYear.new(month)).get
              valInUnits = OpenStudio.convert(valInJ, 'W', unit_str).get

              # populate hash for monthly totals
              month = monthly_fuel[:header][month]
              if monthly_total[month]
                monthly_total[month] += valInJ
              else
                monthly_total[month] = valInJ
              end

              monthly_fuel[:chart] << JSON.generate(label: category_str, label_x: month, value: valInUnits, color: end_use_colors[category_type])
              # for some reason sometimes 0 comes through here, show as blank of 0
              if valInUnits > 0
                row_data << valInUnits.round(4)
                value_found = true
              else
                row_data << ''
              end

            else
              row_data << ''
              month = monthly_fuel[:header][month]
              if monthly_total[month]
                # do nothing
              else
                monthly_total[month] = 0.0
              end

            end
          end
        end

        monthly_fuel[:data] << row_data
      end

      # add row for totals
      row_data = ['Total']
      monthly_total.each do |k, v|
        if OpenStudio.convert(v, 'W', unit_str).get == 0
          row_data << ''
        else
          row_data << OpenStudio.convert(v, 'W', unit_str).get.round(2)
        end
      end

      monthly_fuel[:data] << row_data

      # add table if value found
      if value_found
        monthly_tables << monthly_fuel
      end
    end

    return @monthly_overview_section
  end

  # create utility section
  def self.utility_bills_rates_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    utility_bills_rates_tables = []

    # gather data for section
    @utility_bills_rates_section = {}
    @utility_bills_rates_section[:title] = 'Utility Bills/Rates'
    @utility_bills_rates_section[:tables] = utility_bills_rates_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @utility_bills_rates_section
    end

    # create table
    utility_table = {}
    utility_table[:title] = 'Energy Type Summary'
    utility_table[:header] = ['', 'Utility Rate', 'Average Rate', 'Units of Energy', 'Units of Demand']
    utility_table[:query_column] = ['', 'Utility Rate', 'Virtual Rate', 'Units of Energy', 'Units of Demand']
    utility_table[:units] = ['', '', '$/unit energy', '', '']
    utility_table[:data] = []

    # populate dynamic rows
    rows_name_query = "SELECT DISTINCT  RowName FROM tabulardatawithstrings WHERE ReportName='LEEDsummary' and TableName='EAp2-3. Energy Type Summary'"
    row_names = sqlFile.execAndReturnVectorOfString(rows_name_query).get
    rows = []
    row_names.each do |row_name|
      next if row_name == 'Other' # other currently mixes water and dist htg/clg. Don't want to show that
      rows << row_name
    end

    # loop through rows
    value_found = false
    rows.each do |row|
      data = [row]
      utility_table[:query_column].each do |header|
        next if header == ''
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='LEEDsummary'  and TableName='EAp2-3. Energy Type Summary' and RowName= '#{row}' and ColumnName='#{header}';"
        data << sqlFile.execAndReturnFirstString(query).get
        if sqlFile.execAndReturnFirstString(query).get.to_f > 0
          value_found = true
        end
      end
      utility_table[:data] << data
    end

    # add table to array of tables
    if value_found
      utility_bills_rates_tables << utility_table
    end

    # create table
    energy_cost_table = {}
    energy_cost_table[:title] = 'Energy Cost Summary'
    energy_cost_table[:header] = ['', 'Total Energy Cost'] # skipping Process Subtotal
    energy_cost_table[:units] = ['', '$'] # skipping Process Subtotal
    energy_cost_table[:data] = []

    # populate dynamic rows
    rows_name_query = "SELECT DISTINCT  RowName FROM tabulardatawithstrings WHERE ReportName='LEEDsummary' and TableName='EAp2-7. Energy Cost Summary'"
    row_names = sqlFile.execAndReturnVectorOfString(rows_name_query).get
    rows = []
    row_names.each do |row_name|
      rows << row_name
    end

    # loop through rows
    value_found = false
    rows.each do |row|
      data = [row]
      energy_cost_table[:header].each do |header|
        next if header == ''
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='LEEDsummary' and tableName = 'EAp2-7. Energy Cost Summary' and RowName= '#{row}' and ColumnName='#{header}';"
        data << sqlFile.execAndReturnFirstDouble(query).get
        if sqlFile.execAndReturnFirstDouble(query).get > 0
          value_found = true
        end
      end
      energy_cost_table[:data] << data
    end

    # add table to array of tables
    if value_found
      utility_bills_rates_tables << energy_cost_table
    end

    return @utility_bills_rates_section
  end

  # create unmet hours
  def self.zone_condition_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    zone_condition_tables = []

    # gather data for section
    @zone_condition_section = {}
    @zone_condition_section[:title] = 'Zone Conditions'
    @zone_condition_section[:tables] = zone_condition_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @zone_condition_section
    end

    temperature_bins_temps_ip = [56, 61, 66, 68, 70, 72, 74, 76, 78, 83, 88]
    temperature_bins_temps_si = [13, 16, 18, 20, 21, 22, 23, 24, 26, 28, 30]
    # temperature_bins_temps_ip.each do |i|
    # temperature_bins_temps_si << OpenStudio.convert(i, 'F', 'C').get.round(2)
    # end

    # hash to store hours
    temperature_bins = {}
    if is_ip_units
      for i in 0..(temperature_bins_temps_ip.size - 1)
        if i == 0
          temperature_bins["< #{temperature_bins_temps_ip[i]}"] = 0
        else
          temperature_bins["#{temperature_bins_temps_ip[i - 1]}-#{temperature_bins_temps_ip[i]}"] = 0
        end
      end

      # catchall bin for values over the top
      temperature_bins[">= #{temperature_bins_temps_ip.last}"] = 0
    else
      for i in 0..(temperature_bins_temps_si.size - 1)
        if i == 0
          temperature_bins["< #{temperature_bins_temps_si[i]}"] = 0
        else
          temperature_bins["#{temperature_bins_temps_si[i - 1]}-#{temperature_bins_temps_si[i]}"] = 0
        end
      end

      # catchall bin for values over the top
      temperature_bins[">= #{temperature_bins_temps_si.last}"] = 0
    end

    # create table
    temperature_table = {}
    temperature_table[:title] = 'Temperature (Table values represent hours spent in each temperature range)'
    temperature_table[:header] = ['Zone', 'Unmet Htg', 'Unmet Htg - Occ']
    temperature_bins.each do |k, v|
      temperature_table[:header] << k
    end
    temperature_table[:header] += ['Unmet Clg', 'Unmet Clg - Occ', 'Mean Temp']
    temperature_table[:units] = ['', 'hr', 'hr']
    temperature_bins.each do |k, v|
      if is_ip_units
        temperature_table[:units] << 'F'
      else
        temperature_table[:units] << 'C'
      end
    end
    if is_ip_units
      temperature_table[:units] += ['hr', 'hr', 'F']
    else
      temperature_table[:units] += ['hr', 'hr', 'C']
    end
    temperature_table[:data] = []
    temperature_table[:data_color] = []

    # get time series data for each zone
    ann_env_pd = OsLib_Reporting.ann_env_pd(sqlFile)
    if ann_env_pd

      # get keys
      keys = sqlFile.availableKeyValues(ann_env_pd, 'Hourly', 'Zone Air Temperature')
      keys.each do |key|
        # reset bin values
        temperature_bins.each do |k, v|
          temperature_bins[k] = 0
        end

        # get desired variable
        output_timeseries = sqlFile.timeSeries(ann_env_pd, 'Hourly', 'Zone Air Temperature', key)
        # loop through timeseries and move the data from an OpenStudio timeseries to a normal Ruby array (vector)
        if output_timeseries.is_initialized # checks to see if time_series exists
          output_timeseries = output_timeseries.get.values
          temp_counter = 0
          temp_sum = 0
          for i in 0..(output_timeseries.size - 1)
            # add to counter and sum
            temp_counter += 1
            temp_sum += output_timeseries[i]

            found_bin = false
            for j in 0..(temperature_bins_temps_si.size - 1)
              if found_bin == false && output_timeseries[i] < temperature_bins_temps_si[j]
                temperature_bins[temperature_bins.keys[j]] += 1
                found_bin = true
              end
            end

            # add to top if larger than all other hash values
            if !found_bin
              temperature_bins[temperature_bins.keys.last] += 1
            end

          end # end of for i in 0..(output_timeseries.size - 1)
        else
          runner.registerWarning("Didn't find data for Zone Air Temperature") # not getting triggered when variable missing
        end # end of if output_timeseries.is_initialized

        # get unmet hours for each zone from tabular data
        query_htg = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='SystemSummary' and TableName = 'Time Setpoint Not Met' and RowName= '#{key}' and ColumnName='During Heating';"
        unmet_htg = sqlFile.execAndReturnFirstDouble(query_htg).get
        query_clg = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='SystemSummary' and TableName = 'Time Setpoint Not Met' and RowName= '#{key}' and ColumnName='During Cooling';"
        unmet_clg = sqlFile.execAndReturnFirstDouble(query_clg).get
        query_htg_occ = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='SystemSummary' and TableName = 'Time Setpoint Not Met' and RowName= '#{key}' and ColumnName='During Occupied Heating';"
        unmet_htg_occ = sqlFile.execAndReturnFirstDouble(query_htg_occ).get
        query_clg_occ = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='SystemSummary' and TableName = 'Time Setpoint Not Met' and RowName= '#{key}' and ColumnName='During Occupied Cooling';"
        unmet_clg_occ = sqlFile.execAndReturnFirstDouble(query_clg_occ).get

        # get mean temp
        if is_ip_units
          mean = OpenStudio.convert(temp_sum / temp_counter.to_f, 'C', 'F').get
        else
          mean = temp_sum / temp_counter.to_f
        end

        # add rows to table
        row_data = [key, unmet_htg.round, unmet_htg_occ.round]
        row_color = ['', '', '']
        temperature_bins.each do |k, v|
          row_data << v
          if v > 2000
            row_color << 'indianred'
          elsif v > 1000
            row_color << 'orange'
          elsif v > 500
            row_color << 'yellow'
          else
            row_color << ''
          end
        end
        if is_ip_units
          row_data += [unmet_clg.round, unmet_clg_occ.round, "#{mean.round(1)} (F)"]
        else
          row_data += [unmet_clg.round, unmet_clg_occ.round, "#{mean.round(1)} (C)"]
        end
        row_color += ['', '', '']
        temperature_table[:data] << row_data
        temperature_table[:data_color] << row_color
      end
    else
      runner.registerWarning('An annual simulation was not run. Cannot get annual timeseries data')
      return false
    end

    # add table to array of tables
    zone_condition_tables << temperature_table

    humidity_bins_ip = [30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80]
    humidity_bins_si = humidity_bins_ip

    # hash to store hours
    humidity_bins = {}
    for i in 0..(humidity_bins_ip.size - 1)
      if i == 0
        humidity_bins["< #{humidity_bins_ip[i]}"] = 0
      else
        humidity_bins["#{humidity_bins_ip[i - 1]}-#{humidity_bins_ip[i]}"] = 0
      end
    end

    # add catch all bin at top
    humidity_bins[">= #{humidity_bins_ip.last}"] = 0

    # create table
    humidity_table = {}
    humidity_table[:title] = 'Humidity (Table values represent hours spent in each Humidity range)'
    humidity_table[:header] = ['Zone']
    humidity_bins.each do |k, v|
      humidity_table[:header] << k
    end
    humidity_table[:header] += ['Mean Relative Humidity']
    humidity_table[:units] = ['']
    humidity_bins.each do |k, v|
      humidity_table[:units] << '%'
    end
    humidity_table[:units] += ['%']
    humidity_table[:data] = []
    humidity_table[:data_color] = []

    # get time series data for each zone
    ann_env_pd = OsLib_Reporting.ann_env_pd(sqlFile)
    if ann_env_pd

      # store values about humidity fir reguster values
      zone_max_hours_over_70_rh = 0
      zone_max_hours_over_55_rh = 0
      rh_hours_threshold = 10 # hr
      num_zones_x_hours_over_70 = 0
      num_zones_x_hours_over_55 = 0

      # get keys
      keys = sqlFile.availableKeyValues(ann_env_pd, 'Hourly', 'Zone Air Relative Humidity')
      keys.each do |key|
        # reset bin values
        humidity_bins.each do |k, v|
          humidity_bins[k] = 0
        end

        # reset humidity zone flag
        zone_rh_count_hr_55 = 0.0
        zone_rh_count_hr_70 = 0.0

        # get desired variable
        output_timeseries = sqlFile.timeSeries(ann_env_pd, 'Hourly', 'Zone Air Relative Humidity', key)
        # loop through timeseries and move the data from an OpenStudio timeseries to a normal Ruby array (vector)
        if output_timeseries.is_initialized # checks to see if time_series exists
          output_timeseries = output_timeseries.get.values
          humidity_counter = 0
          humidity_sum = 0
          for i in 0..(output_timeseries.size - 1)
            # add to counter and sum
            humidity_counter += 1
            humidity_sum += output_timeseries[i]

            found_bin = false
            for j in 0..(humidity_bins_si.size - 1)
              if found_bin == false && output_timeseries[i] < humidity_bins_si[j]
                humidity_bins[humidity_bins.keys[j]] += 1
                found_bin = true
              end
            end

            # add to top if larger than all other hash values
            if !found_bin
              humidity_bins[humidity_bins.keys.last] += 1
            end

          end # end of for i in 0..(output_timeseries.size - 1)
        else
          runner.registerWarning("Didn't find data for Zone Air Relative Humidity") # not getting triggered when variable missing
        end # end of if output_timeseries.is_initialized

        # get mean humidity
        mean = humidity_sum / humidity_counter.to_f

        # add rows to table
        row_data = [key]
        row_color = ['']
        humidity_bins.each do |k, v|
          row_data << v
          if v > 2000
            row_color << 'indianred'
          elsif v > 1000
            row_color << 'orange'
          elsif v > 500
            row_color << 'yellow'
          else
            row_color << ''
          end

          # populate rh data for register_values
          # catch greater than 70 and 80 for runner.registerValue
          if ['55-60', '60-65', '65-70', '70-75', '75-80', '>= 80'].include?(k)
            zone_rh_count_hr_55 += v
          end
          if ['70-75', '75-80', '>= 80'].include?(k)
            zone_rh_count_hr_70 += v
          end
        end
        row_data += ["#{mean.round(1)} (%)"]
        row_color += ['']
        humidity_table[:data] << row_data
        humidity_table[:data_color] << row_color

        # apply rh zones and max hours
        if zone_rh_count_hr_55 >= rh_hours_threshold then num_zones_x_hours_over_55 += 1 end
        if zone_rh_count_hr_70 >= rh_hours_threshold then num_zones_x_hours_over_70 += 1 end
        if zone_max_hours_over_55_rh < zone_rh_count_hr_55 then zone_max_hours_over_55_rh = zone_rh_count_hr_55 end
        if zone_max_hours_over_70_rh < zone_rh_count_hr_70 then zone_max_hours_over_70_rh = zone_rh_count_hr_70 end

        # add rh runner.registerValues to be used as output in analyses
        runner.registerValue('zone_max_hours_over_70_rh', zone_max_hours_over_70_rh, 'hr')
        runner.registerValue('zone_max_hours_over_55_rh', zone_max_hours_over_55_rh, 'hr')
        runner.registerValue('num_zones_x_hours_over_70', num_zones_x_hours_over_70, 'zones')
        runner.registerValue('num_zones_x_hours_over_55', num_zones_x_hours_over_55, 'zones')
      end
    else
      runner.registerWarning('An annual simulation was not run. Cannot get annual timeseries data')
      return false
    end

    # add table to array of tables
    zone_condition_tables << humidity_table

    return @zone_condition_section
  end

  # create interior_lighting section
  def self.interior_lighting_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    interior_lighting_tables = []

    # gather data for section
    @interior_lighting_section = {}
    @interior_lighting_section[:title] = 'Interior Lighting Summary'
    @interior_lighting_section[:tables] = interior_lighting_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @interior_lighting_section
    end

    # data for query
    report_name = 'LightingSummary'
    table_name = 'Interior Lighting'
    columns = ['Lights ', 'Zone', 'Lighting Power Density', 'Total Power', 'Schedule Name', 'Scheduled Hours/Week', 'Actual Load Hours/Week', 'Return Air Fraction', 'Annual Consumption']
    columns_query = ['', 'Zone', 'Lighting Power Density', 'Total Power', 'Schedule Name', 'Scheduled Hours/Week', 'Full Load Hours/Week', 'Return Air Fraction', 'Consumption']

    # populate dynamic rows
    rows_name_query = "SELECT DISTINCT  RowName FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}'"
    row_names = sqlFile.execAndReturnVectorOfString(rows_name_query).get
    rows = []
    row_names.each do |row_name|
      next if row_name == 'Interior Lighting Total' # skipping this on purpose, may give odd results in some instances
      rows << row_name
    end

    # Zone-level Lighting Summary
    table = {}
    table[:title] = 'Zone Lighting'
    table[:header] = columns
    source_units_area = 'm^2'
    source_units_lpd = 'W/m^2'
    source_units_energy = 'GJ'
    if is_ip_units
      target_units_area = 'ft^2'
      target_units_lpd = 'W/ft^2'
      target_units_energy = 'kWh'
    else
      target_units_area = 'm^2'
      target_units_lpd = 'W/m^2'
      target_units_energy = 'kWh'
    end
    table[:source_units] = ['', '', source_units_lpd, 'W', '', 'hr', 'hr', '', source_units_energy] # used for conversion, not needed for rendering.
    table[:units] = ['', '', target_units_lpd, 'W', '', 'hr', 'hr', '', target_units_energy]
    table[:data] = []

    # run query and populate table
    rows.each do |row|
      row_data = [row]
      column_counter = -1
      columns_query.each do |header|
        column_counter += 1
        next if header == ''
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row}' and ColumnName= '#{header}'"
        if table[:source_units][column_counter] != ''
          results = sqlFile.execAndReturnFirstDouble(query)
          row_data_ip = OpenStudio.convert(results.to_f, table[:source_units][column_counter], table[:units][column_counter]).get
          row_data << row_data_ip.round(2)
        else
          results = sqlFile.execAndReturnFirstString(query)
          row_data << results
        end
      end

      table[:data] << row_data
    end

    # Space-level lighting loads table(s)
    space_lighting_table = {}
    space_lighting_table[:title] = 'Space Lighting Details'
    space_lighting_table[:header] = ['Load Name', 'Definition Name', 'Load Type', 'Load (units)', 'Multiplier', 'Total Load (W)']
    space_lighting_table[:data] = []

    spaces = model.getSpaces
    spaces.each do |space|
      table_row = []

      area = OpenStudio.convert(space.floorArea, source_units_area, target_units_area).get
      lp = OpenStudio.convert(space.lightingPowerPerFloorArea, source_units_lpd, target_units_lpd).get

      space_lighting_table[:data] << [{ sub_header: "Space Name: '#{space.name}', Area: #{area.round(0)} #{target_units_area},
                                        Total LPD: #{lp.round(2)} #{target_units_lpd}" }, '', '', '', '', '']

      lights_found = 0

      if space.spaceType.is_initialized

        space.spaceType.get.lights.each do |lights_object|
          tlp = ''
          def_name = lights_object.lightsDefinition.name

          lights_found += 1

          if lights_object.lightsDefinition.designLevelCalculationMethod == 'LightingLevel'

            val = "#{lights_object.lightsDefinition.lightingLevel.to_f.round(0)} (W)"
            tlp = lights_object.lightsDefinition.lightingLevel.to_f * lights_object.multiplier

          end

          if lights_object.lightsDefinition.designLevelCalculationMethod == 'Watts/Area'

            val_conv = OpenStudio.convert(lights_object.lightsDefinition.wattsperSpaceFloorArea.to_f, source_units_lpd, target_units_lpd).get
            val = "#{val_conv.to_f.round(2)} (#{target_units_lpd})"
            tlp = (lights_object.lightsDefinition.wattsperSpaceFloorArea.to_f * space.floorArea) * lights_object.multiplier

          end

          if lights_object.lightsDefinition.designLevelCalculationMethod == 'Watts/Person'

            val = "#{lights_object.lightsDefinition.wattsperPerson.to_f.round(2)} (W/Person)"
            tlp = (lights_object.lightsDefinition.wattsperPerson.to_f * space.numberOfPeople) * lights_object.multiplier

          end

          space_lighting_table[:data] << [lights_object.name.to_s, def_name, 'Spacetype', val, lights_object.multiplier.round(0), tlp.round(0)]
        end

      end

      space.lights.each do |sl|
        tlp = ''
        def_name = sl.lightsDefinition.name

        lights_found += 1

        if sl.lightsDefinition.designLevelCalculationMethod == 'LightingLevel'

          val = "#{sl.lightsDefinition.lightingLevel.to_f.round(0)} (W)"
          tlp = sl.lightsDefinition.lightingLevel.to_f * sl.multiplier

        end

        if sl.lightsDefinition.designLevelCalculationMethod == 'Watts/Area'

          val_conv = OpenStudio.convert(sl.lightsDefinition.wattsperSpaceFloorArea.to_f, source_units_lpd, target_units_lpd).get
          val = "#{val_conv.to_f.round(2)} (#{target_units_lpd})"
          tlp = (sl.lightsDefinition.wattsperSpaceFloorArea.to_f * space.floorArea) * sl.multiplier

        end

        if sl.lightsDefinition.designLevelCalculationMethod == 'Watts/Person'

          val = "#{sl.lightsDefinition.wattsperPerson.to_f.round(2)} (W/Person)"
          tlp = (sl.lightsDefinition.wattsperPerson.to_f * space.numberOfPeople) * sl.multiplier

        end

        space_lighting_table[:data] << [sl.name.to_s, def_name, 'Space', val, sl.multiplier.round(0), tlp.round(0)]
      end

      space_lighting_table[:data] << ['-', '-', '-', '-', '-', '-'] if lights_found == 0
    end

    # Lighting Controls

    source_units_illuminance = 'lux'
    if is_ip_units
      target_units_illuminance = 'fc'
    else
      target_units_illuminance = source_units_illuminance
    end

    lighting_controls_table = {}
    lighting_controls_table[:title] = 'Lighting Controls Details'
    lighting_controls_table[:header] = ['Space Name', 'Control Name', 'Zone Controlled (type, fraction)', "Illuminance Setpoint (#{target_units_illuminance})"]
    lighting_controls_table[:data] = []
    model.getSpaces.sort.each do |space|
      thermal_zone = space.thermalZone.get

      zone_control = 'n/a'

      space.daylightingControls.each do |dc|
        if thermal_zone.primaryDaylightingControl.is_initialized && dc.isPrimaryDaylightingControl
          zone_control = "#{thermal_zone.name} (primary, #{thermal_zone.fractionofZoneControlledbyPrimaryDaylightingControl.round(1)})"
        end
        if thermal_zone.secondaryDaylightingControl.is_initialized && dc.isSecondaryDaylightingControl
          zone_control = "#{thermal_zone.name} (secondary, #{thermal_zone.fractionofZoneControlledbySecondaryDaylightingControl.round(1)})"
        end
        illuminance_conv = OpenStudio.convert(dc.illuminanceSetpoint, source_units_illuminance, target_units_illuminance).get
        lighting_controls_table[:data] << [space.name, dc.name, zone_control, illuminance_conv.round(0)]
      end
    end

    # add tables to report
    interior_lighting_tables << table
    interior_lighting_tables << space_lighting_table
    interior_lighting_tables << lighting_controls_table

    return @interior_lighting_section
  end

  # create plug_loads section
  def self.plug_loads_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    plug_loads_tables = []

    # gather data for section
    @plug_loads_section = {}
    @plug_loads_section[:title] = 'Plug Loads Summary'
    @plug_loads_section[:tables] = plug_loads_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @plug_loads_section
    end

    # data for query
    report_name = 'EnergyMeters'
    table_name = 'Annual and Peak Values - Electricity'
    columns = ['', 'Electricity Annual Value'] # TODO: - would be nice to make this more like lighting summary

    # populate dynamic rows
    rows_name_query = "SELECT DISTINCT  RowName FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}'"
    row_names = sqlFile.execAndReturnVectorOfString(rows_name_query).get
    rows = []
    row_names.each do |row_name|
      next unless row_name.include?('InteriorEquipment:Electricity:Zone:')
      rows << row_name
    end

    # create table
    table = {}
    table[:title] = 'Electric Plug Load Consumption'
    table[:header] = columns
    source_units_energy = 'GJ'
    target_units_energy = 'kWh'
    table[:source_units] = ['', source_units_energy] # used for conversation, not needed for rendering.
    table[:units] = ['', target_units_energy]
    table[:data] = []

    # run query and populate table
    rows.each do |row|
      row_data = [row]
      column_counter = -1
      table[:header].each do |header|
        column_counter += 1
        next if header == ''
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row}' and ColumnName= '#{header}'"
        results = sqlFile.execAndReturnFirstDouble(query)
        row_data_ip = OpenStudio.convert(results.to_f, table[:source_units][column_counter], table[:units][column_counter]).get
        row_data << row_data_ip.round(2)
      end

      table[:data] << row_data
    end

    # add table to array of tables
    if !table[:data].empty? then plug_loads_tables << table end

    # space-level electric plug loads inputs table
    table = {}
    table[:title] = 'Space-level Electric Plug Loads'
    table[:header] = ['Equipment Name', 'Definition', 'Load (units)', 'Inheritance Level', 'Multiplier', 'Total Load (W)']
    table[:data] = []

    model.getSpaces.sort.each do |space|
      space_elec_equip = {}

      # check for equipment from both inheritance paths

      space_elec_equip['spacetype'] = space.spaceType.is_initialized ? space.spaceType.get.electricEquipment : []
      space_elec_equip['space'] = space.electricEquipment

      # space subheading if any equipment found

      if is_ip_units
        area = space.floorArea * 10.7639
        table[:data] << [{ sub_header: "Space Name: #{space.name}, Area: #{area.round(0)} ft^2" }, '', '', '', '', ''] if !space_elec_equip['spacetype'].empty? || !space_elec_equip['space'].empty?
      else
        area = space.floorArea
        table[:data] << [{ sub_header: "Space Name: #{space.name}, Area: #{area.round(0)} m^2" }, '', '', '', '', ''] if !space_elec_equip['spacetype'].empty? || !space_elec_equip['space'].empty?
      end

      # spacetype equipment

      space_elec_equip.each do |inheritance_level, elec_equip_array|
        elec_equip_array.each do |elec_equip|
          if elec_equip.electricEquipmentDefinition.designLevelCalculationMethod == 'Watts/Area'
            if is_ip_units
              ee_power = elec_equip.electricEquipmentDefinition.wattsperSpaceFloorArea.to_f * 0.092903 # IP
              ee_power = "#{ee_power.round(2)} (W/ft^2)"
              ee_total_power = ((elec_equip.powerPerFloorArea.to_f * space.floorArea))
            else
              ee_power = elec_equip.electricEquipmentDefinition.wattsperSpaceFloorArea.to_f
              ee_power = "#{ee_power.round(2)} (W/m^2)"
              ee_total_power = ((elec_equip.powerPerFloorArea.to_f * space.floorArea))
            end
          end

          if elec_equip.electricEquipmentDefinition.designLevelCalculationMethod == 'Watts/Person'
            ee_power = "#{elec_equip.electricEquipmentDefinition.wattsperPerson .to_f.round(2)} (W/person)"
            ee_total_power = (elec_equip.powerPerPerson.to_f * space.numberOfPeople)
          end

          if elec_equip.electricEquipmentDefinition.designLevelCalculationMethod == 'EquipmentLevel'
            ee_power = "#{elec_equip.electricEquipmentDefinition.designLevel.to_f.round(0)} (W)"
            ee_total_power = elec_equip.designLevel.to_f.round(0)
          end

          table[:data] << [elec_equip.name, elec_equip.electricEquipmentDefinition.name, ee_power, inheritance_level, elec_equip.multiplier.round(1), ee_total_power.round(0)]
        end
      end
    end # space-level elec plug loads table

    # if data, add table to report
    plug_loads_tables << table if !table[:data].empty?

    # space-level gas plug loads inputs table
    table = {}
    table[:title] = 'Space-level Gas Plug Loads'
    if is_ip_units
      table[:header] = ['Equipment Name', 'Definition', 'Load (units)', 'Inheritance Level', 'Multiplier', 'Total Load (BTU/hr)']
    else
      table[:header] = ['Equipment Name', 'Definition', 'Load (units)', 'Inheritance Level', 'Multiplier', 'Total Load (W)']
    end
    table[:data] = []

    model.getSpaces.sort.each do |space|
      space_gas_equip = {}

      # check for equipment from both inheritance paths

      space_gas_equip['spacetype'] = space.spaceType.is_initialized ? space.spaceType.get.gasEquipment : []
      space_gas_equip['space'] = space.gasEquipment

      # space subheading if any equipment found

      if is_ip_units
        area = space.floorArea.to_f * 10.7639 # --> IP
      else
        area = space.floorArea.to_f
      end
      if is_ip_units
        table[:data] << [{ sub_header: "Space Name: #{space.name}, Area: #{area.round(0)} ft^2" }, '', '', '', '', ''] if !space_gas_equip['spacetype'].empty? || !space_gas_equip['space'].empty?
      else
        table[:data] << [{ sub_header: "Space Name: #{space.name}, Area: #{area.round(0)} m^2" }, '', '', '', '', ''] if !space_gas_equip['spacetype'].empty? || !space_gas_equip['space'].empty?
      end

      # spacetype equipment

      space_gas_equip.each do |inheritance_level, gas_equip_array|
        gas_equip_array.each do |gas_equip|
          if gas_equip.gasEquipmentDefinition.designLevelCalculationMethod == 'Watts/Area'
            if is_ip_units
              ge_power = gas_equip.powerPerFloorArea.to_f * 0.316998331 # W/m^2 --> BTU/hr/ft^2 (OpenStudio.convert() !work (!))
              ge_total_power = ((ge_power * area) * gas_equip.multiplier).to_f
              ge_power = "#{ge_power.to_f.round(2)} (BTU/hr/ft^2)"
            else
              ge_power = gas_equip.powerPerFloorArea.to_f
              ge_total_power = ((ge_power * area) * gas_equip.multiplier).to_f
              ge_power = "#{ge_power.to_f.round(2)} (W/m^2)"
            end
          end

          if gas_equip.gasEquipmentDefinition.designLevelCalculationMethod == 'Watts/Person'
            if is_ip_units
              ge_power = gas_equip.powerPerPerson.to_f * 3.412142 # W --> BTU/hr
              ge_total_power = ((ge_power * space.numberOfPeople) * gas_equip.multiplier).to_f
              ge_power = "#{ge_power.to_f.round(2)} (BTU/hr/person)"
            else
              ge_power = gas_equip.powerPerPerson.to_f
              ge_total_power = ((ge_power * space.numberOfPeople) * gas_equip.multiplier).to_f
              ge_power = "#{ge_power.to_f.round(2)} (W/person)"
            end
          end

          if gas_equip.gasEquipmentDefinition.designLevelCalculationMethod == 'EquipmentLevel'
            if is_ip_units
              ge_power = gas_equip.designLevel.to_f * 3.412142 # W --> BTU/hr
              ge_power = "#{ge_power.to_f.round(0)} (BTU/hr)"
              ge_total_power = gas_equip.designLevel.to_f * gas_equip.multiplier
            else
              ge_power = gas_equip.designLevel.to_f
              ge_power = "#{ge_power.to_f.round(0)} (W)"
              ge_total_power = gas_equip.designLevel.to_f * gas_equip.multiplier
            end
          end

          table[:data] << [gas_equip.name, gas_equip.gasEquipmentDefinition.name, ge_power, inheritance_level, gas_equip.multiplier.round(1), ge_total_power.round(0)]
        end
      end
    end # space-level gas plug loads table

    # if data, add table to report
    plug_loads_tables << table if !table[:data].empty?

    # data for query
    # TODO: - need to test this in model with gas equipment
    report_name = 'EnergyMeters'
    table_name = 'Annual and Peak Values - Gas'
    columns = ['', 'Gas Annual Value'] # TODO: - would be nice to make this more like lighting summary

    # populate dynamic rows
    rows_name_query = "SELECT DISTINCT  RowName FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}'"
    row_names = sqlFile.execAndReturnVectorOfString(rows_name_query).get
    rows = []
    row_names.each do |row_name|
      next unless row_name.include?('InteriorEquipment:Gas:Zone:')
      rows << row_name
    end

    # create table
    table = {}
    table[:title] = 'Gas Plug Load Consumption'
    table[:header] = columns
    source_units_energy = 'GJ'
    if is_ip_units
      target_units_energy = 'kBtu'
    else
      target_units_energy = 'kWh'
    end
    table[:source_units] = ['', source_units_energy] # used for conversation, not needed for rendering.
    table[:units] = ['', target_units_energy]
    table[:data] = []

    # run query and populate table
    rows.each do |row|
      row_data = [row]
      column_counter = -1
      table[:header].each do |header|
        column_counter += 1
        next if header == ''
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row}' and ColumnName= '#{header}'"
        results = sqlFile.execAndReturnFirstDouble(query)
        row_data_ip = OpenStudio.convert(results.to_f, table[:source_units][column_counter], table[:units][column_counter]).get
        row_data << row_data_ip.round(2)
      end

      table[:data] << row_data
    end

    # add table to array of tables
    if !table[:data].empty? then plug_loads_tables << table end

    return @plug_loads_section
  end

  # create unmet hours
  def self.hvac_load_profile(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    hvac_load_profile_tables = []

    # gather data for section
    @hvac_load_profile_section = {}
    @hvac_load_profile_section[:title] = 'HVAC Load Profiles'
    @hvac_load_profile_section[:tables] = hvac_load_profile_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @hvac_load_profile_section
    end

    month_order = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']

    # create table
    hvac_load_profile_monthly_table = {}
    hvac_load_profile_monthly_table[:title] = 'Monthly Load Profiles'
    hvac_load_profile_monthly_table[:header] = ['', 'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
    hvac_load_profile_monthly_table[:units] = []
    hvac_load_profile_monthly_table[:data] = []
    hvac_load_profile_monthly_table[:chart_type] = 'vertical_grouped_bar_with_comp_line'
    if is_ip_units
      hvac_load_profile_monthly_table[:chart_attributes] = { value_left: 'Cooling/Heating Load (MBtu)', label_x: 'Month', value_right: 'Average Outdoor Air Dry Bulb (F)', sort_xaxis: month_order }
    else
      hvac_load_profile_monthly_table[:chart_attributes] = { value_left: 'Cooling/Heating Load (kWh)', label_x: 'Month', value_right: 'Average Outdoor Air Dry Bulb (C)', sort_xaxis: month_order }
    end
    hvac_load_profile_monthly_table[:chart] = []

    # hash to store monthly values
    cooling_monthly = { 'Jan' => 0, 'Feb' => 0, 'Mar' => 0, 'Apr' => 0, 'May' => 0, 'Jun' => 0, 'Jul' => 0, 'Aug' => 0, 'Sep' => 0, 'Oct' => 0, 'Nov' => 0, 'Dec' => 0 }
    heating_monthly = { 'Jan' => 0, 'Feb' => 0, 'Mar' => 0, 'Apr' => 0, 'May' => 0, 'Jun' => 0, 'Jul' => 0, 'Aug' => 0, 'Sep' => 0, 'Oct' => 0, 'Nov' => 0, 'Dec' => 0 }

    # units for conversion
    source_units = 'J'
    if is_ip_units
      target_units = 'MBtu'
    else
      target_units = 'kWh'
    end

    # loop through fuel types
    fuel_type_names.each do |fuel_type|
      OpenStudio::MonthOfYear.getValues.each do |month|
        if month >= 1 && month <= 12

          if fuel_type == 'Natural Gas' then fuel_type = 'Gas' end

          # get cooling value for this fuel and month
          if !sqlFile.energyConsumptionByMonth(OpenStudio::EndUseFuelType.new(fuel_type),
                                               OpenStudio::EndUseCategoryType.new('Cooling'),
                                               OpenStudio::MonthOfYear.new(month)).empty?
            cooling_valInJ = sqlFile.energyConsumptionByMonth(OpenStudio::EndUseFuelType.new(fuel_type),
                                                              OpenStudio::EndUseCategoryType.new('Cooling'),
                                                              OpenStudio::MonthOfYear.new(month)).get

            cooling_valInUnits = OpenStudio.convert(cooling_valInJ, source_units, target_units).get

          else
            cooling_valInUnits = 0
          end

          # get heating value for this fuel and month
          if !sqlFile.energyConsumptionByMonth(OpenStudio::EndUseFuelType.new(fuel_type),
                                               OpenStudio::EndUseCategoryType.new('Heating'),
                                               OpenStudio::MonthOfYear.new(month)).empty?
            heating_valInJ = sqlFile.energyConsumptionByMonth(OpenStudio::EndUseFuelType.new(fuel_type),
                                                              OpenStudio::EndUseCategoryType.new('Heating'),
                                                              OpenStudio::MonthOfYear.new(month)).get

            heating_valInUnits = OpenStudio.convert(heating_valInJ, source_units, target_units).get

          else
            heating_valInUnits = 0
          end

          # create or add to hash to sum across fuel types
          month = hvac_load_profile_monthly_table[:header][month]
          if cooling_monthly.key?(month)
            cooling_monthly[month] = cooling_monthly[month].to_f + cooling_valInUnits
          else
            cooling_monthly[month] = cooling_valInUnits
          end
          if heating_monthly.key?(month)
            heating_monthly[month] = heating_monthly[month].to_f + heating_valInUnits
          else
            heating_monthly[month] = heating_monthly
          end

        end
      end
    end

    # populate dry bulb data
    if is_ip_units
      dry_bulb_monthly = ['Average Outdoor Air Dry Bulb (F)']
    else
      dry_bulb_monthly = ['Average Outdoor Air Dry Bulb (C)']
    end

    ann_env_pd = OsLib_Reporting.ann_env_pd(sqlFile)
    if ann_env_pd
      # get desired variable
      output_timeseries = sqlFile.timeSeries(ann_env_pd, 'Monthly', 'Site Outdoor Air Drybulb Temperature', 'Environment')
      # loop through timeseries and move the data from an OpenStudio timeseries to a normal Ruby array (vector)
      if output_timeseries.is_initialized # checks to see if time_series exists

        # see if filler needed at start or end of table/chart
        num_blanks_start = output_timeseries.get.dateTimes[0].date.monthOfYear.value - 2
        num_blanks_end = 12 - output_timeseries.get.values.size - num_blanks_start

        # fill in blank data for partial year simulations
        for i in 0..(num_blanks_start - 1)
          month = hvac_load_profile_monthly_table[:header][i + 1]
          dry_bulb_monthly << ''
        end

        output_timeseries = output_timeseries.get.values
        for i in 0..(output_timeseries.size - 1)
          month = hvac_load_profile_monthly_table[:header][i + 1 + num_blanks_start]
          if is_ip_units
            value = OpenStudio.convert(output_timeseries[i], 'C', 'F').get
          else
            value = OpenStudio.convert(output_timeseries[i], 'C', 'C').get
          end
          dry_bulb_monthly << value.round(1)
          hvac_load_profile_monthly_table[:chart] << JSON.generate(label: 'Outdoor Temp', label_x: month, value2: value, color: 'green')
        end # end of for i in 0..(output_timeseries.size - 1)

        # fill in blank data for partial year simulations
        for i in 0..(num_blanks_end - 1)
          month = hvac_load_profile_monthly_table[:header][i]
          dry_bulb_monthly << ''
        end

      else
        # TODO: - see why this is getting thrown on some models
        runner.registerWarning("Didn't find data for Site Outdoor Air Drybulb Temperature")
      end # end of if output_timeseries.is_initialized
    else
      runner.registerWarning('An annual simulation was not run.  Cannot get annual timeseries data')
      return false
    end

    # populate tables
    hvac_load_profile_monthly_table[:data] << dry_bulb_monthly
    if is_ip_units
      cooling_array = ['Cooling Load (MBtu)']
    else
      cooling_array = ['Cooling Load (kWh)']
    end
    cooling_monthly.each do |k, v|
      cooling_array << v.round(2)

      # populate chart
      hvac_load_profile_monthly_table[:chart] << JSON.generate(label: 'Cooling Load', label_x: k, value: v, color: '#0071BD')
    end
    hvac_load_profile_monthly_table[:data] << cooling_array
    if is_ip_units
      heating_array = ['Heating Load (MBtu)']
    else
      heating_array = ['Heating Load (kWh)']
    end
    heating_monthly.each do |k, v|
      heating_array << v.round(2)

      # populate chart
      hvac_load_profile_monthly_table[:chart] << JSON.generate(label: 'Heating Load', label_x: k, value: v, color: '#EF1C21')
    end
    hvac_load_profile_monthly_table[:data] << heating_array

    # add table to array of tables
    hvac_load_profile_tables << hvac_load_profile_monthly_table

    # create table
    hvac_part_load_profile_table = {}
    hvac_part_load_profile_table[:title] = 'Part Load Profiles'
    hvac_part_load_profile_table[:header] = ['Load', 'Clg: Cutoff', 'Clg: Hours', 'Clg: Hours', 'Htg: Cutoff', 'Htg: Hours', 'Htg: Hours']
    if is_ip_units
      hvac_part_load_profile_table[:units] = ['%', 'MBtu', '%', 'hr', 'MBtu', '%', 'hr']
    else
      hvac_part_load_profile_table[:units] = ['%', 'kWh', '%', 'hr', 'kWh', '%', 'hr']
    end
    hvac_part_load_profile_table[:data] = []

    # add rows to table
    hvac_part_load_profile_table[:data] << ['0-5', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['5-10', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['10-25', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['15-20', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['20-25', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['25-30', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['30-35', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['35-40', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['40-45', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['45-50', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['50-55', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['55-60', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['60-65', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['65-70', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['70-75', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['75-80', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['80-85', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['85-90', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['90-95', '', '', '', '', '', '']
    hvac_part_load_profile_table[:data] << ['95-100', '', '', '', '', '', '']

    # TODO: - add table to array of tables
    # hvac_load_profile_tables << hvac_part_load_profile_table

    return @hvac_load_profile_section
  end

  def self.zone_summary_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    template_tables = []

    # gather data for section
    @zone_summary_section = {}
    @zone_summary_section[:title] = 'Zone Overview'
    @zone_summary_section[:tables] = template_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @zone_summary_section
    end

    # data for query
    report_name = 'InputVerificationandResultsSummary'
    table_name = 'Zone Summary'
    columns = ['', 'Area', 'Conditioned (Y/N)', 'Part of Total Floor Area (Y/N)', 'Volume', 'Multiplier', 'Above Ground Gross Wall Area', 'Underground Gross Wall Area', 'Window Glass Area', 'Lighting', 'People', 'Plug and Process']

    # test looking at getting entire table to get rows
    # query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}'"
    # results = sqlFile.execAndReturnVectorOfString(query).get

    # populate dynamic rows
    rows_name_query = "SELECT DISTINCT  RowName FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}'"
    row_names = sqlFile.execAndReturnVectorOfString(rows_name_query).get
    rows = []
    row_names.each do |row_name|
      rows << row_name
    end

    # rows = ['Total','Conditioned Total','Unconditioned Total','Not Part of Total']

    # create zone_summary_table
    zone_summary_table = {}
    zone_summary_table[:title] = table_name
    zone_summary_table[:header] = columns
    source_units_area = 'm^2'
    source_units_area_per_person = 'm^2/person'
    source_units_volume = 'm^3'
    source_units_pd = 'W/m^2'
    if is_ip_units
      target_units_area = 'ft^2'
      target_units_area_per_person = 'ft^2/person'
      target_units_volume = 'ft^3'
      target_units_pd = 'W/ft^2'
    else
      target_units_area = 'm^2'
      target_units_area_per_person = 'm^2/person'
      target_units_volume = 'm^3'
      target_units_pd = 'W/m^2'
    end
    zone_summary_table[:units] = ['', target_units_area, '', '', target_units_volume, '', target_units_area, target_units_area, target_units_area, target_units_pd, target_units_area_per_person, target_units_pd]
    zone_summary_table[:source_units] = ['', source_units_area, '', '', source_units_volume, '', source_units_area, source_units_area, source_units_area, source_units_pd, source_units_area_per_person, source_units_pd] # used for conversation, not needed for rendering.
    zone_summary_table[:data] = []

    # run query and populate zone_summary_table
    rows.each do |row|
      row_data = [row]
      column_counter = -1
      zone_summary_table[:header].each do |header|
        column_counter += 1
        next if header == ''
        if header == 'Multiplier' then header = 'Multipliers' end # what we want to show is different than what is in E+ table
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row}' and ColumnName= '#{header}'"
        if zone_summary_table[:source_units][column_counter] != ''
          results = sqlFile.execAndReturnFirstDouble(query)
          row_data_ip = OpenStudio.convert(results.to_f, zone_summary_table[:source_units][column_counter], zone_summary_table[:units][column_counter]).get
          row_data << row_data_ip.round(2)
        else
          results = sqlFile.execAndReturnFirstString(query)
          row_data << results
        end
      end

      zone_summary_table[:data] << row_data
    end

    # add zone_summary_table to array of tables
    template_tables << zone_summary_table

    # data for query
    report_name = 'HVACSizingSummary'
    table_01_name = 'Zone Sensible Cooling'
    table_02_name = 'Zone Sensible Heating'
    columns = ['', 'Heating/Cooling', 'Calculated Design Load', 'Design Load With Sizing Factor', 'Calculated Design Air Flow', 'Design Air Flow  With Sizing Factor', 'Date/Time Of Peak', 'Outdoor Temperature at Peak Load', 'Outdoor Humidity Ratio at Peak Load']
    columns_query = ['', 'Heating/Cooling', 'Calculated Design Load', 'User Design Load', 'Calculated Design Air Flow', 'User Design Air Flow', 'Date/Time Of Peak {TIMESTAMP}', 'Outdoor Temperature at Peak Load', 'Outdoor Humidity Ratio at Peak Load']

    # populate dynamic rows
    rows_name_query = "SELECT DISTINCT  RowName FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_01_name}'"
    row_names = sqlFile.execAndReturnVectorOfString(rows_name_query).get
    rows = []
    row_names.each do |row_name|
      rows << row_name
    end

    # create zone_dd_table
    zone_dd_table = {}
    zone_dd_table[:title] = 'Zone Sensible Cooling and Heating Sensible Sizing'
    zone_dd_table[:header] = columns
    source_units_power = 'W'
    source_units_air_flow = 'm^3/s'
    source_units_temp = 'C'

    if is_ip_units
      target_units_power_clg = 'ton'
      target_units_power_htg = 'kBtu/h'
      target_units_air_flow = 'ft^3/min'
      target_units_temp = 'F'

      # This one's a ratio... so it doesn't matter really, we don't convert
      # but we want to display something pretty
      target_units_humrat_display = 'lbWater/lbAir'

    else
      target_units_power_clg = 'W'
      target_units_power_htg = 'W'
      target_units_air_flow = 'm^3/h'
      target_units_temp = 'C'

      target_units_humrat_display = 'kgWater/kgAir'

    end

    zone_dd_table[:units] = ['', '', '', '', target_units_air_flow, target_units_air_flow, '', target_units_temp, target_units_humrat_display]
    # used for convertion, not needed for rendering.
    zone_dd_table[:source_units] = ['', '', '', '', source_units_air_flow, source_units_air_flow, '', source_units_temp, target_units_humrat_display]

    zone_dd_table[:data] = []

    # run query and populate zone_dd_table
    rows.each do |row|
      # populate cooling row
      row_data = [row, 'Cooling']
      column_counter = -1
      columns_query.each do |header|
        column_counter += 1
        next if header == '' || header == 'Heating/Cooling'
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_01_name}' and RowName= '#{row}' and ColumnName= '#{header}'"
        if zone_dd_table[:source_units][column_counter] != ''
          results = sqlFile.execAndReturnFirstDouble(query)
          row_data_ip = OpenStudio.convert(results.to_f, zone_dd_table[:source_units][column_counter], zone_dd_table[:units][column_counter]).get
          row_data << row_data_ip.round(2)
        elsif header == 'Calculated Design Load' || header == 'User Design Load'
          results = sqlFile.execAndReturnFirstDouble(query)
          row_data_ip = OpenStudio.convert(results.to_f, source_units_power, target_units_power_clg).get
          row_data << "#{row_data_ip.round(2)} (#{target_units_power_clg})"
        else
          results = sqlFile.execAndReturnFirstString(query)
          row_data << results
        end
      end
      zone_dd_table[:data] << row_data

      # populate heating row
      row_data = [row, 'Heating']
      column_counter = -1
      columns_query.each do |header|
        column_counter += 1
        next if header == '' || header == 'Heating/Cooling'
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_02_name}' and RowName= '#{row}' and ColumnName= '#{header}'"
        if zone_dd_table[:source_units][column_counter] != ''
          results = sqlFile.execAndReturnFirstDouble(query)
          row_data_ip = OpenStudio.convert(results.to_f, zone_dd_table[:source_units][column_counter], zone_dd_table[:units][column_counter]).get
          row_data << row_data_ip.round(2)
        elsif header == 'Calculated Design Load' || header == 'User Design Load'
          results = sqlFile.execAndReturnFirstDouble(query)
          row_data_ip = OpenStudio.convert(results.to_f, source_units_power, target_units_power_htg).get
          row_data << "#{row_data_ip.round(2)} (#{target_units_power_htg})"
        else
          results = sqlFile.execAndReturnFirstString(query)
          row_data << results
        end
      end
      zone_dd_table[:data] << row_data
    end

    # add zone_dd_table to array of tables
    template_tables << zone_dd_table

    return @zone_summary_section
  end

  # create air_loop_summary section
  def self.air_loop_summary_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    air_loop_summary_tables = []

    # gather data for section
    @air_loop_summary_section = {}
    @air_loop_summary_section[:title] = 'Air Loops Summary'
    @air_loop_summary_section[:tables] = air_loop_summary_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @air_loop_summary_section
    end

    # create table
    air_loop_summary_table = {}
    air_loop_summary_table[:title] = 'Part load histograms for fans; graphical for annual, tabular for annual and monthly'
    air_loop_summary_table[:header] = []
    air_loop_summary_table[:units] = []
    air_loop_summary_table[:data] = []

    # add table to array of tables
    air_loop_summary_tables << air_loop_summary_table

    return @air_loop_summary_section
  end

  # create plant_loop_summary section
  def self.plant_loop_summary_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    plant_loop_summary_tables = []

    # gather data for section
    @outdoor_air_section = {}
    @outdoor_air_section[:title] = 'Plant Loops Summary'
    @outdoor_air_section[:tables] = plant_loop_summary_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @outdoor_air_section
    end

    # create table
    plant_loop_summary_table = {}
    plant_loop_summary_table[:title] = 'Part load histograms for chillers, boilers, pumps'
    plant_loop_summary_table[:header] = []
    plant_loop_summary_table[:units] = []
    plant_loop_summary_table[:data] = []

    # add table to array of tables
    plant_loop_summary_tables << plant_loop_summary_table

    return @outdoor_air_section
  end

  # create outdoor_air_section
  def self.outdoor_air_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    outdoor_air_section_tables = []

    # gather data for section
    @outdoor_air_section = {}
    @outdoor_air_section[:title] = 'Outdoor Air'
    @outdoor_air_section[:tables] = outdoor_air_section_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @outdoor_air_section
    end

    # data for query
    report_name = 'OutdoorAirSummary'
    table_name = 'Average Outdoor Air During Occupied Hours'
    min_table_name = 'Minimum Outdoor Air During Occupied Hours'
    columns = ['', 'Average Number of Occupants', 'Nominal Number of Occupants', 'Zone Volume', 'Avg. Mechanical Ventilation', 'Min. Mechanical Ventilation', 'Avg. Infiltration', 'Min. Infiltration', 'Avg. Simple Ventilation', 'Min. Simple Ventilation']

    # populate dynamic rows
    rows_name_query = "SELECT DISTINCT  RowName FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}'"
    row_names = sqlFile.execAndReturnVectorOfString(rows_name_query).get
    rows = []
    row_names.each do |row_name|
      rows << row_name
    end

    # create table
    table = {}
    table[:title] = 'Average and Minimum Outdoor Air During Occupied Hours'
    table[:header] = columns
    source_units_volume = 'm^3'
    if is_ip_units
      target_units_volume = 'ft^3'
    else
      target_units_volume = 'm^3'
    end
    table[:units] =        ['', '', '', target_units_volume, 'ach', 'ach', 'ach', 'ach', 'ach', 'ach']
    table[:source_units] = ['', '', '', source_units_volume, 'ach', 'ach', 'ach', 'ach', 'ach', 'ach']
    table[:data] = []

    # run query and populate table
    rows.each do |row|
      row_data = [row]
      column_counter = -1
      table[:header].each do |header|
        column_counter += 1
        next if header == ''
        if header.include? 'Avg. '
          query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row}' and ColumnName= '#{header.gsub('Avg. ', '')}'"
          results = sqlFile.execAndReturnFirstDouble(query)
          row_data_ip = OpenStudio.convert(results.to_f, table[:source_units][column_counter], table[:units][column_counter]).get
          row_data << row_data_ip.round(4)
        elsif header.include? 'Min. '
          query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{min_table_name}' and RowName= '#{row}' and ColumnName= '#{header.gsub('Min. ', '')}'"
          results = sqlFile.execAndReturnFirstDouble(query)
          row_data_ip = OpenStudio.convert(results.to_f, table[:source_units][column_counter], table[:units][column_counter]).get
          row_data << row_data_ip.round(4)
        elsif header == 'Zone Volume'
          query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row}' and ColumnName= '#{header}'"
          results = sqlFile.execAndReturnFirstDouble(query)
          row_data_ip = OpenStudio.convert(results.to_f, table[:source_units][column_counter], table[:units][column_counter]).get
          row_data << row_data_ip.round(0)
        else
          query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row}' and ColumnName= '#{header}'"
          results = sqlFile.execAndReturnFirstDouble(query)
          row_data_ip = OpenStudio.convert(results.to_f, table[:source_units][column_counter], table[:units][column_counter]).get
          row_data << row_data_ip.round(4)
        end
      end

      table[:data] << row_data
    end

    # add table to array of tables
    outdoor_air_section_tables << table
    return @outdoor_air_section
  end

  # create cost_summary_section
  def self.cost_summary_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    cost_summary_section_tables = []

    # gather data for section
    @cost_summary_section = {}
    @cost_summary_section[:title] = 'Cash Flow'
    @cost_summary_section[:tables] = cost_summary_section_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @cost_summary_section
    end

    # order for fuel types
    yaxis_order = ['Electricity Purchased', 'Natural Gas', 'District Heating', 'District Cooling', 'Additional', 'Water', 'Capital and O&M']

    # create table
    cost_summary_table = {}
    cost_summary_table[:title] = 'Annual Cash Flow <br>(Not adjusted for inflation or utility escalation)'
    cost_summary_table[:header] = ['Year', 'Capital and O&M', 'Electricity', 'Natural Gas', 'District Heating', 'District Cooling', 'Additional', 'Water']
    cost_summary_table[:units] = ['', '$', '$', '$', '$', '$', '$', '$']
    cost_summary_table[:data] = []
    cost_summary_table[:chart_type] = 'vertical_stacked_bar'
    cost_summary_table[:chart_attributes] = { value: 'Annual Cash Flow ($)', label_x: 'Date', sort_yaxis: yaxis_order }
    cost_summary_table[:chart] = []

    # inflation approach
    inf_appr_query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='Life-Cycle Cost Report' AND ReportForString='Entire Facility' AND TableName='Life-Cycle Cost Parameters' AND RowName='Inflation Approach' AND ColumnName='Value'"
    inf_appr = sqlFile.execAndReturnFirstString(inf_appr_query)
    if inf_appr.is_initialized
      if inf_appr.get == 'ConstantDollar'
        inf_appr = 'Constant Dollar'
      elsif inf_appr.get == 'CurrentDollar'
        inf_appr = 'Current Dollar'
      else
        runner.registerWarning("Inflation approach: #{inf_appr.get} not recognized")
        return false
      end
      runner.registerValue('inflation_approach', inf_appr)
    else
      runner.registerWarning('Could not determine inflation approach used')
      return false
    end

    # base year
    base_yr_query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='Life-Cycle Cost Report' AND ReportForString='Entire Facility' AND TableName='Life-Cycle Cost Parameters' AND RowName='Base Date' AND ColumnName='Value'"
    base_yr = sqlFile.execAndReturnFirstString(base_yr_query)
    if base_yr.is_initialized
      if base_yr.get =~ /\d\d\d\d/
        base_yr = base_yr.get.match(/\d\d\d\d/)[0].to_f
      else
        runner.registerWarning("Could not determine the analysis start year from #{base_yr.get}")
        return false
      end
    else
      runner.registerWarning('Could not determine analysis start year')
      return false
    end

    # analysis length
    length_yrs_query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='Life-Cycle Cost Report' AND ReportForString='Entire Facility' AND TableName='Life-Cycle Cost Parameters' AND RowName='Length of Study Period in Years' AND ColumnName='Value'"
    length_yrs = sqlFile.execAndReturnFirstInt(length_yrs_query)
    if length_yrs.is_initialized
      length_yrs = length_yrs.get
      runner.registerValue('analysis_length', length_yrs, 'yrs')
    else
      runner.registerWarning('Could not determine analysis length')
      return false
    end

    # record the cash flow in these hashes
    cap_cash_flow = {}
    om_cash_flow = {}
    energy_cash_flow = {}
    electricity_cash_flow = {}
    gas_cash_flow = {}
    water_cash_flow = {}
    tot_cash_flow = {}
    additional_cash_flow = {}

    data_cashFlow = []
    data_running_total = []
    running_total = 0

    color = []
    color << '#DDCC77' # Electricity
    color << '#999933' # Natural Gas
    color << '#AA4499' # Additional Fuel
    color << '#88CCEE' # District Cooling
    color << '#CC6677' # District Heating
    color << '#332288' # Water (not used here but is in cash flow chart)
    color << '#117733' # Capital and O&M (not used here but is in cash flow chart)

    # loop through each year and record the cash flow
    for i in 0..(length_yrs - 1) do
      new_yr = base_yr + i
      yr = "January         #{new_yr.round}" # note: two spaces removed from earlier version of sql file
      ann_cap_cash = 0.0
      ann_om_cash = 0.0
      ann_energy_cash = 0.0
      ann_electricity_cash = 0.0
      ann_gas_cash = 0.0
      ann_dist_htg_cash = 0.0
      ann_dist_clg_cash = 0.0
      ann_water_cash = 0.0
      ann_tot_cash = 0.0

      # capital cash flow
      cap_cash_query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='Life-Cycle Cost Report' AND ReportForString='Entire Facility' AND TableName='Capital Cash Flow by Category (Without Escalation)' AND RowName='#{yr}' AND ColumnName='Total'"
      cap_cash = sqlFile.execAndReturnFirstDouble(cap_cash_query)
      if cap_cash.is_initialized
        ann_cap_cash += cap_cash.get
        ann_tot_cash += cap_cash.get
      end

      # o&m cash flow (excluding utility costs)
      om_types = ['Maintenance', 'Repair', 'Operation', 'Replacement', 'MinorOverhaul', 'MajorOverhaul', 'OtherOperational']
      om_types.each do |om_type|
        om_cash_query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='Life-Cycle Cost Report' AND ReportForString='Entire Facility' AND TableName='Operating Cash Flow by Category (Without Escalation)' AND RowName='#{yr}' AND ColumnName='#{om_type}'"
        om_cash = sqlFile.execAndReturnFirstDouble(om_cash_query)
        if om_cash.is_initialized
          ann_om_cash += om_cash.get
          ann_tot_cash += om_cash.get
        end
      end

      # energy cost cash flows (by fuel)
      electricity_purchased_query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='Life-Cycle Cost Report' AND ReportForString='Entire Facility' AND TableName='Energy and Water Cost Cash Flows (Without Escalation)' AND RowName='#{yr}' AND ColumnName='ElectricityPurchased'"
      electricity_purchased = sqlFile.execAndReturnFirstDouble(electricity_purchased_query)
      if electricity_purchased.is_initialized
        ann_electricity_cash += electricity_purchased.get
      end

      gas_query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='Life-Cycle Cost Report' AND ReportForString='Entire Facility' AND TableName='Energy and Water Cost Cash Flows (Without Escalation)' AND RowName='#{yr}' AND ColumnName='Gas'"
      gas = sqlFile.execAndReturnFirstDouble(gas_query)
      if gas.is_initialized
        ann_gas_cash += gas.get
      end

      dist_htg_query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='Life-Cycle Cost Report' AND ReportForString='Entire Facility' AND TableName='Energy and Water Cost Cash Flows (Without Escalation)' AND RowName='#{yr}' AND ColumnName='DistrictHeating'"
      dist_htg = sqlFile.execAndReturnFirstDouble(dist_htg_query)
      if dist_htg.is_initialized
        ann_dist_htg_cash += dist_htg.get
      end

      dist_clg_query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='Life-Cycle Cost Report' AND ReportForString='Entire Facility' AND TableName='Energy and Water Cost Cash Flows (Without Escalation)' AND RowName='#{yr}' AND ColumnName='DistrictCooling'"
      dist_clg = sqlFile.execAndReturnFirstDouble(dist_clg_query)
      if dist_clg.is_initialized
        ann_dist_clg_cash += dist_clg.get
      end

      # energy cash flow
      energy_cash_query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='Life-Cycle Cost Report' AND ReportForString='Entire Facility' AND TableName='Operating Cash Flow by Category (Without Escalation)' AND RowName='#{yr}' AND ColumnName='Energy'"
      energy_cash = sqlFile.execAndReturnFirstDouble(energy_cash_query)
      if energy_cash.is_initialized
        ann_energy_cash += energy_cash.get
        ann_tot_cash += energy_cash.get
      end

      # water cash flow
      water_cash_query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='Life-Cycle Cost Report' AND ReportForString='Entire Facility' AND TableName='Operating Cash Flow by Category (Without Escalation)' AND RowName='#{yr}' AND ColumnName='Water'"
      water_cash = sqlFile.execAndReturnFirstDouble(water_cash_query)
      if water_cash.is_initialized
        ann_water_cash += water_cash.get
        ann_tot_cash += water_cash.get
      end

      # log the values for this year
      cap_cash_flow[yr] = ann_cap_cash
      om_cash_flow[yr] = ann_om_cash
      electricity_cash_flow[yr] = ann_electricity_cash
      gas_cash_flow[yr] = ann_gas_cash
      energy_cash_flow[yr] = ann_energy_cash
      water_cash_flow[yr] = ann_water_cash
      tot_cash_flow[yr] = ann_tot_cash
      ann_additional_cash = ann_energy_cash - ann_electricity_cash - ann_gas_cash - ann_dist_htg_cash - ann_dist_clg_cash
      additional_cash_flow[yr] = ann_additional_cash

      # populate table row
      cost_summary_table[:data] << [yr, ann_cap_cash + ann_om_cash, ann_electricity_cash, ann_gas_cash, ann_dist_htg_cash, ann_dist_clg_cash, ann_additional_cash.round(2), ann_water_cash]

      # gather graph data
      if ann_electricity_cash > 0
        cost_summary_table[:chart] << JSON.generate(label: 'Electricity Purchased', label_x: yr, value: ann_electricity_cash, color: color[0])
      end
      if ann_gas_cash > 0
        cost_summary_table[:chart] << JSON.generate(label: 'Natural Gas', label_x: yr, value: ann_gas_cash, color: color[1])
      end
      if ann_additional_cash > 0
        cost_summary_table[:chart] << JSON.generate(label: 'Additional', label_x: yr, value: ann_additional_cash, color: color[2])
      end
      if ann_dist_clg_cash > 0
        cost_summary_table[:chart] << JSON.generate(label: 'District Cooling', label_x: yr, value: ann_dist_clg_cash, color: color[3])
      end
      if ann_dist_htg_cash > 0
        cost_summary_table[:chart] << JSON.generate(label: 'District Heating', label_x: yr, value: ann_dist_htg_cash, color: color[4])
      end
      if ann_water_cash > 0
        cost_summary_table[:chart] << JSON.generate(label: 'Water', label_x: yr, value: ann_water_cash, color: color[5])
      end
      if ann_cap_cash + ann_om_cash > 0
        cost_summary_table[:chart] << JSON.generate(label: 'Capital and O&M', label_x: yr, value: ann_cap_cash + ann_om_cash, color: color[6])
      end

      # gather running total data for line plot
      running_total += ann_tot_cash

    end # next year

    # add table to array of tables
    if running_total > 0
      cost_summary_section_tables << cost_summary_table
    else
      # don't make chart of no data to add to it.
      cost_summary_table[:chart] = []
    end

    return @cost_summary_section
  end

  # create source_energy_section
  def self.source_energy_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    source_energy_section_tables = []

    # gather data for section
    @source_energy_section = {}
    @source_energy_section[:title] = 'Site and Source Summary'
    @source_energy_section[:tables] = source_energy_section_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @source_energy_section
    end

    # data for query
    report_name = 'AnnualBuildingUtilityPerformanceSummary'
    table_name = 'Site and Source Energy'
    columns = ['', 'Total Energy', 'Energy Per Total Building Area', 'Energy Per Conditioned Building Area']
    rows = ['Total Site Energy', 'Net Site Energy', 'Total Source Energy', 'Net Source Energy']

    # create table
    source_energy_table = {}
    source_energy_table[:title] = table_name
    source_energy_table[:header] = columns
    source_units_total = 'GJ'
    source_units_area = 'MJ/m^2'
    if is_ip_units
      target_units_total = 'kBtu'
      target_units_area = 'kBtu/ft^2'
    else
      target_units_total = 'kWh'
      target_units_area = 'kWh/m^2'
    end
    source_energy_table[:units] = ['', target_units_total, target_units_area, target_units_area]
    source_energy_table[:source_units] = ['', source_units_total, source_units_area, source_units_area] # used for conversation, not needed for rendering.
    source_energy_table[:data] = []

    # run query and populate table
    rows.each do |row|
      row_data = [row]
      column_counter = -1
      source_energy_table[:header].each do |header|
        column_counter += 1
        next if header == ''
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row}' and ColumnName= '#{header}'"
        results = sqlFile.execAndReturnFirstDouble(query)
        row_data_ip = OpenStudio.convert(results.to_f, source_energy_table[:source_units][column_counter], source_energy_table[:units][column_counter]).get
        row_data << row_data_ip.round(1)
      end
      source_energy_table[:data] << row_data
    end

    # add table to array of tables
    source_energy_section_tables << source_energy_table

    # data for query
    report_name = 'AnnualBuildingUtilityPerformanceSummary'
    table_name = 'Site to Source Energy Conversion Factors'
    columns = ['', 'Site=>Source Conversion Factor']
    rows = ['Electricity', 'Natural Gas', 'District Cooling', 'District Heating'] # TODO: - complete this and add logic to skip row if not used in model

    # create table
    source_energy_table = {}
    source_energy_table[:title] = table_name
    source_energy_table[:header] = columns
    source_energy_table[:units] = []
    source_energy_table[:data] = []

    # run query and populate table
    rows.each do |row|
      row_data = [row]
      column_counter = -1
      source_energy_table[:header].each do |header|
        column_counter += 1
        next if header == ''
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row}' and ColumnName= '#{header}'"
        results = sqlFile.execAndReturnFirstDouble(query).to_f
        row_data << results.round(3)
      end
      source_energy_table[:data] << row_data
    end

    # add table to array of tables
    source_energy_section_tables << source_energy_table

    return @source_energy_section
  end

  # create co2_and_other_emissions_section
  def self.co2_and_other_emissions_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    co2_and_other_emissions_section_tables = []

    # gather data for section
    @co2_and_other_emissions_section = {}
    @co2_and_other_emissions_section[:title] = 'CO2 and Other Emissions'
    @co2_and_other_emissions_section[:tables] = co2_and_other_emissions_section_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @co2_and_other_emissions_section
    end

    # data for query
    report_name = 'EnergyMeters'
    table_name = 'Annual and Peak Values - Other by Weight/Mass'
    columns = ['', 'Annual Value', 'Minimum Value', 'Timestamp of Minimum', 'Maximum Value', 'Timestamp of Maximum']
    rows = ['Carbon Equivalent:Facility', 'CarbonEquivalentEmissions:Carbon Equivalent']

    # create table
    table = {}
    table[:title] = table_name
    table[:header] = columns
    source_units_total = 'kg'
    source_units_rate = 'kg/s'
    if is_ip_units
      target_units_total = 'lb'
      target_units_rate = 'lb/s'
    else
      target_units_total = 'kg'
      target_units_rate = 'kg/s'
    end
    table[:units] = ['', target_units_total, target_units_rate, '', target_units_rate, '']
    table[:source_units] = ['', source_units_total, source_units_rate, '', source_units_rate, ''] # used for conversation, not needed for rendering.
    table[:data] = []

    # run query and populate table
    rows.each do |row|
      row_data = [row]
      column_counter = -1
      table[:header].each do |header|
        column_counter += 1
        next if header == ''
        query = "SELECT Value FROM tabulardatawithstrings WHERE ReportName='#{report_name}' and TableName='#{table_name}' and RowName= '#{row}' and ColumnName= '#{header}'"
        if table[:source_units][column_counter] != ''
          results = sqlFile.execAndReturnFirstDouble(query)
          row_data_ip = OpenStudio.convert(results.to_f, table[:source_units][column_counter], table[:units][column_counter]).get
          row_data << row_data_ip.round(2)
        else
          results = sqlFile.execAndReturnFirstString(query)
          row_data << results
        end
      end

      table[:data] << row_data
    end

    # add table to array of tables
    co2_and_other_emissions_section_tables << table

    return @co2_and_other_emissions_section
  end

  # create typical_load_profiles_section
  def self.typical_load_profiles_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    typical_load_profiles_section_tables = []

    # gather data for section
    @typical_load_profiles_section = {}
    @typical_load_profiles_section[:title] = 'Typical Load Profiles'
    @typical_load_profiles_section[:tables] = typical_load_profiles_section_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @typical_load_profiles_section
    end

    # create table
    typical_load_profiles_table = {}
    typical_load_profiles_table[:title] = 'Content To Be Determined'
    typical_load_profiles_table[:header] = []
    typical_load_profiles_table[:units] = []
    typical_load_profiles_table[:data] = []

    # add table to array of tables
    typical_load_profiles_section_tables << typical_load_profiles_table

    return @typical_load_profiles_section
  end

  # create schedules_overview_section
  def self.schedules_overview_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    schedules_overview_section_tables = []

    # gather data for section
    @schedules_overview_section = {}
    @schedules_overview_section[:title] = 'Schedule Overview'
    @schedules_overview_section[:tables] = schedules_overview_section_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @schedules_overview_section
    end

    # create table
    schedules_overview_table = {}
    schedules_overview_table[:title] = ''
    schedules_overview_table[:header] = ['Schedule', 'Type Limits', 'Rules', 'Use Count']
    schedules_overview_table[:units] = []
    schedules_overview_table[:data] = []
    schedules_overview_table[:chart_type] = 'multi_step_line_grid'
    schedules_overview_table[:chart] = [] # for this chart type, this contains an array of charts

    # add table to array of tables
    schedules_overview_section_tables << schedules_overview_table

    model.getSchedules.sort.each do |schedule|
      next unless schedule.to_ScheduleRuleset.is_initialized
      schedule = schedule.to_ScheduleRuleset.get
      next if schedule.nonResourceObjectUseCount(true) == 0 # true excludes children

      # hash to hold chart
      chart = { chart_data: [], chart_attributes: {} }

      # get schedule and type limits
      type_limits = nil
      if schedule.scheduleTypeLimits.is_initialized
        type_limits = schedule.scheduleTypeLimits.get.unitType
      end

      # populate table with high level schedule information
      schedules_overview_table[:data] << [schedule.name, type_limits, schedule.scheduleRules.size, schedule.nonResourceObjectUseCount(true)]

      # array to hold profiles
      profiles = []

      # get default profile
      profiles << [schedule.defaultDaySchedule, 'default profile']

      # get design days
      summer_design = schedule.summerDesignDaySchedule
      profiles << [summer_design, 'summer design day']
      winter_design = schedule.winterDesignDaySchedule
      profiles << [winter_design, 'winter design day']

      # get rules
      schedule.scheduleRules.each do |rule|
        # add days of week to text
        rule.applySunday ? (sun = 'Sun') : (sun = '')
        rule.applyMonday ? (mon = 'Mon') : (mon = '')
        rule.applyTuesday ? (tue = 'Tue') : (tue = '')
        rule.applyWednesday ? (wed = 'Wed') : (wed = '')
        rule.applyThursday ? (thu = 'Thu') : (thu = '')
        rule.applyFriday ? (fri = 'Fri') : (fri = '')
        rule.applySaturday ? (sat = 'Sat') : (sat = '')

        # add dates to text
        if rule.startDate.is_initialized
          date = rule.startDate.get
          start = date
        else
          start = ''
        end
        if rule.endDate.is_initialized
          date = rule.endDate.get
          finish = date
        else
          finish = ''
        end

        text = "(#{sun}#{mon}#{tue}#{wed}#{thu}#{fri}#{sat}) #{start}-#{finish}"
        profiles << [rule.daySchedule, text]
      end

      # rule colors by index (starting with default, then summer, winter, then p1,p2, etc)
      rule_colors = ['#88CCEE', 'red', 'blue', '#AA4499', '#332286', '#117733', '#99995B', '#DDCC77', '#CC6677', '#882255', '#6699CC', '#661100', '#AA4466', '#505050']

      # temp test of profiles
      profile_counter = -2
      profiles.each do |array|
        profile = array[0]
        text = array[1]

        if profile_counter == -2
          name = text.to_s
        elsif profile_counter < 1
          name = " #{text}"
        else
          name = "Priority #{profile_counter} - #{text}"
        end

        # update counter
        profile_counter += 1

        times = profile.times
        values = profile.values
        (1..times.size).each do |index|
          # add for this index value
          time_double = times[index - 1].hours + times[index - 1].minutes / 60.0
          value = values[index - 1]

          # populate chart with datapoint
          # chart[:chart_data] << JSON.generate({:label => name, :label_x => time_double, :value => value, :color => rule_colors[profile_counter+1]})
        end

        # add datapoint for 24
        time = OpenStudio::Time.new(0, 24, 0, 0)
        val = profile.getValue(time)
        # chart[:chart_data] << JSON.generate({:label => name, :label_x => 24.0, :value => val, :color => rule_colors[profile_counter+1]})

        # add datapoint for 0
        time = OpenStudio::Time.new(0, 0, 0, 0)
        val = profile.getValue(time)
        # chart[:chart_data] << JSON.generate({:label => name, :label_x => 0.0, :value => val, :color => rule_colors[profile_counter+1]})

        # get datapoint every 15min (doing this because I could get step to work just with input profile values)
        (1..24).each do |i|
          fractional_hours = i / 1.0

          hr = fractional_hours.truncate
          min = ((fractional_hours - fractional_hours.truncate) * 60.0).truncate

          time = OpenStudio::Time.new(0, hr, min, 0)
          val = profile.getValue(time)

          # add unit conversion depending on type limits
          if type_limits == 'Temperature'
            if is_ip_units
              val = OpenStudio.convert(val, 'C', 'F').get
            else
              val = OpenStudio.convert(val, 'C', 'C').get
            end
          end

          # populate chart with datapoint
          chart[:chart_data] << JSON.generate(label: name, label_x: fractional_hours, value: val, color: rule_colors[profile_counter + 1])
        end
      end

      # populate chart attributes
      if type_limits == 'Temperature'
        if is_ip_units
          chart[:chart_attributes][:value] = "#{type_limits} (F)"
        else
          chart[:chart_attributes][:value] = "#{type_limits} (C)"
        end
      elsif type_limits == 'ActivityLevel'
        chart[:chart_attributes][:value] = "#{type_limits} (W)"
      else
        chart[:chart_attributes][:value] = type_limits
      end
      chart[:chart_attributes][:label_x] = 'Hours'
      chart[:chart_attributes][:title] = schedule.name

      # push chart to array of charts
      schedules_overview_table[:chart] << chart
    end

    return @schedules_overview_section
  end

  # create measure_warning_section (creates tables and runner.registerValues)
  def self.measure_warning_section(model, sqlFile, runner, name_only = false, is_ip_units = true)
    # array to hold tables
    measure_tables = []

    # gather data for section
    @measure_warnings_section = {}
    @measure_warnings_section[:title] = 'Measure Warnings'
    @measure_warnings_section[:tables] = measure_tables

    # stop here if only name is requested this is used to populate display name for arguments
    if name_only == true
      return @measure_warnings_section
    end

    # will be used for registerValues
    num_measures_with_warnings = 0
    num_warnings = 0
    num_measures = 0

    # loop through workflow steps
    runner.workflow.workflowSteps.each do |step|
      if step.to_MeasureStep.is_initialized
        measure_step = step.to_MeasureStep.get
        measure_name = measure_step.measureDirName
        num_measures += 1
        if measure_step.name.is_initialized
          measure_name = measure_step.name.get # this is instance name in PAT
        end
        if measure_step.result.is_initialized
          result = measure_step.result.get
          # create and populate table if warnings exist
          if !result.warnings.empty?
            measure_table_01 = {}
            measure_table_01[:title] = measure_name
            measure_table_01[:header] = ['Warning']
            measure_table_01[:data] = []
            num_measures_with_warnings += 1

            # step through warnings
            start_counter = num_warnings
            result.warnings.each do |step|
              # add rows to table and register value
              num_warnings += 1
              if num_warnings < start_counter + 25
                measure_table_01[:data] << [step.logMessage]
              elsif num_warnings == start_counter + 25
                measure_table_01[:data] << ["* See OSW file for full list of warnings. This measure has #{result.warnings.size} warnings."]
              end
            end
            # add table to section
            measure_tables << measure_table_01
          end
        else
          # puts "No result for #{measure_name}"
        end
      else
        # puts "This step is not a measure"
      end
    end

    # check for warnings in openstudio_results and add table for it if there are warnings.
    if !runner.result.stepWarnings.empty?
      measure_table_os_results = {}
      measure_table_os_results[:title] = 'OpenStudio Results'
      measure_table_os_results[:header] = ['Warning']
      measure_table_os_results[:data] = []
      num_measures_with_warnings += 1
      runner.result.stepWarnings.each do |warning|
        measure_table_os_results[:data] << [warning]
        num_warnings += 1
      end
      measure_tables << measure_table_os_results
    end

    # add summary table (even when there are no warnings)
    measure_table_summary = {}
    measure_table_summary[:title] = 'Measure Warning Summary'
    measure_table_summary[:header] = ['Description', 'Count']
    measure_table_summary[:data] = []

    # add summary rows
    measure_table_summary[:data] << ['Number of measures in workflow', num_measures]
    measure_table_summary[:data] << ['Number of measures with warnings', num_measures_with_warnings]
    measure_table_summary[:data] << ['Total number of warnings', num_warnings]

    # add table to section
    measure_tables.insert(0, measure_table_summary)

    runner.registerValue('number_of_measures_with_warnings', num_measures_with_warnings)
    runner.registerValue('number_warnings', num_warnings)

    return @measure_warnings_section
  end
end
