cmake_minimum_required(VERSION 3.10.2)
cmake_policy(SET CMP0048 NEW)

set(CMAKE_CXX_STANDARD 17)

# Use ccache is available, has to be before "project()"
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  # Support Unix Makefiles and Ninja
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(OpenStudioApplication VERSION 2.8.0)

# The PROJECT_SOURCE_DIR here is OpenStudioApplication/, which we will use instead of using CMAKE_SOURCE_DIR
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
# We will use BINARY_DIR instead of CMAKE_BINARY_DIR
set(PROJECT_BINARY_DIR "${CMAKE_BINARY_DIR}")


#MESSAGE(FATAL_ERROR "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}, BINARY_DIR=${BINARY_DIR}, PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}, CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}, CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

if ( APPLE )
  find_library(SECURITY_FRAMEWORK Security)
endif()

include(ExternalProject)
include(CPackComponent)
include(CheckCXXCompilerFlag)

if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()
if(POLICY CMP0022)
  cmake_policy(SET CMP0022 NEW)
endif()
if(POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()
if(POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()
if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

###############################################################################
#                                  N I N J A                                  #
###############################################################################
# Ninja support: has to be atop for it to take effect before anything else is done
# Add Color Output if Using Ninja
macro(AddCXXFlagIfSupported flag test)
  CHECK_CXX_COMPILER_FLAG(${flag} ${test})
  if(${${test}})
    message("adding ${flag}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
  endif()
endmacro()

if("Ninja" STREQUAL ${CMAKE_GENERATOR})
  # Clang
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    AddCXXFlagIfSupported(-fcolor-diagnostics COMPILER_SUPPORTS_fcolor-diagnostics)
  endif()

  # g++
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # For some reason it doesn't say its supported, but it works...
    # AddCXXFlagIfSupported(-fdiagnostics-color COMPILER_SUPPORTS_fdiagnostics-color)
    message(STATUS "Ninja: Forcing -fdiagnostics-color=always")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
  endif()
endif()

# Xcode/Ninja generators undefined MAKE
if(CMAKE_GENERATOR MATCHES "Make")
  set(MAKE "$(MAKE)")
else()
  set(MAKE make)
endif()


###############################################################################
#                       P R O J E C T    V E R S I O N                        #
###############################################################################
# Project version information

set(PROJECT_VERSION_BUILD "Unknown" CACHE STRING "Build number") # git sha
find_package(Git)

if(NOT GIT_FOUND)
  find_program(GIT_EXECUTABLE git HINTS "$ENV{LOCALAPPDATA}/Programs/git/bin" "C:/Program Files/Git/bin")
  if(NOT GIT_EXECUTABLE_NOTFOUND)
    set(GIT_FOUND TRUE)
  endif()
endif()

if(GIT_FOUND)
  execute_process(COMMAND "${GIT_EXECUTABLE}" "rev-parse" "--short=10" "HEAD"
                  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                  TIMEOUT 10
                  RESULT_VARIABLE RESULT
                  OUTPUT_VARIABLE GIT_VERSION
                  ERROR_QUIET
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(${RESULT} EQUAL 0 AND NOT "${GIT_VERSION}" EQUAL "${PROJECT_VERSION_BUILD}")
    set(PROJECT_VERSION_BUILD ${GIT_VERSION} CACHE STRING "Build number" FORCE) # git sha
  endif()

  get_filename_component(GIT_DIR "${GIT_EXECUTABLE}" DIRECTORY)
else()
  set(GIT_DIR "")
endif()

#find_program(PATCH_EXE patch HINTS "${GIT_DIR}" "${GIT_DIR}/../bin/" "${GIT_DIR}/../usr/bin/")
#string(COMPARE EQUAL "${PATCH_EXE}" "PATCH_EXE-NOTFOUND" PATCH_EXE_NOTFOUND)
#if(PATCH_EXE_NOTFOUND)
  #message(SEND_ERROR "Required program patch not found")
#endif()

###############################################################################
#                         B U I L D    O P T I O N S                          #
###############################################################################

if(UNIX AND NOT APPLE)
  # the RPATH to be used when installing
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

option(BUILD_PACKAGE "Build package" OFF)

###############################################################################
#                         C M A K E    C O N T R O L                          #
###############################################################################

# High level project configuration
# Do we actually want everything to go to CMAKE_BINARY_DIR/Products,
# so that when you build OpenStudioApplication you get both OpenStudio (core) and Application in the same place?
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Products")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Products")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Products")

set(LIBRARY_SEARCH_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug")

# TODO: move App macros to a local ProjectMacros.cmake
# Include project specific macros
include("${PROJECT_SOURCE_DIR}/ProjectMacros.cmake")


# Search first in the binary dir, where conan will install finders, then
# search for modules in the root dir to override cmake ones
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR} ${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/CMake")

# Use CTest
set(ALL_TESTING_TARGETS "") # global list
if(BUILD_TESTING)
  enable_testing()
  include(CTest)
endif()


###############################################################################
#          C O M P I L E R    A N D    S Y S T E M    O P T I O N S           #
###############################################################################
# Compiler and system specific options

if(UNIX)

  #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC -fno-strict-aliasing")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fno-strict-aliasing")

  # all warnings
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -fno-strict-aliasing -Winvalid-pch -Wnon-virtual-dtor")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fPIC -fno-strict-aliasing -Winvalid-pch")
  # Treat all warnings as errors
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual -ftemplate-depth=1024")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual")
    # DLM: had to add this due to issues with boost optional and gcc, may be resolved if we move to std::optional
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized")
  endif()

  # Note: CMAKE_CXX_STANDARD set to 17 should already take care of adding -std=c++17 or equivalent
  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  endif()
endif()
if(MINGW)
  # all warnings
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  # effective c++
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
  # treat warnings as errors
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

  # wchar not supported for MinGW
  #add_definitions(-DBOOST_LOG_USE_CHAR)
  #add_definitions(-DBOOST_LOG_NO_COMPILER_TLS)
endif()
if(MSVC)
  # warning level 3
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")

  # warning level 4 - DLM: we should shoot for this
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

  # all warnings - DLM: probably too high to ever use
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")

  # create compiler processes on all effective processors
  if(BUILD_WITH_MULTIPLE_PROCESSES)
    add_definitions(/MP)
  endif()

  #Enable warning on thread un-safe static member initialization - DLM: we have been bit by this many times
  add_definitions(/w44640)

  # ignore decorated name length exceeded
  add_definitions(/wd4503)

  # ignore needs to have dll-interface to be used by clients of class
  # we will not rely on exporting templates instead all libraries must
  # be linked against the same runtime
  add_definitions(/wd4251)

  add_definitions(/bigobj)

  if(CMAKE_CL_64)
    # ignore conversion from size_t to int for now
    add_definitions(/wd4267)
  endif()

  # treat warnings as errors
  # DLM: only do this for our code, added in later
  # add_definitions(/WX)

  # ignore warnings about the stl being insecure
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

  # ignore c++17 deprecation warnings generated by boost
  add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
  add_definitions(-D_SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING)

endif()

# Check version of gcc
if(CMAKE_COMPILER_IS_GNUCC)
  execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                  OUTPUT_VARIABLE GCC_VERSION)
endif()

# If Linux, check LSB_RELEASE globally so we can use it after
if(UNIX AND NOT APPLE)

  find_program(LSB_RELEASE lsb_release)
  # -rs outputs only 16.04, or 18.04
  execute_process(COMMAND ${LSB_RELEASE} -rs
    OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  message(STATUS "LSB_RELEASE_ID_SHORT = ${LSB_RELEASE_ID_SHORT}")
endif()

###############################################################################
#                    G I T    S U B M O D U L E    I N I T                    #
###############################################################################

if( NOT EXISTS "${PROJECT_SOURCE_DIR}/openstudio/.git" )

  # we couldn't find the header files for FOO or they don't exist
  message("The git submodule openstudio (core) doesn't seem to be intialized")

  # we have a submodule setup for openstudio
  # now we need to clone this submodule
  execute_process(COMMAND git submodule update --init -- openstudio
                  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

endif()

###############################################################################
#                           D E P E N D E N C I E S                           #
###############################################################################

# Required dependencies

# Threading library
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads)
if(UNIX)
  set(CMAKE_THREAD_LIBS "${CMAKE_THREAD_LIBS_INIT}" CACHE STRING "Thread library used.")
  mark_as_advanced(CMAKE_THREAD_LIBS)
endif()

# Qt via conan
list(APPEND CONAN_OPTIONS "qt:commercial=False") # opensource
list(APPEND CONAN_OPTIONS "qt:shared=True") # Shared, not static
list(APPEND CONAN_OPTIONS "qt:GUI=True") # Support for GUI applications
list(APPEND CONAN_OPTIONS "qt:widgets=True") # Support for GUI applications (Widgets are the base classes we use)

list(APPEND CONAN_OPTIONS "qt:openssl=True") # For QNetwork
list(APPEND CONAN_OPTIONS "qt:with_pcre2=True")
list(APPEND CONAN_OPTIONS "qt:with_harfbuzz=True")

# Database stuff, we don't use QSql at all actuallly
list(APPEND CONAN_OPTIONS "qt:with_sqlite3=False") # sqlite
list(APPEND CONAN_OPTIONS "qt:with_mysql=False")  # MySQL
list(APPEND CONAN_OPTIONS "qt:with_pq=False")     # PostgreSQL
list(APPEND CONAN_OPTIONS "qt:with_odbc=False")   # Oracle Database

# I think that's why produces Qt Positioning
list(APPEND CONAN_OPTIONS "qt:qtlocation=True")

# Stuff we definitely want to enable because it's disabled, and we are actively using find_package to find them below
list(APPEND CONAN_OPTIONS "qt:qtwebengine=True")
list(APPEND CONAN_OPTIONS "qt:qtwebview=True")
list(APPEND CONAN_OPTIONS "qt:qtwebchannel=True")

list(APPEND CONAN_OPTIONS "qt:qttools=False")

list(APPEND CONAN_OPTIONS "qt:qtmultimedia=False")
# Already turned off by qtmultimedia=False
list(APPEND CONAN_OPTIONS "qt:with_sdl2=False")
list(APPEND CONAN_OPTIONS "qt:with_openal=False")

list(APPEND CONAN_OPTIONS "qt:qtdoc=False")
list(APPEND CONAN_OPTIONS "qt:qtsensors=False")
list(APPEND CONAN_OPTIONS "qt:qtconnectivity=False")
list(APPEND CONAN_OPTIONS "qt:qtwayland=False")
list(APPEND CONAN_OPTIONS "qt:qtpurchasing=False")

# TODO: Temporary option
option(WITH_ICU BOOL ON)

# Convenient place to bump version
set(QT_VERSION "5.12.3")

if (WITH_ICU)
  # This recipe is the same as bincrafters', except I enabled ICU with ICU 64.2
  set(CONAN_QT "qt/${QT_VERSION}@jmarrec/testing")
  # Recipe has 2.3.0 which appears on the bincrafter repo but cannot be found...
  #set(CONAN_HARFBUZZ "harfbuzz/2.4.0@bincrafters/stable")
  list(APPEND CONAN_OPTIONS "qt:with_icu=True")
else()
  set(CONAN_QT "qt/${QT_VERSION}@bincrafters/testing")
endif()


###############################################################################
#                         I N C L U D E    P A T H S                          #
###############################################################################
# Add to include path

include_directories("${PROJECT_SOURCE_DIR}/")
include_directories("${PROJECT_SOURCE_DIR}/src/")
# Include openstudio (core) source
# TODO: JM 2019-03-29
# I'm aggressively including directories due to build errors. Need to revisit this later, changing include statements in openstudio (core) as needed too.
include_directories("${PROJECT_SOURCE_DIR}/openstudio")
include_directories("${PROJECT_SOURCE_DIR}/openstudio/src/")

# Project binary directory
include_directories("${PROJECT_BINARY_DIR}/")
include_directories("${PROJECT_BINARY_DIR}/src/")
include_directories("${PROJECT_BINARY_DIR}/openstudio/")
include_directories("${PROJECT_BINARY_DIR}/openstudio/src/")

###############################################################################
#                         S U B D I R E C T O R I E S                         #
###############################################################################
# Add project sub directories

if(MSVC)
  # treat warnings as errors
  add_definitions(/WX)
elseif()
  add_definitions(-Werror)
endif()

# Let openstudio's conan file do its thing
set(CONAN_INSTALL_FILE "${PROJECT_SOURCE_DIR}/openstudio/ConanInstall.cmake")
include("${CONAN_INSTALL_FILE}")

# Include core
add_subdirectory(openstudio)

message("RUBY_INCLUDE_DIRS = ${RUBY_INCLUDE_DIRS}")

###############################################################################
#                                     Q T                                     #
###############################################################################

# Now that openstudio (core) has run conan, we need to pick up the Qt dependencies

# Qt

# TODO: replace with more appropriate find_package in CONFIG Mode
# eg: find_package(Qt5Core 5.12.3 CONFIG REQUIRED)

set(QT_INSTALL_DIR "${CONAN_QT_ROOT}")



find_package(Qt5Core ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
find_package(Qt5Widgets ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
find_package(Qt5Sql ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
find_package(Qt5Network ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
find_package(Qt5Xml ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
find_package(Qt5Concurrent ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
find_package(Qt5PrintSupport ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
find_package(Qt5Gui ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
find_package(Qt5Quick ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
find_package(Qt5QuickWidgets ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
find_package(Qt5Qml ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
find_package(Qt5WebChannel ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
find_package(Qt5Positioning ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)

find_package(Qt5WebEngine ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
find_package(Qt5WebEngineWidgets ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
#find_package(Qt53DCore ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
#find_package(Qt53DInput ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
#find_package(Qt53DRender ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)

if(APPLE)
  set(QtWebEngineProcess "${QT_INSTALL_DIR}/lib/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app" CACHE PATH "Path to the QtWebEngineProcess")
else()
  find_program(QtWebEngineProcess NAMES QtWebEngineProcess PATHS "${QT_INSTALL_DIR}/bin/" "${QT_INSTALL_DIR}/libexec/" "${QT_INSTALL_DIR}/lib/QtWebEngineCore.framework/Helpers" NO_DEFAULT_PATH)
endif()
find_file(icudtl NAMES icudtl.dat PATHS "${QT_INSTALL_DIR}/resources/" "${QT_INSTALL_DIR}/lib/QtWebEngineCore.framework/Resources" NO_DEFAULT_PATH)
find_file(qweb_resources NAMES qtwebengine_resources.pak PATHS "${QT_INSTALL_DIR}/resources/" "${QT_INSTALL_DIR}/lib/QtWebEngineCore.framework/Resources" NO_DEFAULT_PATH)
find_file(qweb_resources_100 NAMES qtwebengine_resources_100p.pak PATHS "${QT_INSTALL_DIR}/resources/" "${QT_INSTALL_DIR}/lib/QtWebEngineCore.framework/Resources" NO_DEFAULT_PATH)
find_file(qweb_resources_200 NAMES qtwebengine_resources_200p.pak PATHS "${QT_INSTALL_DIR}/resources/" "${QT_INSTALL_DIR}/lib/QtWebEngineCore.framework/Resources" NO_DEFAULT_PATH)

# DLM: why do we have QT_WEB_LIBS separate from QT_LIBS?  can we combine these?
# DLM: now the distincton should be between Qt libs linked by openstudio_modeleditor.so and the OS App
list(APPEND QT_WEB_LIBS Qt5::Core)
list(APPEND QT_WEB_LIBS Qt5::Gui)
list(APPEND QT_WEB_LIBS Qt5::WebEngine)
list(APPEND QT_WEB_LIBS Qt5::WebEngineCore)
list(APPEND QT_WEB_LIBS Qt5::WebEngineWidgets)
#list(APPEND QT_WEB_LIBS Qt5::3DCore)
#list(APPEND QT_WEB_LIBS Qt5::3DInput)
#list(APPEND QT_WEB_LIBS Qt5::3DRender)
set_target_properties(${QT_WEB_LIBS} PROPERTIES INTERFACE_LINK_LIBRARIES "")

if(NOT APPLE)

  find_package(Qt5WebChannel ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  list(APPEND QT_WEB_LIBS Qt5::WebChannel)

  find_package(Qt5Quick ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  list(APPEND QT_WEB_LIBS Qt5::Quick)

  find_package(Qt5QuickWidgets ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  list(APPEND QT_WEB_LIBS Qt5::QuickWidgets)

  find_package(Qt5Qml ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  list(APPEND QT_WEB_LIBS Qt5::Qml)

  find_package(Qt5Positioning ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  list(APPEND QT_WEB_LIBS Qt5::Positioning)

  if(UNIX)
    find_package(Qt5DBus ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
    list(APPEND QT_WEB_LIBS Qt5::DBus)

    # there does not appear to be a normal qt finder for this
    # DLM: actually there is, it is a plugin associated with Qt5::Gui
    #message( Qt5Gui_PLUGINS = ${Qt5Gui_PLUGINS})
    #get_target_property(Qt5QXcbIntegrationPlugin_LOCATION Qt5::QXcbIntegrationPlugin LOCATION)
    #get_target_property(Qt5QXcbIntegrationPlugin_TYPE Qt5::QXcbIntegrationPlugin TYPE)
    #message("Qt5::QXcbIntegrationPlugin Location = ${Qt5QXcbIntegrationPlugin_LOCATION}")
    #message("Qt5::QXcbIntegrationPlugin Type = ${Qt5QXcbIntegrationPlugin_TYPE}")
    list(APPEND QT_WEB_LIBS Qt5::QXcbIntegrationPlugin)
    list(APPEND QT_WEB_LIBS Qt5::QXcbGlxIntegrationPlugin)
  endif()
endif()

list(APPEND QT_LIBS Qt5::Core)
list(APPEND QT_LIBS Qt5::Widgets)
list(APPEND QT_LIBS Qt5::Network)
list(APPEND QT_LIBS Qt5::Xml)
list(APPEND QT_LIBS Qt5::Concurrent)
list(APPEND QT_LIBS Qt5::PrintSupport)
list(APPEND QT_LIBS Qt5::Gui)

if(WIN32)
  find_package(Qt5WinExtras ${QT_VERSION} REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  list(APPEND QT_LIBS Qt5::WinExtras)

  find_library(QT_MAIN_LIB NAMES qtmain PATHS "${QT_INSTALL_DIR}/lib" NO_DEFAULT_PATH)
  find_library(QT_MAIN_LIB_D NAMES qtmaind PATHS "${QT_INSTALL_DIR}/lib" NO_DEFAULT_PATH)
endif()

list(APPEND QT_INCLUDES ${Qt5Core_INCLUDE_DIRS})
list(APPEND QT_INCLUDES ${Qt5Concurrent_INCLUDE_DIRS})
list(APPEND QT_INCLUDES ${Qt5Widgets_INCLUDE_DIRS})
list(APPEND QT_INCLUDES ${Qt5Xml_INCLUDE_DIRS})
list(APPEND QT_INCLUDES ${Qt5Network_INCLUDE_DIRS})
list(APPEND QT_INCLUDES ${Qt5Gui_INCLUDE_DIRS})
list(APPEND QT_INCLUDES "${QT_INSTALL_DIR}/include/QtGui/${QT_VERSION}/QtGui") # needed by qtwinmigrate
list(APPEND QT_INCLUDES ${Qt5PrintSupport_INCLUDE_DIRS})

# DLM: added this, but seems to conflict with idea of a separate set of Qt Web dependencies?
list(APPEND QT_INCLUDES ${Qt5Network_INCLUDE_DIRS})
list(APPEND QT_INCLUDES ${Qt5WebEngine_INCLUDE_DIRS})
list(APPEND QT_INCLUDES ${Qt5WebEngineCore_INCLUDE_DIRS})
list(APPEND QT_INCLUDES ${Qt5WebEngineWidgets_INCLUDE_DIRS})

if(UNIX)
  list(APPEND QT_INCLUDES ${Qt5XcbQpa_INCLUDE_DIRS})
endif()

set(CMAKE_AUTOMOC OFF)



###############################################################################
#                                T A R G E T S                                #
###############################################################################

set(project_directories "")
if(WIN32)
  include_directories("${PROJECT_SOURCE_DIR}/src/qtwinmigrate")
  list(APPEND project_directories "qtwinmigrate")
endif()
list(APPEND project_directories "utilities")
list(APPEND project_directories "model_editor")
list(APPEND project_directories "bimserver")
list(APPEND project_directories "openstudio_lib")
list(APPEND project_directories "openstudio_app")

foreach(D ${project_directories})
  add_subdirectory(src/${D})
endforeach()


###############################################################################
#                        E X P O R T    T A R G E T S                         #
###############################################################################
# Export targets and generate OpenStudioCoreConfig.cmake

set(all_lib_targets "")
if(WIN32)
  list(APPEND all_lib_targets "qtwinmigrate")
endif()
list(APPEND all_lib_targets "openstudio_modeleditor")
list(APPEND all_lib_targets "openstudio_bimserver")
list(APPEND all_lib_targets "openstudio_lib")

set(all_exe_targets "")
list(APPEND all_exe_targets "OpenStudioApp")

if(UNIX AND NOT APPLE)
  foreach(targ ${all_lib_targets} ${all_exe_targets} ${ALL_TESTING_TARGETS})
    set_target_properties(${targ} PROPERTIES
      #ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_ARCHIVE_OUTPUT_DIRECTORY}"
      #LIBRARY_OUTPUT_DIRECTORY "${PROJECT_LIBRARY_OUTPUT_DIRECTORY}"
      #RUNTIME_OUTPUT_DIRECTORY "${PROJECT_RUNTIME_OUTPUT_DIRECTORY}"
      VERSION ${OPENSTUDIO_VERSION}
    )
    target_include_directories(${targ} PUBLIC openstudio)
  endforeach()
endif()


###############################################################################
#                              I N S T A L L E R                              #
###############################################################################

# CPack: TODO

# TODO: There's a lot of packaging stuff that needs to be done still, and most of it is still in openstudio(core) while it relates to OpenStudioApp and needs to be moved.
# This is the case for the CPACK_DEBIAN_PACKAGE_DEPENDS for eg: the libwxgtk3 stuff is definitely OS App related
# We built against shared Qt, so we need to list the required deps here too, like qt5-default, libqt5webkit5(-dev?), etc
# libwxgtk3: Trusty (14.04) uses 3.0-0, all other including Xenial (16.04) use 3.0-0v5
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libwxgtk3.0-0v5 (>= 3.0.0) | libwxgtk3.0-0 (>=3.0.0)")

set(CPACK_INSTALL_PREFIX /usr/local/openstudio-${OPENSTUDIO_VERSION}/)

# Same for OpenStudioApp
execute_process(COMMAND ln -sf ${CPACK_INSTALL_PREFIX}bin/OpenStudioApp ${CMAKE_CURRENT_BINARY_DIR}/OpenStudioApp)
execute_process(COMMAND ln -sf ${CPACK_INSTALL_PREFIX}bin/OpenStudioApp ${CMAKE_CURRENT_BINARY_DIR}/OpenStudioApp-${OPENSTUDIO_VERSION})

# Have this link be installed with the .deb package in /usr/local/bin
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OpenStudioApp DESTINATION /usr/local/bin COMPONENT CLI)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OpenStudioApp-${OPENSTUDIO_VERSION} DESTINATION /usr/local/bin COMPONENT CLI)


# Create a proper App with file associations

# Install the .desktop manifest (allows the App to be seen in the Dash and adding to the dock, and to map it a mimetype)
install(FILES "${PROJECT_SOURCE_DIR}/openstudioapp.desktop" DESTINATION /usr/share/applications)
# Install the XML mime info
install(FILES "${PROJECT_SOURCE_DIR}/x-openstudio.xml" DESTINATION /usr/share/mime/application)

cpack_add_component(Resources
  DISPLAY_NAME "Application Resources"
  DESCRIPTION "Resource Files used by the OpenStudio Application"
)

cpack_add_component(OpenStudioApp
  DISPLAY_NAME "OpenStudio Application"
  DESCRIPTION "OpenStudio Application"
)

cpack_ifw_configure_component(Resources
  REQUIRES_ADMIN_RIGHTS
)

cpack_ifw_configure_component(OpenStudioApp
  DEPENDS Resources CLI
  SCRIPT src/openstudio_app/install_operations.qs
  REQUIRES_ADMIN_RIGHTS
)
