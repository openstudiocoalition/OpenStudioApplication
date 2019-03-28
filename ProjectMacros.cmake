include(CMakeParseArguments)

macro(FIND_QT_STATIC_LIB NAMES P)

  unset(NAMES_D)
  unset(FIRST_NAME)
  foreach(N ${NAMES})
    if (NOT FIRST_NAME)
      set(FIRST_NAME ${N})
    endif()
    list(APPEND NAMES_D "${N}d")
  endforeach()

  if ("${P}" STREQUAL "*")
    #message("LOOKING FOR ${NAMES} in default locations")
    find_library(QT_STATIC_LIB NAMES ${NAMES})
  else()
    #message("LOOKING FOR ${NAMES} in ${P}")
    find_library(QT_STATIC_LIB NAMES ${NAMES} PATHS ${P} NO_DEFAULT_PATH)

    if(QT_STATIC_LIB)
    else()
      message(SEND_ERROR "Cannot find ${NAMES} in ${P}")
    endif()
  endif()

  if(QT_STATIC_LIB)
    list(APPEND QT_STATIC_LIBS ${QT_STATIC_LIB})
  else()
    #message("Cannot find ${NAMES}, using ${FIRST_NAME}")
    list(APPEND QT_STATIC_LIBS ${FIRST_NAME})
  endif()

  if ("${P}" STREQUAL "*")
    #message("LOOKING FOR ${NAMES_D} in default locations")
    find_library(QT_STATIC_LIB_D NAMES ${NAMES_D})
  else()
    #message("LOOKING FOR ${NAMES_D} in ${P}")
    find_library(QT_STATIC_LIB_D NAMES ${NAMES_D} PATHS ${P} NO_DEFAULT_PATH)
  endif()

  if(QT_STATIC_LIB_D)
    list(APPEND QT_STATIC_LIBS_D ${QT_STATIC_LIB_D})
  elseif(QT_STATIC_LIB)
    list(APPEND QT_STATIC_LIBS_D ${QT_STATIC_LIB})
  else()
    #message("Cannot find ${NAMES_D} or ${NAMES}, using ${FIRST_NAME}")
    list(APPEND QT_STATIC_LIBS_D ${FIRST_NAME})
  endif()

  unset(QT_STATIC_LIB CACHE )
  unset(QT_STATIC_LIB_D CACHE )
endmacro()

# This function is nearly identical to QT5_WRAP_CPP (from Qt5CoreMacros.cmake), except that it removes Boost
# from the include directories and outputs .cxx files

# qt5_wrap_cpp_minimally(outfiles inputfile ...)
function(QT5_WRAP_CPP_MINIMALLY outfiles)
  # Remove Boost and possibly other include directories
  get_directory_property(_inc_DIRS INCLUDE_DIRECTORIES)
  set(_orig_DIRS ${_inc_DIRS})
  if(APPLE)
    if(NOT ${target_name} STREQUAL "qwt")
      foreach(_current ${_inc_DIRS})
        if(NOT "${_current}" MATCHES "[Qq][Tt]5")
          list(REMOVE_ITEM _inc_DIRS "${_current}")
        endif()
      endforeach()
      set_directory_properties(PROPERTIES INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/src;${PROJECT_BINARY_DIR}/src;${_inc_DIRS}")
    endif()
  elseif(UNIX)
    foreach(_current ${_inc_DIRS})
      if(NOT "${_current}" MATCHES "[Qq][Tt]5")
        list(REMOVE_ITEM _inc_DIRS "${_current}")
      endif()
    endforeach()
    set_directory_properties(PROPERTIES INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/src;${PROJECT_BINARY_DIR}/src;${_inc_DIRS}")
  else()
    foreach(_current ${_inc_DIRS})
      if("${_current}" MATCHES "[Bb][Oo][Oo][Ss][Tt]")
        list(REMOVE_ITEM _inc_DIRS "${_current}")
      endif()
    endforeach()
    set_directory_properties(PROPERTIES INCLUDE_DIRECTORIES "${_inc_DIRS}")
  endif()

  qt5_get_moc_flags(moc_flags)

  set(options)
  set(oneValueArgs TARGET)
  set(multiValueArgs OPTIONS)

  cmake_parse_arguments(_WRAP_CPP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(moc_files ${_WRAP_CPP_UNPARSED_ARGUMENTS})
  set(moc_options ${_WRAP_CPP_OPTIONS})
  set(moc_target ${_WRAP_CPP_TARGET})

  if(moc_target AND CMAKE_VERSION VERSION_LESS 2.8.12)
    message(FATAL_ERROR "The TARGET parameter to qt5_wrap_cpp is only available when using CMake 2.8.12 or later.")
  endif()
  foreach(it ${moc_files})
    get_filename_component(it ${it} ABSOLUTE)
    qt5_make_output_file(${it} moc_ cxx outfile)
    qt5_create_moc_command(${it} ${outfile} "${moc_flags}" "${moc_options}" "${moc_target}" "")
    list(APPEND ${outfiles} ${outfile})
  endforeach()
  set(${outfiles} ${${outfiles}} PARENT_SCOPE)

  # Restore include directories
  set_directory_properties(PROPERTIES INCLUDE_DIRECTORIES "${_orig_DIRS}")
endfunction()


# link target with debug and release libs
function(LINK_DEBUG_AND_RELEASE this_target debug_libs release_libs)
  list(LENGTH debug_libs len1)
  list(LENGTH release_libs len2)

  if (NOT len1 EQUAL len2)
    message(SEND_ERROR "Unequal lists passed to LINK_DEBUG_AND_RELEASE")
  endif()

  math(EXPR len "${len1} - 1")

  foreach(i RANGE ${len})
    list(GET debug_libs ${i} debug_lib)
    list(GET release_libs ${i} release_lib)
    target_link_libraries(${this_target} debug ${debug_lib} optimized ${release_lib})
  endforeach()

endfunction()
