#!/usr/bin/env ruby

require 'pathname'

ROOT_DIR = File.absolute_path(File.join(File.dirname(__FILE__), "../../"))
SDK_CMAKE_PATH = File.join(ROOT_DIR, 'FindOpenStudioSDK.cmake')

# Parse the CMake stuff for the current version of the SDK
# It will raise if something goes wrong
#
# @param cmake_path [String] The path to FindOpenStudioSDK.cmake
# @return [String] the resulting version, eg "2.8.0".
def parse_version_from_cmake(cmake_path)
  major_str = nil
  minor_str = nil
  patch_str = nil
  File.open(cmake_path) do |f|
    f.each_line do |line|
      m = line.match(/OPENSTUDIO_VERSION_MAJOR\s+(\d+)/)
      if m
        if !major_str.nil?
          raise "Expected to find only one OPENSTUDIO_VERSION_MAJOR in #{cmake_path}"
        end
        major_str = m.captures[0]
      end

      m = line.match(/OPENSTUDIO_VERSION_MINOR\s+(\d+)/)
      if m
        if !minor_str.nil?
          raise "Expected to find only one OPENSTUDIO_VERSION_MINOR in #{cmake_path}"
        end
        minor_str = m.captures[0]
      end

      m = line.match(/OPENSTUDIO_VERSION_PATCH\s+(\d+)/)
      if m
        if !patch_str.nil?
          raise "Expected to find only one OPENSTUDIO_VERSION_PATCH in #{cmake_path}"
        end
        patch_str = m.captures[0]
      end
    end
  end

  if major_str.nil?
    raise "Couldn't find 'set(OPENSTUDIO_VERSION_MAJOR' in #{cmake_path}"
  elsif minor_str.nil?
    raise "Couldn't find 'set(OPENSTUDIO_VERSION_MINOR' in #{cmake_path}"
  elsif patch_str.nil?
    raise "Couldn't find 'set(OPENSTUDIO_VERSION_PATCH' in #{cmake_path}"
  end
  os_version_str = "#{major_str}.#{minor_str}.#{patch_str}"
  return os_version_str
end

# Find all OSMs of interest
#
# @param None
# @return [Array] Array of paths to the OSMs
def find_resource_osms()

  # Glob all OSMs
  path = File.join(ROOT_DIR, 'src/**/*.osm')
  files = Dir.glob(path)
  # Only keep the one we're interested in
  files = files.grep(/openstudio_app\/Resources|sketchup_plugin\/resources\/templates|sketchup_plugin\/user_scripts/)
  return files

end


# Parse the Version object in an OSM
# Does this by loading the file and grepping the version object (does not rely
# on 'openstudio' bindings)
# Will raise if something goes wrong
#
# @param cmake_path [osm_path] The path to OSM
# @return [String] the resulting version, eg "2.8.0"
def parse_osm_version(osm_path)

  this_version = File.read(osm_path).scan(/OS:Version,.*?(\d+\.\d+\.\d+)\s*;/m)

  if !((this_version.class == Array) && (this_version.size == 1) &&
       (this_version[0].class == Array) && (this_version[0].size == 1))
    raise "Something went wrong looking for version in #{osm_path},"
          "found: #{this_version}"
  end

  osm_version_str = this_version[0][0]

  return osm_version_str
end

def check_all_osm_versions(all_osms, expected_version_str)

  mismatches = []
  all_osms.each do |fpath|

    # Path from project root, for reporting
    frelpath = Pathname.new(fpath).relative_path_from(Pathname.new(ROOT_DIR)).to_s

    osm_version_str = parse_osm_version(fpath)

    if osm_version_str != expected_version_str
      puts "Version mistmatch for #{frelpath}, found: #{osm_version_str}, expected #{expected_version_str}"
      mismatches << fpath
    end
  end

  return mismatches
end
