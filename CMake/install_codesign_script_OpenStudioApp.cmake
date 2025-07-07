#[=======================================================================[.rst:
install_codesign_script_OpenStudioApp
-------------------------------------

This file is meant to be used up as a ``install(SCRIPT)``

Pre-conditions:

- It requires that you're running on ``APPLE``

- **Important: In the scope of this** ``install(SCRIPT)``, you must define

    * :cmake:variable:`CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION`
    * :cmake:variable:`CPACK_CODESIGNING_MACOS_IDENTIFIER`

**In the scope** meaning that you have to issue some ``install(CODE ..)`` commands prior to calling it, and they must be:

    * Part of the same ``project()``
    * Using the same CPack ``COMPONENT``

This script will codesign the ``FILES_TO_SIGN`` that is defined in this file as well as some globs.

To do so, it uses the `CodeSigning`_ functions :cmake:command:`codesign_files_macos`

This script will therefore run in the CPack staging area, and should be added after installation of root dylibs (they may come from fixup_executable)
after any rpath adjustments, to ensure the signature sticks.

Usage::

  if(APPLE AND CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION)
    install(CODE "set(CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION \"${CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION}\")" COMPONENT OpenStudioApp)
    install(CODE "set(CPACK_CODESIGNING_MACOS_IDENTIFIER \"${CPACK_CODESIGNING_MACOS_IDENTIFIER}\")" COMPONENT OpenStudioApp)
    # call the script
    install(SCRIPT "${CMAKE_CURRENT_LIST_DIR}/install_codesign_script_OpenStudioApp.cmake" COMPONENT OpenStudioApp)
  endif()
#]=======================================================================]

#------------------------------------------------------------------------------
# Just a helper
function(print_relative_paths)
  set(prefix "")
  set(valueLessKeywords NAME_ONLY NEWLINE)
  set(singleValueKeywords PREFIX BASE_DIRECTORY)
  set(multiValueKeywords ABSOLUTE_PATHS)
  cmake_parse_arguments(
    PARSE_ARGV 0 # Start at one with NAME is the first param
      "${prefix}"
      "${valueLessKeywords}"
      "${singleValueKeywords}"
      "${multiValueKeywords}"
  )

  if (_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Extra unknown arguments were passed: ${_UNPARSED_ARGUMENTS}")
  endif()
  if (_KEYWORDS_MISSING_VALUES)
    message(FATAL_ERROR "Keywords missing values: ${_KEYWORDS_MISSING_VALUES}")
  endif()

  if(NOT _BASE_DIRECTORY)
    set(_BASE_DIRECTORY ${CMAKE_INSTALL_PREFIX})
  endif()

  foreach(path ${_ABSOLUTE_PATHS})
    if(_NAME_ONLY)
      cmake_path(GET path FILENAME rel_path)
    else()
      cmake_path(RELATIVE_PATH path BASE_DIRECTORY ${_BASE_DIRECTORY} OUTPUT_VARIABLE rel_path)
    endif()
    list(APPEND rel_paths ${rel_path})
  endforeach()

  if(_NEWLINE)
    message("${_PREFIX}")
    foreach(path ${rel_paths})
      message("   - ${path}")
    endforeach()
  else()
    message("${_PREFIX}${rel_paths}")
  endif()
endfunction()
#------------------------------------------------------------------------------

message("Codesigning inner executables and library from ${CMAKE_CURRENT_LIST_FILE}")

message("CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")
message("CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION=${CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION}")
message("CPACK_CODESIGNING_MACOS_IDENTIFIER=${CPACK_CODESIGNING_MACOS_IDENTIFIER}")
message("CMAKE_INSTALL_COMPONENT=${CMAKE_INSTALL_COMPONENT}")
message("CPACK_GENERATOR=${CPACK_GENERATOR}")

if(NOT CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION)
  message(FATAL_ERROR "CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION is required")
endif()

if(NOT CPACK_CODESIGNING_MACOS_IDENTIFIER)
  message(FATAL_ERROR "CPACK_CODESIGNING_MACOS_IDENTIFIER is required")
endif()


set(FILES_TO_SIGN
  EnergyPlus/ExpandObjects
  EnergyPlus/energyplus-25.1.0
  EnergyPlus/energyplus.1
  EnergyPlus/libenergyplusapi.25.1.0.dylib
  EnergyPlus/libintl.8.dylib
  EnergyPlus/libpython3.12.dylib
  Ruby/openstudio.bundle

  Radiance/bin/vinfo
  Radiance/bin/pcompos
  Radiance/bin/dgraph
  Radiance/bin/optics2rad
  Radiance/bin/pmapdump
  Radiance/bin/bsdf2klems
  Radiance/bin/gensurf
  Radiance/bin/ra_gif
  Radiance/bin/xglaresrc
  Radiance/bin/histo
  Radiance/bin/libxcb.1.dylib
  Radiance/bin/ranimate
  Radiance/bin/eplus_adduvf
  Radiance/bin/glaze
  Radiance/bin/rlam
  Radiance/bin/neaten
  Radiance/bin/oconv
  Radiance/bin/normtiff
  Radiance/bin/pfilt
  Radiance/bin/ltpict.pl
  Radiance/bin/rhoptimize
  Radiance/bin/mgf2meta
  Radiance/bin/ra_bmp
  Radiance/bin/genworm
  Radiance/bin/gendaylit
  Radiance/bin/libXplugin.1.dylib
  Radiance/bin/rhcopy
  Radiance/bin/igraph
  Radiance/bin/wrapBSDF
  Radiance/bin/pvalue
  Radiance/bin/rcollate
  Radiance/bin/genambpos
  Radiance/bin/rcalc
  Radiance/bin/pinterp
  Radiance/bin/obj2mesh
  Radiance/bin/pdfblur
  Radiance/bin/falsecolor
  Radiance/bin/vwright
  Radiance/bin/ra_rgbe
  Radiance/bin/rfluxmtx
  Radiance/bin/libICE.6.dylib
  Radiance/bin/fieldcomb
  Radiance/bin/rpiece
  Radiance/bin/pcwarp
  Radiance/bin/3ds2mgf
  Radiance/bin/psign
  Radiance/bin/libX11.6.dylib
  Radiance/bin/bgraph
  Radiance/bin/ev
  Radiance/bin/genprism
  Radiance/bin/vwrays
  Radiance/bin/libglapi.0.dylib
  Radiance/bin/bsdf2ttree
  Radiance/bin/pgblur
  Radiance/bin/mgf2inv
  Radiance/bin/phisto
  Radiance/bin/rhinfo
  Radiance/bin/meta2tga
  Radiance/bin/ra_tiff
  Radiance/bin/trad
  Radiance/bin/objpict
  Radiance/bin/ies2rad
  Radiance/bin/plotin
  Radiance/bin/protate
  Radiance/bin/plot4
  Radiance/bin/replmarks
  Radiance/bin/findglare
  Radiance/bin/genbox
  Radiance/bin/pextrem
  Radiance/bin/gensky
  Radiance/bin/xform
  Radiance/bin/pexpand
  Radiance/bin/genBSDF
  Radiance/bin/rcontrib
  Radiance/bin/ra_hexbit
  Radiance/bin/mgf2rad
  Radiance/bin/rad2mgf
  Radiance/bin/libxcb-glx.0.dylib
  Radiance/bin/rtrace
  Radiance/bin/rhpict
  Radiance/bin/libXfixes.3.dylib
  Radiance/bin/getbbox
  Radiance/bin/lampcolor
  Radiance/bin/cnt
  Radiance/bin/ra_xyze
  Radiance/bin/total
  Radiance/bin/genskyvec
  Radiance/bin/epw2wea
  Radiance/bin/pmdblur
  Radiance/bin/objline
  Radiance/bin/ximage
  Radiance/bin/libX11-xcb.1.dylib
  Radiance/bin/psort
  Radiance/bin/pkgBSDF
  Radiance/bin/pbilat
  Radiance/bin/ttyimage
  Radiance/bin/libXdmcp.6.dylib
  Radiance/bin/lookamb
  Radiance/bin/ra_ppm
  Radiance/bin/libXau.6.dylib
  Radiance/bin/mkpmap
  Radiance/bin/pmblur
  Radiance/bin/tmesh2rad
  Radiance/bin/rlux
  Radiance/bin/psmeta
  Radiance/bin/libXext.6.dylib
  Radiance/bin/objview
  Radiance/bin/libXxf86vm.1.dylib
  Radiance/bin/glarendx
  Radiance/bin/ra_t8
  Radiance/bin/libGL.1.dylib
  Radiance/bin/libXdamage.1.dylib
  Radiance/bin/raddepend
  Radiance/bin/genklemsamp
  Radiance/bin/pflip
  Radiance/bin/icalc
  Radiance/bin/dctimestep
  Radiance/bin/ra_ps
  Radiance/bin/tabfunc
  Radiance/bin/gendaymtx
  Radiance/bin/cv
  Radiance/bin/rholo
  Radiance/bin/mkillum
  Radiance/bin/rvu
  Radiance/bin/glrad
  Radiance/bin/obj2rad
  Radiance/bin/pcomb
  Radiance/bin/rmtxop
  Radiance/bin/genrev
  Radiance/bin/pcond
  Radiance/bin/libSM.6.dylib
  Radiance/bin/evalglare
  Radiance/bin/normpat
  Radiance/bin/ra_t16
  Radiance/bin/meta2bmp
  Radiance/bin/rpict
  Radiance/bin/libGLU.1.dylib
  Radiance/bin/gcomp
  Radiance/bin/rsensor
  Radiance/bin/compamb
  Radiance/bin/getinfo
  Radiance/bin/macbethcal
  Radiance/bin/xshowtrace
  Radiance/bin/ran2tiff
  Radiance/bin/ra_pfm
  Radiance/bin/ranimove
  Radiance/bin/x11meta
  Radiance/bin/rad
  Radiance/bin/rttree_reduce
  Radiance/bin/x11.hdi
  Radiance/bin/nff2rad
  Radiance/bin/genrhgrid
  Radiance/bin/genblinds
  Radiance/bin/genclock
  Radiance/bin/mksource
  Radiance/bin/dayfact
  Radiance/bin/mgfilt
  Radiance/bin/xyzimage
  Radiance/bin/ra_pict
  Radiance/bin/glare
  Radiance/bin/ltview.pl
  Radiance/bin/libz.1.dylib
  Radiance/bin/plugins/platforms/libqcocoa.dylib
  Radiance/bin/plugins/imageformats/libqgif.dylib
  Radiance/bin/plugins/imageformats/libqico.dylib
  Radiance/bin/plugins/imageformats/libqjpeg.dylib
  Radiance/bin/plugins/imageformats/libqtiff.dylib
  Radiance/bin/QtPrintSupport.framework/Versions/5/QtPrintSupport
  Radiance/bin/QtGui.framework/Versions/5/QtGui
  Radiance/bin/QtCore.framework/Versions/5/QtCore
  Radiance/bin/QtWidgets.framework/Versions/5/QtWidgets
)

foreach(path ${FILES_TO_SIGN})
  list(APPEND _FULL_PATHS "${CMAKE_INSTALL_PREFIX}/${path}")
endforeach()

file(GLOB_RECURSE _PYTHON_SOS "${CMAKE_INSTALL_PREFIX}/EnergyPlus/python_lib/**/*.so")
file(GLOB _numpy_dylibs "${CMAKE_INSTALL_PREFIX}/EnergyPlus/python_lib/numpy/.dylibs/*.dylib")
list (APPEND _PYTHON_SOS ${_numpy_dylibs})

print_relative_paths(PREFIX "FULL_PATHS=" ABSOLUTE_PATHS ${_FULL_PATHS})
print_relative_paths(PREFIX "PYTHON_SOS=" ABSOLUTE_PATHS ${_PYTHON_SOS})

include(${CMAKE_CURRENT_LIST_DIR}/CodeSigning.cmake)
codesign_files_macos(
  FILES ${_FULL_PATHS} ${_PYTHON_SOS}
  SIGNING_IDENTITY ${CPACK_CODESIGNING_DEVELOPPER_ID_APPLICATION}
  PREFIX "${CPACK_CODESIGNING_MACOS_IDENTIFIER}."
  FORCE VERBOSE
)

# Clean up to avoid multiple passes (several components) appending to a pre-existing list
unset(FILES_TO_SIGN)
unset(_FULL_PATHS)
unset(_PYTHON_SOS)

message("Finished Codesigning inner executables and library for CPack Component OpenStudioApp")
