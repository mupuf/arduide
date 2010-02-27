cmake_minimum_required(VERSION 2.8)

# OS-dependant dependancies
if(CMAKE_HOST_WIN32)
	SET(Boost_USE_STATIC_LIBS ON)
elseif(CMAKE_HOST_APPLE)
elseif(CMAKE_HOST_UNIX)
	find_package(Qt4 COMPONENTS QtDbus REQUIRED)
	include_directories("/usr/include/grantlee") #Bad bad bad !!
endif(CMAKE_HOST_WIN32)

# Grantlee
include(FindGrantlee)
find_package(Grantlee REQUIRED)
set(QT_DONT_USE_QTGUI 0)
include_directories(${Grantlee_INCLUDE_DIRS})

# Qt
include(FindQt4)
find_package(Qt4 COMPONENTS QtCore QtGui QtWebkit REQUIRED)
include(${QT_USE_FILE})

# Boost
SET(Boost_ADDITIONAL_VERSIONS "1.41" "1.41.0")
include(FindBoost)
find_package(Boost COMPONENTS system REQUIRED) # for boost::asio
include_directories(${Boost_INCLUDE_DIRS})

# QScintilla
FIND_PATH(QSCINTILLA_INCLUDE_DIR Qsci/qsciscintilla.h )
IF(NOT QSCINTILLA_INCLUDE_DIR)
    
ENDIF()
FIND_PATH(QSCINTILLA_LIBRARY_PATH NAMES qscintilla2 )

# common
include_directories(BEFORE "${CMAKE_SOURCE_DIR}/utils/qxt")
include_directories(BEFORE "${QSCINTILLA_INCLUDE_DIR}")
include_directories(BEFORE "${QSCINTILLA_INCLUDE_DIR}/Qsci")
include_directories(BEFORE "${CMAKE_BINARY_DIR}")
include_directories(BEFORE "${CMAKE_SOURCE_DIR}")
link_directories(BEFORE "${QSCINTILLA_LIBRARY_PATH}")
link_directories(BEFORE "${GRANTLEE_LIBRARY_PATH}")

add_definitions(-DPROJECT_URL="http://mupuf.org/")

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(arduino_DATA_PATH "${CMAKE_SOURCE_DIR}/data" CACHE STRING "Path to the data files")
else()
  set(arduino_DATA_PATH "${CMAKE_INSTALL_PREFIX}/share/arduino-ide" CACHE STRING "Path to the data files")
endif()
add_definitions(-DDATA_PATH="${arduino_DATA_PATH}")

add_definitions(-DGRANTLEE_PLUGIN_DIR="/usr/lib/grantlee")
add_definitions(-DQT)

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

target_link_libraries(arduino ${QT_LIBRARIES} ${Boost_LIBRARIES} ${Grantlee_LIBRARIES} grantlee_core_library qscintilla2)

install(TARGETS arduino DESTINATION bin)

install(DIRECTORY "${arduino_DATA_PATH}/" DESTINATION share/arduino-ide)
