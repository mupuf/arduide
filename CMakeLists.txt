cmake_minimum_required(VERSION 2.6)

include(FindQt4)
include(FindBoost)
find_package(Qt4 COMPONENTS QtCore QtGui QtDbus QtWebkit REQUIRED)
find_package(Boost COMPONENTS system REQUIRED) # for boost::asio
include(${QT_USE_FILE})
include_directories(${Boost_INCLUDE_DIRS})

include_directories(BEFORE "${CMAKE_BINARY_DIR}")
include_directories(BEFORE "${CMAKE_SOURCE_DIR}")
include_directories(BEFORE "${CMAKE_SOURCE_DIR}/utils/qxt")

add_definitions(-DPROJECT_URL="http://mupuf.org/")

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(arduino_DATA_PATH "${CMAKE_SOURCE_DIR}/data" CACHE STRING "Path to the data files")
else()
  set(arduino_DATA_PATH "${CMAKE_INSTALL_PREFIX}/share/arduino-ide" CACHE STRING "Path to the data files")
endif()
add_definitions(-DDATA_PATH="${arduino_DATA_PATH}")

add_definitions(-DGRANTLEE_PLUGIN_DIR="/usr/lib/grantlee")

file(GLOB arduino_SOURCES
  "main.cpp"
  "gui/*.h" "gui/*.cpp"
  "env/*.cpp" "env/*.h"
  "utils/*.cpp" "utils/*.h"
  "utils/qxt/*.cpp" "utils/qxt/*.h")

file(GLOB arduino_UIS
  "gui/*.ui")

file(GLOB arduino_MOCS
  "gui/*.h"
  "env/*.h"
  "utils/qxt/*.h")

file(GLOB arduino_QRCS
  "resources/*.qrc")

qt4_wrap_ui(arduino_UI_SOURCES ${arduino_UIS})
qt4_wrap_cpp(arduino_MOC_SOURCES ${arduino_MOCS})
qt4_add_resources(arduino_QRC_SOURCES ${arduino_QRCS})
add_executable(arduino
  ${arduino_SOURCES}
  ${arduino_UI_SOURCES}
  ${arduino_QRC_SOURCES}
  ${arduino_MOC_SOURCES})
target_link_libraries(arduino ${QT_LIBRARIES} ${Boost_LIBRARIES} qscintilla2 grantlee_core_library)
