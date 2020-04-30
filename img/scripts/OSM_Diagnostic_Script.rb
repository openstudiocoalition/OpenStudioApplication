######################################################################
#  Copyright (c) 2008-2010, Alliance for Sustainable Energy.
#  All rights reserved.
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
######################################################################

# Each user script is implemented within a class that derives from OpenStudio::Ruleset::UserScript
class DiagnosticScript < OpenStudio::Ruleset::ModelUserScript

  # override name to return the name of your script
  def name
    return "OSM Diagnostic Script"
  end

  # returns a vector of arguments, the runner will present these arguments to the user
  # then pass in the results on run
  def arguments(model)
    result = OpenStudio::Ruleset::UserScriptArgumentVector.new

    open_path = OpenStudio::Ruleset::UserScriptArgument::makePathArgument("open_path", true, "osm")
    open_path.setDisplayName("Select OSM File to Run Diagnostics On ")
    result << open_path

    begin
      Sketchup.send_action("showRubyPanel:")
    rescue => e
    end

    return result
  end

  # override run to implement the functionality of your script
  # model is an OpenStudio::Model::Model, runner is a OpenStudio::Ruleset::UserScriptRunner
  def run(model, runner, arguments)

    open_path = arguments["open_path"]
    return false if not open_path

    open_path = open_path.valueAsString

    require 'openstudio'

    puts ""
    puts ">>diagnostic test start"

    puts ""
    puts "File - " + open_path

    idfFile = OpenStudio::IdfFile::load(OpenStudio::Path.new(open_path)).get
    workspace = OpenStudio::Workspace.new(idfFile, "None".to_StrictnessLevel)

    # Open OSM file
    model = OpenStudio::Model::Model::load(OpenStudio::Path.new(open_path)).get

    puts "Model has " + model.numObjects.to_s + " objects"

    # number of thermal zones
    thermal_zones = model.getThermalZones
    puts "Model has " + thermal_zones.size.to_s + " thermal zones"

    # number of spaces
    spaces = model.getSpaces
    puts "Model has " + spaces.size.to_s + " spaces"

    # number of surfaces
    surfaces = model.getPlanarSurfaces
    # puts "Model has " + surfaces.size.to_s + " planar surfaces"

    # number of base surfaces
    base_surfaces = model.getSurfaces
    puts "Model has " + base_surfaces.size.to_s + " base surfaces"

    # number of base surfaces
    sub_surfaces = model.getSubSurfaces
    puts "Model has " + sub_surfaces.size.to_s + " sub surfaces"

    # number of surfaces
    shading_surfaces = model.getShadingSurfaces
    puts "Model has " + shading_surfaces.size.to_s + " shading surfaces"

    # number of surfaces
    partition_surfaces = model.getInteriorPartitionSurfaces
    puts "Model has " + partition_surfaces.size.to_s + " interior partition surfaces"

#    puts ""
#    puts "Outputting Draft Validity Report"
#    puts workspace.validityReport("Draft".to_StrictnessLevel)
#
#    puts ""
#    puts "Outputting Final Validity Report (note: some errors are expected here)"
#    puts workspace.validityReport("Final".to_StrictnessLevel)

    remove_errors = true # set to false if you don't want to create a diagnostic copy of your file
    remove_warnings = true # set to false if you don't want to create a diagnostic copy of your file

    savediagnostic = false # this will change to true later in script if necessary

    puts ""
    puts "Removing catchall objects (objects unknown to your version of OpenStudio)"
    switch = 0
    model.getObjectsByType("Catchall".to_IddObjectType).each do |obj|
      puts "*(error) '" + obj.getString( 0 ).to_s + "' object type is unkown to OpenStudio"
      switch = 1
      if remove_errors
        puts "**(removing object)  '#{obj.getString( 0 ).to_s}'"
        remove = obj.remove
        savediagnostic = true
      end
    end
    if switch == 0 then puts "none" end

    puts ""
    puts "Removing objects that fail draft validity test"
    switch = 0
    model.objects.each do |object|
      if !object.isValid("Draft".to_StrictnessLevel)
        report = object.validityReport("Draft".to_StrictnessLevel)
        puts "*(error)" + report.to_s
        switch = 1
        if remove_errors
          puts "**(removing object)  '#{object.name}'"
          remove = object.remove
          savediagnostic = true
        end
      end
    end
    if switch == 0 then puts "none" end
    
    base_surfaces = model.getSurfaces
    # Find base surfaces with area < 0.1
    puts ""
    puts "Surfaces with area less than 0.1 m^2"
    switch = 0
    base_surfaces.each do |base_surface|
      grossarea = base_surface.grossArea
      if grossarea < 0.1
        puts "*(warning) '" + base_surface.name.to_s + "' has area of " + grossarea.to_s + " m^2"
        switch = 1
        if remove_warnings
          puts "**(removing object) '#{base_surface.name.to_s}'"
          # remove surfaces with area < 0.1
          remove = base_surface.remove
          savediagnostic = true
        end
      end
    end
    if switch == 0 then puts "none" end

    sub_surfaces = model.getSubSurfaces
    # Find sub surfaces with area < 0.1
    puts ""
    puts "SubSurfaces with area less than 0.1 m^2"
    switch = 0
    sub_surfaces.each do |sub_surface|
      grossarea = sub_surface.grossArea
      if grossarea < 0.1
        puts "*(warning) '" + sub_surface.name.to_s + "' has area of " + grossarea.to_s + " m^2"
        switch = 1
        if remove_warnings
          puts "**(removing object) '#{sub_surface.name.to_s}'"
          # remove surfaces with area < 0.1
          remove = sub_surface.remove
          savediagnostic = true
        end
      end
    end
    if switch == 0 then puts "none" end

    surfaces = model.getSurfaces
    # Find surfaces with greater than 25 vertices
    puts ""
    puts "Surfaces with more than 25 vertices"
    switch = 0
    surfaces.each do |surface|
      vertexcount = surface.vertices.size
      if vertexcount > 25
        puts "*(warning) '" + surface.name.to_s + "' has " + vertexcount.to_s + " vertices"
        switch = 1
      end
    end
    if switch == 0 then puts "none" end

    # Find surfaces within surface groups that share same vertices
    puts ""
    puts "Surfaces and SubSurfaces which have similar geometry within same surface group"
    switch = 0
    planar_surface_groups = model.getPlanarSurfaceGroups
    planar_surface_groups.each do |planar_surface_group|

      planar_surfaces = []
      planar_surface_group.children.each do |child|
        planar_surface = child.to_PlanarSurface
        next if planar_surface.empty?
        planar_surfaces << planar_surface.get
      end

      n = planar_surfaces.size

      sub_surfaces = []
      (0...n).each do |k|
        planar_surfaces[k].children.each do |l|
          sub_surface = l.to_SubSurface
          next if sub_surface.empty?
          sub_surfaces << sub_surface.get
        end
      end

      all_surfaces = []
      sub_surfaces.each do |m|  # subsurfaces first so they get removed vs. base surface
        all_surfaces << m
      end
      planar_surfaces.each do |n|
        all_surfaces << n
      end

      n2 = all_surfaces.size # updated with sub-surfaces added at the beginning

      (0...n2).each do |i|
        (i+1...n2).each do |j|

          p1 = all_surfaces[i]
          p2 = all_surfaces[j]
       
          if p1.equalVertices(p2) or p1.reverseEqualVertices(p2)
            switch = 1
            puts "*(error) '#{p1.name.to_s}' has similar geometry to '#{p2.name.to_s}' in the surface group named '#{planar_surface_group.name.to_s}'"
            puts "**(removing object) '#{p1.name.to_s}'" # remove p1 vs. p2 to avoid failure if three or more similar surfaces in a group
            remove = p1.remove
            savediagnostic = true
          end

        end
      end

    end
    if switch == 0 then puts "none" end

    puts ""
    puts ">>diagnostic test complete"

    if savediagnostic
      newfilename = open_path.gsub(".osm","_diagnostic.osm")
      if File.exists? newfilename
        # I would like to add a prompt to ask the user if they want to overwrite their file
      end
      puts ""
      puts ">>saving temporary diagnostic version " + newfilename
      model.save(OpenStudio::Path.new(newfilename),true)

    end

    runner.okPrompt("Diognostic Test Complete")
  end

end

# this call registers your script with the OpenStudio SketchUp plug-in
DiagnosticScript.new.registerWithSketchUp
