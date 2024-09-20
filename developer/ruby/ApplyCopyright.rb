# Inserts copyright at beginning of each
#
# Inputs:
#   ARGV[0] - path to top level cmake source directory (one level above 'src' directory)

require 'fileutils'
require 'pathname'

include FileUtils

ROOT_DIR = Pathname.new(__dir__).parent.parent

license_lines = [
  "OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.\n",
  "See also https://openstudiocoalition.org/about/software_license/\n",
]

copyright = "/***********************************************************************************************************************\n"
ruby_copyright = "########################################################################################################################\n"
license_lines.each do |line|
  if line.strip.empty?
    copyright += '*' + line
    ruby_copyright += '#' + line

  else
    copyright += '*  ' + line
    ruby_copyright += '#  ' + line
  end
end
copyright += "***********************************************************************************************************************/\n\n"
ruby_copyright += "########################################################################################################################\n\n"

# first do c++

# exclusions are files that are not part of OpenStudio
folder_exclusions = [
  ROOT_DIR / 'src/qtwinmigrate',
]
filename_exclusions = [
  'mainpage.hpp',
]

# glob for hpp and cpp
files = ROOT_DIR.glob('src/**/*.[ch]pp')
files += ROOT_DIR.glob('ruby/**/*.[ch]pp')
files += ROOT_DIR.glob('src/**/*.[ch]xx.in')
files += ROOT_DIR.glob('src/**/*.tmp')

# reject exceptions
files.reject! do |p|
  filename_exclusions.any? { |fname| p.basename.to_s == fname } ||
    p.ascend { |path| break true if folder_exclusions.any? { |p2| path == p2 } }
end

# loop over all files
files.each do |p|
  # Read lines and remove copyright
  lines = p.readlines
  lines.shift(lines.find_index { |line| !(line.chomp.empty? || %r{^\s?[/*]}.match(line)) })

  # write file
  p.write(copyright + lines.join)
end

# now do ruby

# exceptions are files that are not part of OpenStudio
folder_exclusions = []
filename_exclusions = []

# glob for rb
files = ROOT_DIR.glob('ruby/**/*.rb')

# reject exceptions
files.reject! do |p|
  filename_exclusions.any? { |fname| p.basename.to_s == fname } ||
    p.ascend { |path| break true if folder_exclusions.any? { |p2| path == p2 } }
end

# loop over all files
files.each do |p|
  # Read lines and remove copyright
  lines = p.readlines
  lines.shift(lines.find_index { |line| !(line.chomp.empty? || /^#/.match(line)) })

  # write file
  p.write(ruby_copyright + lines.join)
end
