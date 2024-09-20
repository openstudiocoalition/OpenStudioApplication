set(target_name openstudioapp_utilities)

include_directories(${PROJECT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

CONFIGURE_FILE_WITH_CHECKSUM("OpenStudioApplicationPathHelpers.cxx.in" "${CMAKE_CURRENT_BINARY_DIR}/OpenStudioApplicationPathHelpers.cxx")

set(${target_name}_src
  OpenStudioApplicationPathHelpers.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/OpenStudioApplicationPathHelpers.cxx
)

# set up groups of source files for Visual Studio
source_group(${target_name} FILES ${target_name}_src)

set(${target_name}_test_src
  test/OpenStudioApplicationPathHelpers_GTest.cpp
)

set(${target_name}_depends
  openstudio::openstudiolib
  boost::boost
)

add_library(${target_name}
  ${${target_name}_src}
)
if (NINJA)
  target_compile_definitions(${target_name} PRIVATE NINJA=1)
endif()
target_link_libraries(${target_name} ${${target_name}_depends} )

if(BUILD_MICRO_PROFILING)
  target_link_libraries(${target_name} ${MICRO_PROFILER_LIB} )
endif()

if(BUILD_TESTING)
  CREATE_TEST_TARGETS(${target_name} "${${target_name}_test_src}" "${${target_name}_depends}")
  # add_dependencies("${target_name}_tests" openstudio_energyplus_resources)
endif()

