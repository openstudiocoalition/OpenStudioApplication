########################################################################################################################
#  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
#  See also https://openstudiocoalition.org/about/software_license/
########################################################################################################################

original_path = ENV['PATH'] # add binary dir to system path
original_dll_directory = nil
platform_specific_path = nil

if /mswin/.match(RUBY_PLATFORM) or /mingw/.match(RUBY_PLATFORM)

  require 'fiddle/import'
  require 'fiddle/types'
  module WinAPI
    extend Fiddle::Importer
    dlload 'kernel32.dll'
    include Fiddle::Win32Types
    extern 'BOOL SetDllDirectory(LPCSTR)'
    extern 'DWORD GetDllDirectory(DWORD, LPSTR)'
  end

  buffer = 1024
  original_dll_directory = Fiddle::Pointer.malloc(buffer)
  WinAPI.GetDllDirectory(buffer, original_dll_directory)

  qt_dll_path = File.expand_path(File.join(File.dirname(__FILE__), '../bin/'))

  # if install path fails, try developer paths
  if !File.exist?(File.join(qt_dll_path, 'Qt6Core.dll'))
    release_dll_path = File.expand_path(File.join(File.dirname(__FILE__), '../../Release/'))
    debug_dll_path = File.expand_path(File.join(File.dirname(__FILE__), '../../Debug/'))
    if File.exist?(File.join(release_dll_path, 'Qt6Core.dll'))
      qt_dll_path = release_dll_path
    elsif File.exist?(File.join(debug_dll_path, 'Qt6Core.dll'))
      qt_dll_path = debug_dll_path
    end
  end

  WinAPI.SetDllDirectory(qt_dll_path)

  $OPENSTUDIO_APPLICATION_DIR = qt_dll_path
else

  # Do something here for Mac OSX environments
  qt_so_path = File.expand_path(File.join(File.dirname(__FILE__)))
  ENV['PATH'] = "#{qt_so_path}:#{original_path}"

  $OPENSTUDIO_APPLICATION_DIR = File.join(File.dirname(__FILE__), '../bin/')
end

begin

  # require openstudio
  begin
    require_relative 'openstudio'
  rescue LoadError
    begin
      require 'openstudio'
    rescue LoadError
      puts 'Unable to require openstudio'
    end
  end

  # require openstudio_modeleditor.so
  require_relative 'openstudio_modeleditor.so'

  # add this directory to Ruby load path
  $:.unshift(File.expand_path(File.dirname(__FILE__)))

ensure

  # restore original path
  ENV['PATH'] = original_path

  if original_dll_directory
    WinAPI.SetDllDirectory(original_dll_directory)
  end

end
