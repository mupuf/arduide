set(PLUGINS "serial" "debugger")

file(GLOB plugins_SOURCES
  "*.h" "*.cpp")
list(APPEND arduino_SOURCES ${plugins_SOURCES})
set(arduino_SOURCES ${arduino_SOURCES} PARENT_SCOPE)

add_definitions(-DIDE_PLUGIN)

foreach(PLUGIN ${PLUGINS})
  file(GLOB plugin_SOURCES
    "${PLUGIN}/*.h"
    "${PLUGIN}/*.cpp")

  file(GLOB plugin_UIS
    "${PLUGIN}/*.ui")

  file(GLOB plugin_MOCS
    "${PLUGIN}/*.h")

  file(GLOB plugin_QRCS
    "${PLUGIN}/*.qrc")

  # Empty the variables as qt4_wrap_* would append the result of the
  # previous call with the new result.
  set(plugin_UI_SOURCES "")
  set(plugin_MOC_SOURCES "")
  set(plugin_QRC_SOURCES "")

  qt4_wrap_ui(plugin_UI_SOURCES ${plugin_UIS})
  qt4_wrap_cpp(plugin_MOC_SOURCES ${plugin_MOCS})
  qt4_add_resources(plugin_QRC_SOURCES ${plugin_QRCS})

  add_library(${PLUGIN} MODULE
    ${plugins_SOURCES}
    ${plugin_SOURCES}
    ${plugin_UI_SOURCES}
    ${plugin_MOC_SOURCES}
    ${plugin_QRC_SOURCES})
  target_link_libraries(${PLUGIN} arduino-ide ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QScintilla_LIBRARY})
  set_target_properties(${PLUGIN} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
endforeach()

# Install
if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
  install(TARGETS ${PLUGINS}
    LIBRARY DESTINATION "${arduino_PLUGIN_PATH}")
endif()
