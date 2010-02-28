cmake_minimum_required(VERSION 2.6)

include_directories(BEFORE "${CMAKE_BINARY_DIR}")
include_directories(BEFORE "${CMAKE_SOURCE_DIR}")
include_directories(BEFORE "${CMAKE_SOURCE_DIR}/utils/qxt")

set(PROJECT_NAME "Arduino IDE")
set(PROJECT_URL "http://mupuf.org/project/arduide/")
set(PROJECT_AUTHORS "Denis Martinez, Martin Peres")
set(PROJECT_ORGANIZATION "MuPuF.org")
set(PROJECT_LICENSE "GPLv2 or later")
set(PROJECT_VERSION "0.1")
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")
add_definitions(-DPROJECT_URL="${PROJECT_URL}")
add_definitions(-DPROJECT_AUTHORS="${PROJECT_AUTHORS}")
add_definitions(-DPROJECT_ORGANIZATION="${PROJECT_ORGANIZATION}")
add_definitions(-DPROJECT_LICENSE="${PROJECT_LICENSE}")
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")

# Data path
option(USE_FHS_PATHS "Use the standard UNIX paths instead of installing everything in the same folder." OFF)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  # when debugging, the data files are always searched inside the source directory
  add_definitions(-DDATA_PATH="${CMAKE_SOURCE_DIR}/data")
else()
  set(arduino_DATA_PATH "${CMAKE_INSTALL_PREFIX}/share/arduino-ide")
  if(USE_FHS_PATHS)
    # search data files in a static directory (e.g. Unix)
    add_definitions(-DDATA_PATH="${arduino_DATA_PATH}")
  else()
    # search data files in the current directory (e.g. Windows)
    add_definitions(-DDATA_PATH=".")
  endif()
endif()

# OS-dependent
if(CMAKE_HOST_WIN32)
  set(Boost_USE_STATIC_LIBS ON)
elseif(CMAKE_HOST_APPLE)
  find_library(CoreFoundation_LIBRARY CoreFoundation)
  find_library(IOKit_LIBRARY IOKit)
  if(CoreFoundation_LIBRARY STREQUAL CoreFoundation_LIBRARY-NOTFOUND)
    message(SEND_ERROR "CoreFoundation library not found.")
  endif()
  if(IOKit_LIBRARY STREQUAL IOKit_LIBRARY-NOTFOUND)
    message(SEND_ERROR "IOKit library not found.")
  endif()
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

set(Qt4_LINK_LIBRARIES ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTWEBKIT_LIBRARY})
if(CMAKE_HOST_APPLE)
elseif(CMAKE_HOST_UNIX)
  set(Qt4_LINK_LIBRARIES ${Qt4_LINK_LIBRARIES} ${QT_QTDBUS_LIBRARY})
endif()

# Grantlee
include(${CMAKE_SOURCE_DIR}/cmake/FindGrantlee.cmake)
find_package(Grantlee REQUIRED)
include_directories(${Grantlee_INCLUDE_DIR})
add_definitions(-DGRANTLEE_PLUGIN_DIR="${Grantlee_DIR}")

# QScintilla
find_library(QScintilla_LIBRARY qscintilla2
  HINTS ${QT_LIBRARY_DIR})
if(QScintilla_LIBRARY STREQUAL QScintilla_LIBRARY-NOTFOUND)
  message(SEND_ERROR "QScintilla library not found.")
endif()

# Boost
set(Boost_ADDITIONAL_VERSIONS "1.41" "1.41.0" "1.42" "1.42.0")
include(FindBoost)
find_package(Boost COMPONENTS system REQUIRED) # for boost::asio
include_directories(${Boost_INCLUDE_DIRS})

# Build
file(GLOB arduino_SOURCES
  "*.cpp" "*.h"
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
add_executable(arduino-ide
  ${arduino_SOURCES}
  ${arduino_UI_SOURCES}
  ${arduino_QRC_SOURCES}
  ${arduino_MOC_SOURCES})
target_link_libraries(arduino-ide ${Qt4_LINK_LIBRARIES} ${Grantlee_LIBRARY} ${Boost_LIBRARIES} ${QScintilla_LIBRARY} ${CoreFoundation_LIBRARY} ${IOKit_LIBRARY})

# Install
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  message("Cannot install a Debug build, run CMake again if you would like to install.")
else()
  install(DIRECTORY "${CMAKE_SOURCE_DIR}/data/" DESTINATION "${arduino_DATA_PATH}")
  if(USE_FHS_PATHS)
    install(TARGETS arduino-ide DESTINATION bin)
  else()
    install(TARGETS arduino-ide DESTINATION "${arduino_DATA_PATH}")
  endif()
endif()

### Packages
# General
set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${CMAKE_PROJECT_NAME};ALL;/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ArduIDE is a Qt-based IDE for the open-source Arduino electronics prototyping platform.")
set(CPACK_PACKAGE_EXECUTABLES "arduino;The Qt Arduino IDE.")
set(CPACK_PACKAGE_FILE_NAME "arduide-0.1a")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Arduide 0.1 alpha")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "Arduide 0.1 alpha")
set(CPACK_PACKAGE_NAME "ArduIDE")
set(CPACK_PACKAGE_VENDOR "MùPùF.org")
set(CPACK_PACKAGE_VERSION "0.1a")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_STRIP_FILES "")
set(CPACK_GENERATOR "TGZ;DEB;RPM")

set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README")

# Debian
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Martin PERES <martin.peres@ensi-bourges.fr>")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libgcc1 (>= 1:3.4.2-12)")
set(DEBIAN_PACKAGE_RECOMMENDS "libqtgui4 (>=4.0.0-1), libasio-dev (>=1.1.1-1)")
set(DEBIAN_PACKAGE_SUGGESTS "")
set(DEBIAN_PACKAGE_BUILDS_DEPENDS "cmake (>=2.6), libqt4-dev")

# RPM
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_GROUP "MùPùF.org")
set(CPACK_RPM_PACKAGE_LICENSE "LGPL v2+")
set(CPACK_RPM_PACKAGE_REQUIRES "qt >= 4.0, asio")

# Binary TGZ
set(CPACK_PACKAGE_CONTACT "http://arduide.mupuf.org")
#SET(CPACK_RESOURCE_FILE_WELCOME "")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "CPackSourceConfig.cmake")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "arduide-0.1a.tar.gz")
set(CPACK_SOURCE_STRIP_FILES "")

include(CPack)
