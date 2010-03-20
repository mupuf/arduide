set(PLUGINS serial)

file(GLOB plugins_SOURCES
  "*.h" "*.cpp")
list(APPEND arduino_SOURCES ${plugins_SOURCES})
set(arduino_SOURCES ${arduino_SOURCES} PARENT_SCOPE)

foreach(PLUGIN ${PLUGINS})
  file(GLOB plugin_SOURCES
    "${PLUGIN}/*.h"
    "${PLUGIN}/*.cpp")
  add_library(${PLUGIN} MODULE ${plugin_SOURCES})
  set_target_properties(${PLUGIN} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  # Install
endforeach()

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
  install(TARGETS ${PLUGINS}
    LIBRARY DESTINATION "${arduino_PLUGIN_PATH}")
endif()
