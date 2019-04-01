cmake_minimum_required(VERSION 3.10.2)
cmake_policy(SET CMP0048 NEW)

set(CMAKE_CXX_STANDARD 17)

# Use ccache is available, has to be before "project()"
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  # Support Unix Makefiles and Ninja
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(OpenStudioApplication VERSION 2.7.2)

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

# Build the OpenStudio Application
# This option will require shared libs
option(BUILD_OS_APP "Build OS App" ON)

option(BUILD_PAT "Build PAT" OFF)

# DView
option(BUILD_DVIEW "Build DView" OFF)

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
#                           D E P E N D E N C I E S                           #
###############################################################################

# Required dependencies

# Threading library
find_package(Threads)
if(UNIX)
  set(CMAKE_THREAD_LIBS "${CMAKE_THREAD_LIBS_INIT}" CACHE STRING "Thread library used.")
  mark_as_advanced(CMAKE_THREAD_LIBS)
endif()


# TODO: I think **some** these are Qt related
if(UNIX)
  if(APPLE)
    find_library(GDBM libgdbm.a PATHS /usr/local/opt/gdbm/lib)
    find_library(GMP libgmp.a PATHS /usr/local/opt/gmp/lib)
    find_library(EDIT libedit.dylib)
    find_library(UTIL libutil.dylib)
    find_library(CURSES libncurses.dylib)
    find_library(Z libz.dylib)
    find_library(FFI libffi.dylib)
    set(ADDITIONAL_RUBY_LIBS ${GDBM} ${GMP} ${EDIT} ${UTIL} ${CURSES} ${Z} ${FFI})
  else()
    find_library(GDBM libgdbm.a)
    # libqdbm-dev
    find_library(QDBM libqdbm.a)
    find_library(GMP libgmp.a)
    find_library(FFI libffi.a)
    find_library(READLINE libreadline.a)
    find_library(TERMCAP libtermcap.a)
    find_library(YAML libyaml.a)
    #    find_library(CRYPTO libcrypto.a)
    set(ADDITIONAL_RUBY_LIBS ${GDBM} ${QDBM} ${GMP} ${FFI} ${READLINE} ${TERMCAP} ${YAML} CONAN_PKG::OpenSSL)
  endif()
else()
  set(FFI_ZIP_FILENAME "FFI_3_2_1-msvc2017_v2.zip")
  set(FFI_ZIP_EXPECTED_MD5 "9d43417932447eb5830a389273954a75")
  set(FFI_ZIP_LOCAL_PATH "${PROJECT_BINARY_DIR}/${FFI_ZIP_FILENAME}")
  if(EXISTS "${FFI_ZIP_LOCAL_PATH}")
    file(MD5 "${FFI_ZIP_LOCAL_PATH}" FFI_ZIP_MD5)
  endif()

  if(NOT FFI_ZIP_MD5 STREQUAL FFI_ZIP_EXPECTED_MD5)
    unset(FFI CACHE)
    unset(FFID CACHE)
    message(STATUS "Downloading FFI: ${FFI_ZIP_FILENAME}")
    file(DOWNLOAD "http://openstudio-resources.s3.amazonaws.com/dependencies/${FFI_ZIP_FILENAME}"
      ${FFI_ZIP_LOCAL_PATH}
      INACTIVITY_TIMEOUT 120
      SHOW_PROGRESS
      EXPECTED_MD5 ${FFI_ZIP_EXPECTED_MD5})

    file(REMOVE_RECURSE "${PROJECT_BINARY_DIR}/FFI-install")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz ${FFI_ZIP_LOCAL_PATH} WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
  endif()

  find_library(FFI libffi.lib PATHS "${PROJECT_BINARY_DIR}/FFI-install/Release/lib")
  find_library(FFID libffi.lib PATHS "${PROJECT_BINARY_DIR}/FFI-install/Debug/lib")
  set(ADDITIONAL_RUBY_LIBS_RELEASE ${FFI} Iphlpapi)
  set(ADDITIONAL_RUBY_LIBS_DEBUG ${FFID} Iphlpapi)
endif()


if(BUILD_OS_APP)
  # Qt
  set(QT_INSTALL_DIR "" CACHE PATH "Path to Qt Install")
  mark_as_advanced(QT_INSTALL_DIR)

  if(NOT EXISTS ${QT_INSTALL_DIR})
    # download qt
    if(WIN32)
      if(CMAKE_CL_64)
        set(QT_ZIP_FILENAME "qt_5_11_msvc2017_64_shared.zip")
        set(QT_ZIP_EXPECTED_MD5 "82f44a8decf101ca7cb7efafd899e8ee")
      else()
        set(QT_ZIP_FILENAME "DOES_NOT_EXIST_qt_5_11_msvc2017_32_shared.zip")
        set(QT_ZIP_EXPECTED_MD5 "25abd383e7db2dfd971b4a4ef145002b")
      endif()
    elseif(APPLE)
        set(QT_ZIP_FILENAME "DOES_NOT_EXIST_qt_5_11_osx_shared.tar.gz")
        set(QT_ZIP_EXPECTED_MD5 "326887393d55b5da97a1be9764fb0d1b")
    elseif(EXISTS "/etc/redhat-release")
      set(QT_ZIP_FILENAME "DOES_NOT_EXIST_qt_5_11_redhat_shared.tar.gz")
      set(QT_ZIP_EXPECTED_MD5 "b0610716854ed91a003347c455b22eb8")
    else()
      if(LSB_RELEASE_ID_SHORT MATCHES "18.04")
        set(QT_ZIP_FILENAME "qt_5_11_linux_shared.tar.gz")
        set(QT_ZIP_EXPECTED_MD5 "33c5a562935612625ec5e41fe4b75e17")
      else()
        # 16.04
        set(QT_ZIP_FILENAME "qt_5_11_linux_shared_xenial.tar.gz")
        set(QT_ZIP_EXPECTED_MD5 "cb02b8cb34de94038a1a7cba59c0c43a")
      endif()
    endif()

    set(QT_ZIP_LOCAL_PATH "${PROJECT_BINARY_DIR}/${QT_ZIP_FILENAME}")
    if(EXISTS "${QT_ZIP_LOCAL_PATH}")
      file(MD5 "${QT_ZIP_LOCAL_PATH}" QT_ZIP_MD5)
    endif()

    if(NOT QT_ZIP_MD5 STREQUAL QT_ZIP_EXPECTED_MD5)
      message(STATUS "Downloading Qt: ${QT_ZIP_FILENAME}")
      file(DOWNLOAD "http://openstudio-resources.s3.amazonaws.com/dependencies/${QT_ZIP_FILENAME}"
        ${QT_ZIP_LOCAL_PATH}
        INACTIVITY_TIMEOUT 120
        SHOW_PROGRESS
        EXPECTED_MD5 ${QT_ZIP_EXPECTED_MD5})
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz ${QT_ZIP_LOCAL_PATH} WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
    endif()
  endif()

  set(QT_INSTALL_DIR "${PROJECT_BINARY_DIR}/Qt-install/")

  find_package(Qt5Core 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  find_package(Qt5Widgets 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  find_package(Qt5Sql 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  find_package(Qt5Network 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  find_package(Qt5Xml 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  find_package(Qt5Concurrent 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  find_package(Qt5PrintSupport 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  find_package(Qt5Gui 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  find_package(Qt5Quick 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  find_package(Qt5QuickWidgets 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  find_package(Qt5Qml 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  find_package(Qt5WebChannel 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  find_package(Qt5Positioning 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)

  find_package(Qt5WebEngine 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  find_package(Qt5WebEngineWidgets 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  #find_package(Qt53DCore 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  #find_package(Qt53DInput 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
  #find_package(Qt53DRender 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)

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

    find_package(Qt5WebChannel 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
    list(APPEND QT_WEB_LIBS Qt5::WebChannel)

    find_package(Qt5Quick 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
    list(APPEND QT_WEB_LIBS Qt5::Quick)

    find_package(Qt5QuickWidgets 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
    list(APPEND QT_WEB_LIBS Qt5::QuickWidgets)

    find_package(Qt5Qml 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
    list(APPEND QT_WEB_LIBS Qt5::Qml)

    find_package(Qt5Positioning 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
    list(APPEND QT_WEB_LIBS Qt5::Positioning)

    if(UNIX)
      find_package(Qt5DBus 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
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
    find_package(Qt5WinExtras 5.11.2 REQUIRED PATHS ${QT_INSTALL_DIR} NO_DEFAULT_PATH)
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
  list(APPEND QT_INCLUDES "${QT_INSTALL_DIR}/include/QtGui/5.11.2/QtGui") # needed by qtwinmigrate
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

endif()

# Dview
if(BUILD_DVIEW)
  set(DVIEW_INSTALL_DIR "" CACHE PATH "Path to DView Install")
  mark_as_advanced(DVIEW_INSTALL_DIR)

  if(NOT EXISTS ${DVIEW_INSTALL_DIR})
    if(WIN32)
      set(DVIEW_ZIP_FILENAME "DView-windows-20180621.zip")
      set(DVIEW_ZIP_EXPECTED_MD5 "8443b09cbfcc118943127b006b14dbca")
    elseif(APPLE)
      set(DVIEW_ZIP_FILENAME "DView-osx-20180621.tar.gz")
      set(DVIEW_ZIP_EXPECTED_MD5 "41038cbf7c472522e700d850d9de1133")
    else()
      set(DVIEW_ZIP_FILENAME "DView-linux-20180621.tar.gz")
      set(DVIEW_ZIP_EXPECTED_MD5 "c1d3fb6e736668eb4bdf0a5af2b4506c")
    endif()

    set(DVIEW_ZIP_LOCAL_PATH "${PROJECT_BINARY_DIR}/${DVIEW_ZIP_FILENAME}")
    if(EXISTS "${DVIEW_ZIP_LOCAL_PATH}")
      file(MD5 "${DVIEW_ZIP_LOCAL_PATH}" RUBY_ZIP_MD5)
    endif()

    if(NOT DVIEW_ZIP_MD5 STREQUAL DVIEW_ZIP_EXPECTED_MD5)
      message(STATUS "Downloading DView: ${DVIEW_ZIP_FILENAME}")
      file(DOWNLOAD "http://openstudio-resources.s3.amazonaws.com/dependencies/${DVIEW_ZIP_FILENAME}"
        ${DVIEW_ZIP_LOCAL_PATH}
        INACTIVITY_TIMEOUT 120
        SHOW_PROGRESS
        EXPECTED_MD5 ${DVIEW_ZIP_EXPECTED_MD5})
      execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz ${DVIEW_ZIP_LOCAL_PATH} WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
    endif()

    set(DVIEW_INSTALL_DIR "${PROJECT_BINARY_DIR}/DView-install")
  endif()

  if(WIN32)
    install(PROGRAMS "${DVIEW_INSTALL_DIR}/DView.exe" DESTINATION ./bin COMPONENT "DView")
  elseif(APPLE)
    install(DIRECTORY "${DVIEW_INSTALL_DIR}/DView.app" DESTINATION . USE_SOURCE_PERMISSIONS COMPONENT "DView")
  else()
    install(PROGRAMS "${DVIEW_INSTALL_DIR}/DView" DESTINATION ./bin COMPONENT "DView")
  endif()

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

# Include core
add_subdirectory(openstudio)


###############################################################################
#                                  C O N A N                                  #
###############################################################################


## Conan

## Download automatically, you can also just copy the conan.cmake file
## Put it in CMAKE_BINARY_DIR so we don't end up with two when building OpenStudioApplication
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "OpenStudioApplication: Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.13/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake")
else()
  message(STATUS "OpenStudioApplication: using existing conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_check(VERSION 1.0.0 REQUIRED)

# openstudio is the one that installs Conan dependencies, we just reload the info so we can link against them (gtest)
if(EXISTS "${CMAKE_BINARY_DIR}/openstudio/conanbuildinfo.cmake")
  message(STATUS "OpenStudioApplication: Using ${CMAKE_BINARY_DIR}/openstudio/conanbuildinfo.cmake")
  # Silence this part, it's a repeat of what OScore has done
  set(CONAN_CMAKE_SILENT_OUTPUT TRUE)
  include(${CMAKE_BINARY_DIR}/openstudio/conanbuildinfo.cmake)
  # Make sure to pass NO_OUTPUT_DIRS to not override CMAKE_RUNTIME_OUTPUT_DIR etc
  conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
  # message("OpenStudioApplication: CONAN_TARGETS=${CONAN_TARGETS}")
else()
  message(STATUS "OpenStudioApp: Conan will need to run in openstudio")
endif()

###############################################################################
#                                T A R G E T S                                #
###############################################################################

set(project_directories "")
if(BUILD_OS_APP)
  if(WIN32)
    include_directories("${PROJECT_SOURCE_DIR}/src/qtwinmigrate")
    list(APPEND project_directories "qtwinmigrate")
  endif()
  list(APPEND project_directories "model_editor")
  list(APPEND project_directories "bimserver")
  list(APPEND project_directories "openstudio_lib")
  list(APPEND project_directories "openstudio_app")
endif()

foreach(D ${project_directories})
  add_subdirectory(src/${D})
endforeach()


###############################################################################
#                        E X P O R T    T A R G E T S                         #
###############################################################################
# Export targets and generate OpenStudioCoreConfig.cmake

set(all_lib_targets "")
if(BUILD_OS_APP)
  if(WIN32)
    list(APPEND all_lib_targets "qtwinmigrate")
  endif()
  list(APPEND all_lib_targets "openstudio_modeleditor")
  list(APPEND all_lib_targets "openstudio_bimserver")
  list(APPEND all_lib_targets "openstudio_lib")
endif()

set(all_exe_targets "")

if(BUILD_OS_APP)
  list(APPEND all_exe_targets "OpenStudioApp")
endif()

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


if(BUILD_OS_APP)

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
endif()
