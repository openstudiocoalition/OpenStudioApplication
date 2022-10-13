set(target_name OpenStudioApp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QT_INCLUDES})

# source files
set(${target_name}_SRC
  main.cpp
  OpenStudioApp.cpp
  OpenStudioApp.hpp
  StartupView.hpp
  StartupView.cpp
  StartupMenu.hpp
  StartupMenu.cpp
  LibraryDialog.hpp
  LibraryDialog.cpp
  ExternalToolsDialog.hpp
  ExternalToolsDialog.cpp

  ../shared_gui_components/BusyWidget.cpp
  ../shared_gui_components/BusyWidget.hpp
  ../shared_gui_components/OSDialog.cpp
  ../shared_gui_components/OSDialog.hpp
  ../shared_gui_components/WaitDialog.cpp
  ../shared_gui_components/WaitDialog.hpp
)

# moc files
set(${target_name}_moc
  OpenStudioApp.hpp
  StartupView.hpp
  StartupMenu.hpp
  LibraryDialog.hpp
  ExternalToolsDialog.hpp

  ../shared_gui_components/BusyWidget.hpp
  ../shared_gui_components/OSDialog.hpp
  ../shared_gui_components/WaitDialog.hpp
)

## Qt UI generation
qt6_wrap_cpp(${target_name}_moc_src ${${target_name}_moc})

# configure the AboutBox.hpp file
# this needs to be configured to embed the build number
CONFIGURE_FILE_WITH_CHECKSUM(AboutBox.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/AboutBox.hpp")

set(OPENSTUDIOAPPLICATION_ICON_FOLDER "${PROJECT_SOURCE_DIR}/icons")
if(WIN32)
  remove_definitions(/bigobj)
  CONFIGURE_FILE_WITH_CHECKSUM(OpenStudioApp.rc.in "${CMAKE_CURRENT_BINARY_DIR}/OpenStudioApp.rc")
  set(ICON_SRC "${CMAKE_CURRENT_BINARY_DIR}/OpenStudioApp.rc")
elseif(APPLE)
  # For Apple set the icns file containing icons

  # set the name of the bundle icon
  set(MACOSX_BUNDLE_ICON_FILE os.icns)
  # set all icons
  set(ICON_SRC
    ${OPENSTUDIOAPPLICATION_ICON_FOLDER}/os.icns
    ${OPENSTUDIOAPPLICATION_ICON_FOLDER}/osm.icns
    ${OPENSTUDIOAPPLICATION_ICON_FOLDER}/osc.icns
  )

  set_source_files_properties(${ICON_SRC}
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources
  )

  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${OPENSTUDIOAPPLICATION_VERSION}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "${OPENSTUDIOAPPLICATION_LONG_VERSION}")
  set(MACOSX_BUNDLE_COPYRIGHT "Copyright © 2020-2022, OpenStudio Coalition and other contributors. All rights reserved.")

  set(MACOSX_BUNDLE_EXECUTABLE_NAME ${target_name})
  # PROJECT_VERSION_BUILD = Git SHA. This makes it globally unique, and is what we want to avoid CFBUndleIdenfitied collision
  set(MACOSX_BUNDLE_GUI_IDENTIFIER "org.openstudiocoalition.${PROJECT_VERSION_BUILD}.openstudioapplication")
  set(MACOSX_BUNDLE_BUNDLE_NAME "OpenStudioApplication")
  CONFIGURE_FILE_WITH_CHECKSUM(${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)

elseif(UNIX)
  # Install the icons
  # Cf. http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html#directory_layout

  # NOTE: Instead, to avoid dpkg issuing conflicts when installing several different versions, we use a post-installation script
  # So install them to a temp directory that we'll remove later
  install(DIRECTORY ${OPENSTUDIOAPPLICATION_ICON_FOLDER} DESTINATION Temp
          FILES_MATCHING PATTERN "*.png")

  #foreach(_size 16 32 48 64 128 256)
    ## Copy OS App icons:
    #install(
      #FILES "${OPENSTUDIOAPPLICATION_ICON_FOLDER}/os_${_size}.png"
      #DESTINATION "/usr/share/icons/hicolor/${_size}x${_size}/apps"
      #RENAME openstudioapp.png
    #)
    ## Install OSM mimetypes icons
    #install(
      #FILES "${OPENSTUDIOCORE_ICON_FOLDER}/osm_${_size}.png"
      #DESTINATION "/usr/share/icons/hicolor/${_size}x${_size}/mimetypes"
      #RENAME application-x-openstudio.png
    #)
  ## Install OSC
    #install(
      #FILES "${OPENSTUDIOCORE_ICON_FOLDER}/osc_${_size}.png"
      #DESTINATION "/usr/share/icons/hicolor/${_size}x${_size}/mimetypes"
      #RENAME application-x-openstudio-component.png
    #)

  #endforeach()

  ## OS App has a 512 and 1024 too
  #foreach(_size 512 1024)
    ## Copy OS App icons
    #install(
      #FILES "${OPENSTUDIOAPPLICATION_ICON_FOLDER}/os_${_size}.png"
      #DESTINATION "/usr/share/icons/hicolor/${_size}x${_size}/apps"
      #RENAME openstudioapp.png
    #)
  #endforeach()

endif()

add_executable(${target_name}
  WIN32
  MACOSX_BUNDLE
  ${${target_name}_SRC}
  ${${target_name}_moc_src}
  ${${target_name}_moc}
  ${ICON_SRC}
)

if (NINJA)
  target_compile_definitions(${target_name} PRIVATE NINJA=1)
endif()

if(WIN32)
  # increase stack size
  target_link_options(${target_name} PRIVATE /STACK:8388608)
endif()

# This is because Apple has app bundles for .apps.
# We copy the openstudio cli into the app bundle in an effort
# to be self contained. It also makes it easy to find the cli since
# it will be sitting next to the app executable.
if( APPLE )
  add_custom_command(TARGET ${target_name}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:openstudio::openstudio> $<TARGET_FILE_DIR:${target_name}>/$<TARGET_FILE_NAME:openstudio::openstudio>
  )
endif()

add_custom_command(TARGET ${target_name}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${target_name}>/platforms/
)

if(WIN32)
  add_custom_command(TARGET ${target_name}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${target_name}>/styles/
  )
endif()

if(UNIX AND NOT APPLE)
  add_custom_command(TARGET ${target_name}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${target_name}>/xcbglintegrations/
  )
endif()

if( APPLE )
  # Note: JM: Qt 6.3.0, I can no longer access $<TARGET_FILE:Qt6::QCocoaIntegrationPlugin>
  find_library(QT_QCOCOA NAMES libqcocoa.dylib PATHS "${QT_INSTALL_DIR}/plugins/platforms" NO_DEFAULT_PATH)
  add_custom_command(TARGET ${target_name}
    POST_BUILD
    # COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QCocoaIntegrationPlugin> $<TARGET_FILE_DIR:${target_name}>/platforms/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_QCOCOA} $<TARGET_FILE_DIR:${target_name}>/platforms/
    VERBATIM
  )

  #add_custom_command(TARGET ${target_name}
  #  POST_BUILD
  #  COMMAND ${QT_MACDEPLOY_QT} $<TARGET_BUNDLE_DIR:${target_name}> -verbose=2
  #)

elseif( UNIX )

  foreach(LIB ${QT_LIBS} ${QT_WEB_LIBS})
    get_property(LIB_LOCATION TARGET ${LIB} PROPERTY LOCATION)
    list(APPEND QT_LIB_LOCATIONS "${LIB_LOCATION}")
  endforeach()

  foreach(LIB_LOCATION ${QT_LIB_LOCATIONS} ${QT_EXTRA_LIBS})
    # In case we grabed "xx.so", it'll be resolved to "xx.so.X.Y.Z" as we expect
    get_filename_component(LIB_LOCATION_RESOLVED "${LIB_LOCATION}" REALPATH)

    get_filename_component(LIB_BASE_NAME ${LIB_LOCATION_RESOLVED} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" LIB_MAJOR_MINOR_NAME ${LIB_BASE_NAME})
    string(REGEX REPLACE "\\.[^.]*$" "" LIB_MAJOR_NAME ${LIB_MAJOR_MINOR_NAME})

    list(APPEND QT_LIB_SYMLINKS "${LIB_MAJOR_MINOR_NAME}" "${LIB_MAJOR_NAME}")

    add_custom_command(TARGET ${target_name}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LIB_LOCATION_RESOLVED} $<TARGET_FILE_DIR:${target_name}>/
      COMMAND ln -sf "${LIB_BASE_NAME}" "$<TARGET_FILE_DIR:${target_name}>/${LIB_MAJOR_MINOR_NAME}"
      COMMAND ln -sf "${LIB_BASE_NAME}" "$<TARGET_FILE_DIR:${target_name}>/${LIB_MAJOR_NAME}"
      COMMAND patchelf --set-rpath "$ORIGIN/." $<TARGET_FILE_DIR:${target_name}>/${LIB_BASE_NAME}
      VERBATIM
    )
  endforeach()

  add_custom_command(TARGET ${target_name}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QtWebEngineProcess} $<TARGET_FILE_DIR:${target_name}>/
    COMMAND chrpath -r "$ORIGIN/." $<TARGET_FILE_DIR:${target_name}>/QtWebEngineProcess
    VERBATIM
  )

  add_custom_command(TARGET ${target_name}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/qt.conf $<TARGET_FILE_DIR:${target_name}>
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${target_name}>/resources/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${icudtl} $<TARGET_FILE_DIR:${target_name}>/resources/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${qweb_resources} $<TARGET_FILE_DIR:${target_name}>/resources/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${qweb_resources_devtools} $<TARGET_FILE_DIR:${target_name}>/resources/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${qweb_resources_100} $<TARGET_FILE_DIR:${target_name}>/resources/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${qweb_resources_200} $<TARGET_FILE_DIR:${target_name}>/resources/
  )

  # Note: JM: Qt 6.3.0, I can no longer access $<TARGET_FILE:Qt6::QXcbIntegrationPlugin>
  find_library(QT_QXCB NAMES libqxcb.so PATHS "${QT_INSTALL_DIR}/plugins/platforms" NO_DEFAULT_PATH)
  add_custom_command(TARGET ${target_name}
    POST_BUILD
    # Note: JM: Qt 6.3.0, I can no longer access $<TARGET_FILE:Qt6::QXcbIntegrationPlugin>
    # COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QXcbIntegrationPlugin> $<TARGET_FILE_DIR:${target_name}>/platforms/
    # COMMAND patchelf --set-rpath "$ORIGIN/.." $<TARGET_FILE_DIR:${target_name}>/platforms/$<TARGET_FILE_NAME:Qt6::QXcbIntegrationPlugin>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_QXCB} $<TARGET_FILE_DIR:${target_name}>/platforms/
    COMMAND patchelf --set-rpath "$ORIGIN/.." $<TARGET_FILE_DIR:${target_name}>/platforms/libqxcb.so
    VERBATIM
  )

  find_library(QT_QXCB_GLX NAMES libqxcb-glx-integration.so PATHS "${QT_INSTALL_DIR}/plugins/xcbglintegrations" NO_DEFAULT_PATH)
  add_custom_command(TARGET ${target_name}
    POST_BUILD
    # Note: JM: Qt 6.3.0, I can no longer access $<TARGET_FILE:Qt6::QXcbGlxIntegrationPlugin>
    # COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QXcbGlxIntegrationPlugin> $<TARGET_FILE_DIR:${target_name}>/xcbglintegrations/
    # COMMAND chrpath -r "$ORIGIN/." $<TARGET_FILE_DIR:${target_name}>/xcbglintegrations/$<TARGET_FILE_NAME:Qt6::QXcbGlxIntegrationPlugin>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_QXCB_GLX} $<TARGET_FILE_DIR:${target_name}>/xcbglintegrations/
    COMMAND patchelf --set-rpath "$ORIGIN/.." $<TARGET_FILE_DIR:${target_name}>/xcbglintegrations/libqxcb-glx-integration.so
    VERBATIM
  )

elseif( WIN32 )

  target_link_libraries(${target_name} optimized ${QT_ENTRYPOINT_LIB} debug ${QT_ENTRYPOINT_LIB_D})

  add_custom_command(TARGET ${target_name}
    POST_BUILD
    COMMAND ${QT_WINDEPLOY_QT} --dir $<TARGET_FILE_DIR:${target_name}> $<TARGET_FILE:${target_name}>  )

  add_custom_command(TARGET ${target_name}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/qt.conf $<TARGET_FILE_DIR:${target_name}>
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${target_name}>/resources/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${icudtl} $<TARGET_FILE_DIR:${target_name}>/resources/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${qweb_resources} $<TARGET_FILE_DIR:${target_name}>/resources/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${qweb_resources_devtools} $<TARGET_FILE_DIR:${target_name}>/resources/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${qweb_resources_100} $<TARGET_FILE_DIR:${target_name}>/resources/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${qweb_resources_200} $<TARGET_FILE_DIR:${target_name}>/resources/
  )

  # TODO: IMPORTED_SONAME_RELEASE now contains @rpath so it fails
  get_target_property(LIBOSLIB openstudio::openstudiolib IMPORTED_LOCATION_RELEASE)
  add_custom_command(TARGET ${target_name}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LIBOSLIB} $<TARGET_FILE_DIR:${target_name}>/ )

endif()

set(depends
  openstudio_lib
  openstudio_bimserver
  openstudio_modeleditor
  openstudioapp_utilities
  openstudio::openstudiolib
  ${QT_WEB_LIBS}
)

target_link_libraries(${target_name} ${depends})

#AddPCH(${target_name})

#------------------------------------------------------------------------------
# Installation
#------------------------------------------------------------------------------
if(APPLE)
  set_target_properties(${target_name} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
  set(plugin_dest_dir OpenStudioApp.app/Contents/MacOS)
  set(APPS "\${CMAKE_INSTALL_PREFIX}/OpenStudioApp.app")
elseif(WIN32)
  set(plugin_dest_dir bin)
  set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/OpenStudioApp.exe")
else()
  set_target_properties(${target_name} PROPERTIES INSTALL_RPATH "$ORIGIN/.")
  set(plugin_dest_dir bin)
  set(APPS "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin/OpenStudioApp")
endif()

if( APPLE )
  install(TARGETS ${target_name}
    DESTINATION .
    COMPONENT "OpenStudioApp"
  )
else()
  install(TARGETS ${target_name}
    DESTINATION bin
    COMPONENT "OpenStudioApp"
  )
endif()

# directories to look for dependencies
get_target_property(os_lib_location openstudio::openstudiolib LOCATION)
get_filename_component(os_lib_dir ${os_lib_location} DIRECTORY)
set(DIRS "${QT_INSTALL_DIR}/bin" "${QT_INSTALL_DIR}/lib" "${os_lib_dir}")

if(APPLE)
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/openstudio_app/Resources/
          DESTINATION OpenStudioApp.app/Contents/Resources
          COMPONENT "Resources"
  )
elseif(WIN32)
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/openstudio_app/Resources/
    DESTINATION Resources
    COMPONENT "Resources"
  )
else()
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/openstudio_app/Resources/
    DESTINATION Resources
    COMPONENT "Resources"
  )
endif()

# since on apple the whole app bundle is installed (including these webkit components) we can exclude apple
if( NOT APPLE )

  install(FILES $<TARGET_FILE_DIR:${target_name}>/qt.conf DESTINATION bin COMPONENT OpenStudioApp)
  install(DIRECTORY $<TARGET_FILE_DIR:${target_name}>/platforms/ DESTINATION bin/platforms/ COMPONENT OpenStudioApp)
  if (WIN32)
    install(DIRECTORY $<TARGET_FILE_DIR:${target_name}>/styles/ DESTINATION bin/styles/ COMPONENT OpenStudioApp)
  endif()
  install(DIRECTORY $<TARGET_FILE_DIR:${target_name}>/translations/ DESTINATION bin/translations/ COMPONENT OpenStudioApp)
  install(DIRECTORY $<TARGET_FILE_DIR:${target_name}>/resources/ DESTINATION bin/resources/ COMPONENT OpenStudioApp)

  if(WIN32)
    install(TARGETS OpenStudioApp DESTINATION bin COMPONENT OpenStudioApp )
    install(PROGRAMS $<TARGET_FILE:openstudio::openstudiolib> DESTINATION bin COMPONENT OpenStudioApp )

    install(PROGRAMS ${QtWebEngineProcess} DESTINATION bin COMPONENT OpenStudioApp )

    install(PROGRAMS $<TARGET_FILE_DIR:${target_name}>/D3Dcompiler_47.dll DESTINATION bin/ COMPONENT OpenStudioApp)
    install(PROGRAMS $<TARGET_FILE_DIR:${target_name}>/opengl32sw.dll DESTINATION bin/ COMPONENT OpenStudioApp)

    foreach(LIB ${QT_LIBS} ${QT_WEB_LIBS})
      install(PROGRAMS $<TARGET_FILE_DIR:${target_name}>/$<TARGET_FILE_NAME:${LIB}> DESTINATION bin/ COMPONENT OpenStudioApp)
    endforeach()

    list(APPEND QT_DIRS iconengines)
    list(APPEND QT_DIRS imageformats)
    list(APPEND QT_DIRS networkinformation)
    list(APPEND QT_DIRS position)
    list(APPEND QT_DIRS tls)
    foreach(DIR ${QT_DIRS})
      install(DIRECTORY $<TARGET_FILE_DIR:${target_name}>/${DIR} DESTINATION bin/ COMPONENT OpenStudioApp)
    endforeach()

  elseif(UNIX)
    # Let fixup_bundle do that for us
    #install(PROGRAMS $<TARGET_FILE:openstudio::openstudiolib> DESTINATION bin COMPONENT OpenStudioApp )

    install(PROGRAMS $<TARGET_FILE_DIR:${target_name}>/QtWebEngineProcess DESTINATION bin/ COMPONENT OpenStudioApp)
    install(DIRECTORY $<TARGET_FILE_DIR:${target_name}>/xcbglintegrations/ DESTINATION bin/xcbglintegrations/ COMPONENT OpenStudioApp)
    # install(PROGRAMS $<TARGET_FILE_DIR:${target_name}>/xcbglintegrations/$<TARGET_FILE_NAME:Qt6::QXcbGlxIntegrationPlugin> DESTINATION bin/xcbglintegrations/ COMPONENT OpenStudioApp)

    foreach(LIB ${QT_LIBS} ${QT_WEB_LIBS})
      install(PROGRAMS $<TARGET_FILE_DIR:${target_name}>/$<TARGET_FILE_NAME:${LIB}> DESTINATION bin/ COMPONENT OpenStudioApp)
    endforeach()

    foreach(LIB_LOCATION ${QT_EXTRA_LIBS})
      get_filename_component(LIB_LOCATION_RESOLVED "${LIB_LOCATION}" REALPATH)
      get_filename_component(LIB_BASE_NAME ${LIB_LOCATION_RESOLVED} NAME)
      install(PROGRAMS $<TARGET_FILE_DIR:${target_name}>/${LIB_BASE_NAME} DESTINATION bin/ COMPONENT OpenStudioApp)
    endforeach()


    foreach(LINK ${QT_LIB_SYMLINKS})
      install(FILES $<TARGET_FILE_DIR:${target_name}>/${LINK} DESTINATION bin/ COMPONENT OpenStudioApp)
    endforeach()

  endif()
endif()

if( APPLE )
  list(APPEND QTPLUGINS "\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/platforms/libqcocoa.dylib")
  #list(APPEND QTPLUGINS "\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/sceneparsers/libgltfsceneparser.dylib")
  #list(APPEND QTPLUGINS "\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/sceneparsers/libassimpsceneparser.dylib")
  install(DIRECTORY ${QtWebEngineProcess} DESTINATION "OpenStudioApp.app/Contents/Frameworks/QtWebEngineCore.framework/Versions/A/Helpers/" COMPONENT OpenStudioApp USE_SOURCE_PERMISSIONS)

  # TODO: this is definitely a shot in the dark and is unlikely to work. The WebEngine .paks in particular probably need to live somewhere else
  install(DIRECTORY $<TARGET_FILE_DIR:openstudio_lib>/translations/ DESTINATION "OpenStudioApp.app/Contents/translations/" COMPONENT OpenStudioApp)

  list(APPEND QTPLUGINS "\${CMAKE_INSTALL_PREFIX}/OpenStudioApp.app/Contents/Frameworks/QtWebEngineCore.framework/Versions/A/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess")
endif()

# TODO: We used to run fixup_bundle on all plaforms before on 5.15.0...
if(UNIX)
  # This does some "magic" **including on Linux** (cf #345) (On linux it will install in ./bin/ the libopenstudio.so, and libicu* stuff)
  install(CODE "
    # APPS might be a symbolic link which does not work with fixup_bundle
    # expand APPS to a real path
    get_filename_component(APPS_EXPANDED ${APPS} REALPATH)
    include(BundleUtilities)
    # This file defines some libs we are excluding. Some like concrt140.dll are installed already via InstallRequiredSystemLibraries
    include(\"${CMAKE_SOURCE_DIR}/CMake/FixupBundleOverrides.cmake\")
    fixup_bundle(\"\${APPS_EXPANDED}\" \"${QTPLUGINS}\" \"${DIRS}\")
    " COMPONENT "OpenStudioApp"
  )
endif()

set(${target_name}_test_src
  test/OpenStudioAppFixture.hpp
  test/OpenStudioAppFixture.cpp
  test/Resources_GTest.cpp
  test/Units_GTest.cpp
)

CREATE_SRC_GROUPS("${${target_name}_test_src}")
if(BUILD_TESTING)
  set(${target_name}_test_depends
    ${depends}
  )
  CREATE_TEST_TARGETS(${target_name} "${${target_name}_test_src}" "${${target_name}_test_depends}")

  #add_dependencies("${target_name}_tests"
  #  openstudio_app_resources
  #)

endif()
