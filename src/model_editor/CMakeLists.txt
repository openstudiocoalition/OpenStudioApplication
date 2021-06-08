set(target_name openstudio_modeleditor)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QT_INCLUDES})

#--------------------------------------------------------------------------------
# First the library
#--------------------------------------------------------------------------------

# lib files
set(${target_name}_src
  Application.hpp
  Application.cpp
  AccessPolicyStore.hpp
  AccessPolicyStore.cpp
  BridgeClasses.hpp
  BridgeClasses.cpp
  GithubReleases.hpp
  GithubReleases.cpp
  InspectorDialog.hpp
  InspectorDialog.cpp
  InspectorGadget.hpp
  InspectorGadget.cpp
  ListWidget.hpp
  ListWidget.cpp
  ModalDialogs.hpp
  ModalDialogs.cpp
  OSProgressBar.hpp
  OSProgressBar.cpp
  PathWatcher.hpp
  PathWatcher.cpp
  QMetaTypes.hpp
  QMetaTypes.cpp
  TableView.hpp
  TableView.cpp
  TableWidget.hpp
  TableWidget.cpp
  TestButton.hpp
  TestButton.cpp
  UserSettings.hpp
  UserSettings.cpp
  Utilities.hpp
  Utilities.cpp
  IGLineEdit.hpp
  IGLineEdit.cpp
  IGSpinBoxes.hpp
  IGSpinBoxes.cpp
)

# lib moc files
set(${target_name}_moc
  InspectorDialog.hpp
  InspectorGadget.hpp
  ListWidget.hpp
  ModalDialogs.hpp
  PathWatcher.hpp
  TableView.hpp
  TableWidget.hpp
  TestButton.hpp
  IGLineEdit.hpp
  IGSpinBoxes.hpp
  BridgeClasses.hpp
)

# lib resource files
set(${target_name}_qrc
  modeleditorlib.qrc
)

# configure the AboutBox.hpp file
# this needs to be configured to embed the build number
CONFIGURE_FILE_WITH_CHECKSUM(AboutBox.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/AboutBox.hpp")

## Qt MOC generation
qt5_wrap_cpp(${target_name}_mocs ${${target_name}_moc})

# generate rules for building source files from the resources
qt5_add_resources(${target_name}_qrcs ${${target_name}_qrc})

# make the shared library
add_library(${target_name}
  ${${target_name}_src}
  ${${target_name}_moc}
  ${${target_name}_mocs}
  ${${target_name}_qrc}
  ${${target_name}_qrcs}
)

# lib dependencies
set(${target_name}_depends
  openstudio::openstudiolib
  openstudioapp_utilities
  ${QT_LIBS}
  #CONAN_PKG::openstudio_ruby
)
if(WIN32)
  list(APPEND ${target_name}_depends qtwinmigrate)
endif()

add_dependencies(${target_name} ${${target_name}_depends})
target_link_libraries(${target_name} ${${target_name}_depends})

if(BUILD_SHARED_LIBS)
  #target_compile_definitions(${target_name} PUBLIC model_editor_EXPORTS)
else()
  target_compile_definitions(${target_name} PUBLIC model_editor_EXPORTS)
endif()

set(${target_name}_test_src
  test/ModelEditorFixture.hpp
  test/ModelEditorFixture.cpp
  test/IGLineEdit_GTest.cpp
  test/InspectorDialog_GTest.cpp
  test/ModalDialogs_GTest.cpp
  test/PathWatcher_GTest.cpp
  test/QMetaTypes_GTest.cpp
  test/Utilities_GTest.cpp
  test/GithubReleases_GTest.cpp
)

set(${target_name}_test_depends
  ${${target_name}_depends}
)

CREATE_TEST_TARGETS(${target_name} "${${target_name}_test_src}" "${${target_name}_test_depends}")
CREATE_SRC_GROUPS("${${target_name}_test_src}")

# lib swig files
set(${target_name}_swig_src
  ModelEditor.i
  Qt.i
)

set(swig_target_name ${target_name})
MAKE_SWIG_TARGET_OSAPP(OpenStudioModelEditor modeleditor "${CMAKE_CURRENT_SOURCE_DIR}/ModelEditor.i" "${${target_name}_swig_src}" ${swig_target_name} "")

