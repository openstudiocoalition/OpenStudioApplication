# If you want to explicitly trigger an update of the TS Files
# make update_translations
# or, explicitly
# make translations_interface_lupdate

# lrelease (TS => QM) is added to the 'ALL' target and will always run

add_library(translations_interface INTERFACE)

set(TS_FILES
  OpenStudioApp_fr.ts # French
  OpenStudioApp_es.ts # Spanish
  OpenStudioApp_ar.ts # Arabic
  OpenStudioApp_he.ts # Hebrew
  OpenStudioApp_fa.ts # Farsi-Persian
  OpenStudioApp_it.ts # Italian
  OpenStudioApp_zh_CN.ts # Chinese
  OpenStudioApp_el.ts # Greek
  OpenStudioApp_pl.ts # Polish
  OpenStudioApp_ca.ts # Catalan
  OpenStudioApp_hi.ts # Hindi
  OpenStudioApp_vi.ts # Vietnamese
  OpenStudioApp_ja.ts # Japanese
  OpenStudioApp_de.ts # German
)

set(target_names
  openstudioapp_utilities
  openstudio_modeleditor
  openstudio_bimserver
  openstudio_lib
  OpenStudioApp
)
foreach(target_name ${target_names})
  get_target_property(TARGET_SOURCE_DIR ${target_name} SOURCE_DIR)
  get_target_property(TARGET_SOURCES ${target_name} SOURCES)
  list(APPEND TS_SOURCE_DIRS ${combined_path})
  foreach(source ${TARGET_SOURCES})
    get_filename_component(combined_path ${TARGET_SOURCE_DIR}/${source} ABSOLUTE)
    list(APPEND TS_SOURCES ${combined_path})
  endforeach()
endforeach()

list(REMOVE_DUPLICATES TS_SOURCE_DIRS)
list(REMOVE_DUPLICATES TS_SOURCES)
message("TS_SOURCE_DIRS=${TS_SOURCE_DIRS}")
message("TS_SOURCES=${TS_SOURCES}")

qt6_add_translations(translations_interface TS_FILES ${TS_FILES} QM_FILES_OUTPUT_VARIABLE QM_FILES
    INCLUDE_DIRECTORIES
      ${TS_SOURCE_DIRS}
    SOURCES
      ${TS_SOURCES}
)

message("QM_FILES=${QM_FILES}")

add_custom_target(translations ALL )
add_dependencies(translations translations_interface)

if(APPLE)
  set(WEBENGINE_PAK_FOLDER "${QT_INSTALL_DIR}/lib/QtWebEngineCore.framework/Versions/A/Resources/qtwebengine_locales")
else()
  set(WEBENGINE_PAK_FOLDER "${QT_INSTALL_DIR}/translations/qtwebengine_locales")
endif()

set(target_name openstudio_lib)
add_custom_command(TARGET translations
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy ${QM_FILES} $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # English
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_en.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_en.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/en-US.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # French
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_fr.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_fr.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/fr.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # Arabic
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_ar.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_ar.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/ar.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # Spanish
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_es.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_es.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/es.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # Farsi: unfortunaly qt / qtbase isn't translated. webengine is
  # COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_fa.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  # COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_fa.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/fa.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # Hebrew
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_he.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_he.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/he.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # Italian
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_it.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_it.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/it.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # Chinese: TODO NOTE: qtbase_zh_TW.qm only, there is no qtbase_zh_CN.qm...
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_zh_CN.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_zh_TW.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/zh-CN.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # Greek: : unfortunaly qt / qtbase isn't translated. webengine is
  # COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_el.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  # COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_el.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/el.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # Polish
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_pl.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_pl.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/pl.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # Catalan
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_ca.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_ca.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/ca.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # Hindi: unfortunaly qt / qtbase isn't translated. webengine is
  # COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_hi.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  # COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_hi.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/hi.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # Vietnamese: unfortunaly qt / qtbase isn't translated. webengine is
  # COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_vi.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  # COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_vi.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/vi.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # Japanese
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_ja.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_ja.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/ja.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/

  # German
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_de.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_de.qm $<TARGET_FILE_DIR:${target_name}>/translations/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/vi.pak $<TARGET_FILE_DIR:${target_name}>/translations/qtwebengine_locales/
)
