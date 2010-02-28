cmake_minimum_required(VERSION 2.8)

include_directories(BEFORE "${CMAKE_BINARY_DIR}")
include_directories(BEFORE "${CMAKE_SOURCE_DIR}")
include_directories(BEFORE "${CMAKE_SOURCE_DIR}/utils/qxt")
add_definitions(-DPROJECT_URL="http://mupuf.org/")

# OS-dependent
if(CMAKE_HOST_WIN32)
  set(Boost_USE_STATIC_LIBS ON)
elseif(CMAKE_HOST_APPLE)
endif()

# Qt
set(Qt4_COMPONENTS QtCore QtGui QtWebkit)
if(CMAKE_HOST_APPLE)
elseif(CMAKE_HOST_UNIX)
  set(Qt4_COMPONENTS ${Qt4_COMPONENTS} QtDbus)
endif()
include(FindQt4)
find_package(Qt4 COMPONENTS ${Qt4_COMPONENTS} REQUIRED)
include(${QT_USE_FILE})

# Grantlee
include(${CMAKE_SOURCE_DIR}/cmake/FindGrantlee.cmake)
find_package(Grantlee REQUIRED)
include_directories(${Grantlee_INCLUDE_DIR})
add_definitions(-DGRANTLEE_PLUGIN_DIR="${Grantlee_DIR}")

# QScintilla
find_library(QScintilla_LIBRARY qscintilla2
  HINTS /opt/local/libexec/qt4-mac/lib)
if(QScintilla_LIBRARY STREQUAL QScintilla_LIBRARY-NOTFOUND)
  message(SEND_ERROR "QScintilla library not found.")
endif()

# Boost
set(Boost_ADDITIONAL_VERSIONS "1.41" "1.41.0")
include(FindBoost)
find_package(Boost COMPONENTS system REQUIRED) # for boost::asio
include_directories(${Boost_INCLUDE_DIRS})

# Data path
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(arduino_DATA_PATH "${CMAKE_SOURCE_DIR}/data" CACHE STRING "Path to the data files")
else()
  set(arduino_DATA_PATH "${CMAKE_INSTALL_PREFIX}/share/arduino-ide" CACHE STRING "Path to the data files")
endif()
add_definitions(-DDATA_PATH="${arduino_DATA_PATH}")

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
target_link_libraries(arduino ${QT_LIBRARIES} ${Grantlee_LIBRARY} ${Boost_LIBRARIES} ${QScintilla_LIBRARY})
