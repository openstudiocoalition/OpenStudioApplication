########################################################################################################################
#  OpenStudio(R), Copyright (c) 2020-2021, OpenStudio Coalition and other contributors. All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
#  following conditions are met:
#
#  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
#  disclaimer.
#
#  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
#  disclaimer in the documentation and/or other materials provided with the distribution.
#
#  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
#  derived from this software without specific prior written permission from the respective party.
#
#  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
#  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
#  written permission from Alliance for Sustainable Energy, LLC.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
#  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
#  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
#  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
#  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
########################################################################################################################

# add binary dir to system path
original_path = ENV['PATH']
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
  if !File.exists?(File.join(qt_dll_path, 'Qt5Core.dll'))
    release_dll_path = File.expand_path(File.join(File.dirname(__FILE__), '../../Release/'))
    debug_dll_path = File.expand_path(File.join(File.dirname(__FILE__), '../../Debug/'))
    if File.exists?(File.join(release_dll_path, 'Qt5Core.dll'))
      qt_dll_path = release_dll_path
    elsif File.exists?(File.join(debug_dll_path, 'Qt5Core.dll'))
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
