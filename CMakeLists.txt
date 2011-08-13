cmake_minimum_required(VERSION 2.6)

include(CheckIncludeFileCXX)

include_directories(BEFORE "${CMAKE_BINARY_DIR}")
include_directories(BEFORE "${CMAKE_SOURCE_DIR}")
include_directories(BEFORE "${CMAKE_SOURCE_DIR}/utils/qxt")

project(arduide)
set(PROJECT_NAME "Arduino IDE")
set(PROJECT_SHORT_NAME "ArduIDE")
set(PROJECT_URL "http://mupuf.org/project/arduide/")
set(PROJECT_AUTHORS "Denis Martinez, Martin Peres")
set(PROJECT_ORGANIZATION "MuPuF.org")
set(PROJECT_LICENSE "GPLv2 or later")
set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")
add_definitions(-DPROJECT_SHORT_NAME="${PROJECT_SHORT_NAME}")
add_definitions(-DPROJECT_URL="${PROJECT_URL}")
add_definitions(-DPROJECT_AUTHORS="${PROJECT_AUTHORS}")
add_definitions(-DPROJECT_ORGANIZATION="${PROJECT_ORGANIZATION}")
add_definitions(-DPROJECT_LICENSE="${PROJECT_LICENSE}")
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")

# Compatible version of the Arduino SDK
set(ARDUINO_SDK_VERSION "0022")
add_definitions(-DARDUINO_SDK_VERSION="${ARDUINO_SDK_VERSION}")

# Compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

# Data path
option(USE_FHS_PATHS "Use the standard UNIX paths instead of installing everything in the same folder." OFF)

option (UPDATE_TRANSLATIONS "Update source translation translations/*.ts files (WARNING: make clean will delete the source .ts files! Danger!)")

# arduino_DATA_PATH is where the files are installed
# the DATA_PATH define is where the files are found at runtime
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  # when debugging, the data files are always searched inside the source directory
  add_definitions(-DDATA_PATH="${CMAKE_SOURCE_DIR}/data")
  set(USE_FHS_PATHS ON)
else()
  if(USE_FHS_PATHS)
    # search data files in a static directory (e.g. Unix)
    set(arduino_DATA_PATH "${CMAKE_INSTALL_PREFIX}/share/arduino-ide")
    add_definitions(-DDATA_PATH="${arduino_DATA_PATH}")
  else()
    # search data files in the current directory (e.g. Windows)
    set(arduino_DATA_PATH "/.") # install at the root of the package (CPack)
    add_definitions(-DDATA_PATH=".")
  endif()
endif()

# OS-dependent
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(Win32_LIBRARIES wsock32)
  if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    set(GUI_TYPE WIN32)
    set(DEV_LISTING "Register")
  endif()
  add_definitions(-D_WIN32_WINNT=0x501) # Windows XP target
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  find_library(CoreFoundation_LIBRARY CoreFoundation)
  find_library(IOKit_LIBRARY IOKit)
  if(CoreFoundation_LIBRARY STREQUAL "CoreFoundation_LIBRARY-NOTFOUND")
    message(SEND_ERROR "CoreFoundation library not found.")
  endif()
  if(IOKit_LIBRARY STREQUAL "IOKit_LIBRARY-NOTFOUND")
    message(SEND_ERROR "IOKit library not found.")
  endif()
  set(GUI_TYPE MACOSX_BUNDLE)
  set(DEV_LISTING "kIOSerial")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_library(UDEV_LIBRARY udev)
  check_include_file_cxx("libudev.h" LIBUDEV_H_FOUND)
  if(UDEV_LIBRARY AND LIBUDEV_H_FOUND)
    # use udev
    add_definitions(-DUSE_LIBUDEV)
    set(UDEV_LIBRARIES ${UDEV_LIBRARY})
    set(DEV_LISTING "udev")
  else()
    # use dbus
    set(USE_DBUS ON)
    add_definitions(-DUSE_DBUS)
    set(DEV_LISTING "hal")
  endif()

  # add grantlee's default directory on Linux systems
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/usr/lib/grantlee")
endif()

# Qt
set(Qt4_COMPONENTS QtCore QtGui QtNetwork QtWebkit)
if(USE_DBUS)
  set(Qt4_COMPONENTS ${Qt4_COMPONENTS} QtDbus)
endif()
include(FindQt4)
find_package(Qt4 COMPONENTS ${Qt4_COMPONENTS} REQUIRED)
include(${QT_USE_FILE})

set(Qt4_LINK_LIBRARIES ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTWEBKIT_LIBRARY})
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(Qt4_LINK_LIBRARIES ${Qt4_LINK_LIBRARIES} ${QT_QTDBUS_LIBRARY})
endif()

# Grantlee
if(Grantlee_HOME_DIR)
  # this variable can be set manually as the path to GrantleeConfig.cmake
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${Grantlee_HOME_DIR})
endif()
find_package(Grantlee REQUIRED)
include(${Grantlee_USE_FILE})
if(USE_FHS_PATHS)
  add_definitions(-DGRANTLEE_PLUGIN_DIR="${Grantlee_PLUGIN_PATH}")
else()
  # grantlee plugins reside in the application directory
  add_definitions(-DGRANTLEE_PLUGIN_DIR=".")
endif()

# QScintilla
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  find_library(QScintilla_LIBRARY NAMES libqscintilla2.a qscintilla2.lib HINTS ${QT_LIBRARY_DIR})
else()
  find_library(QScintilla_LIBRARY qscintilla2 HINTS ${QT_LIBRARY_DIR})
endif()

# Resources
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/icon.obj"
      COMMAND "${CMAKE_RC_COMPILER}" -O coff -i "${CMAKE_SOURCE_DIR}/win32/icon.rc" -o "${CMAKE_BINARY_DIR}/icon.obj"
      DEPENDS "${CMAKE_SOURCE_DIR}/win32/icon.rc")
    set(rc_SOURCES "${CMAKE_BINARY_DIR}/icon.obj")
  else()
    set(rc_SOURCES win32/icon.rc)
  endif()
endif()

if(QScintilla_LIBRARY)
  message(STATUS "QScintilla library: ${QScintilla_LIBRARY}")
  get_filename_component(QScintilla_LIBRARY_PATH "${QScintilla_LIBRARY}" PATH)
  find_path(QScintilla_INCLUDE_PATH Qsci/qsciscintilla.h HINTS ${QT_INCLUDE_DIR})
  if(EXISTS "${QScintilla_INCLUDE_PATH}/Qsci/qsciscintilla.h")
    include_directories("${QScintilla_INCLUDE_PATH}")
    message(STATUS "QScintilla includes: ${QScintilla_INCLUDE_PATH}")
  else()
    message(SEND_ERROR "QScintilla headers not found.")
  endif()
else()
  message(SEND_ERROR "QScintilla library not found.")
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  add_definitions(-DQSCINTILLA_DLL)
endif()

# Utils
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  file(GLOB utils_OS_SOURCES
    "utils/win32/*.cpp" "utils/win32/*.h")
else()
  file(GLOB utils_OS_SOURCES
    "utils/unix/*.cpp" "utils/unix/*.h")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  file(GLOB utils_SOURCES
    "utils/*.cpp" "utils/*.h"
    "utils/qxt/*.cpp" "utils/qxt/*.h"
    "utils/qxt/win/*.cpp" "utils/qxt/win/*.h"
    "utils/hexview/*.cpp" "utils/hexview/*.h")
else()
  file(GLOB utils_SOURCES
    "utils/*.cpp" "utils/*.h"
    "utils/qxt/*.cpp" "utils/qxt/*.h"
    "utils/qxt/unix/*.cpp" "utils/qxt/unix/*.h"
    "utils/hexview/*.cpp" "utils/hexview/*.h")
endif()

file(GLOB utils_MOCS
  "utils/*.h"
  "utils/qxt/*.h"
  "utils/hexview/*.h")

qt4_wrap_cpp(utils_MOC_SOURCES ${utils_MOCS})

# Build
file(GLOB arduino_SOURCES
  "*.cpp" "*.h"
  "gui/*.h" "gui/*.cpp"
  "env/*.cpp" "env/*.h"
  "plugins/*.cpp" "plugins/*.h")

file(GLOB arduino_UIS
  "gui/*.ui")

file(GLOB arduino_MOCS
  "gui/*.h"
  "env/*.h")

set(arduino_TS
	"translations/template_arduide_xx.ts"
	"translations/arduide_fr.ts"
	"translations/arduide_es.ts")

set(arduino_QRCS
  "resources/css.qrc"
  "resources/images.qrc"
  "resources/js.qrc"
  "resources/templates.qrc")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(arduino_QRCS ${arduino_QRCS} "resources/windows.qrc")
endif()

qt4_wrap_ui(arduino_UI_SOURCES ${arduino_UIS})
qt4_wrap_cpp(arduino_MOC_SOURCES ${arduino_MOCS})
qt4_add_resources(arduino_QRC_SOURCES ${arduino_QRCS})

if (UPDATE_TRANSLATIONS)
  set (FILES_TO_TRANSLATE ${arduino_SOURCES}
  ${arduino_UI_SOURCES}
  ${arduino_QRC_SOURCES}
  ${arduino_MOC_SOURCES}
  ${utils_SOURCES}
  ${utils_OS_SOURCES}
  ${utils_MOC_SOURCES}
  ${rc_SOURCES})
  qt4_create_translation(arduino_QM ${FILES_TO_TRANSLATE} ${arduino_TS})
else (UPDATE_TRANSLATIONS)
  qt4_add_translation(arduino_QM ${arduino_TS})
endif (UPDATE_TRANSLATIONS)
add_custom_target (translations_target DEPENDS ${arduino_QM})

install(FILES ${arduino_QM} DESTINATION
${CMAKE_INSTALL_PREFIX}/translations)

set(arduino_LINK_LIBRARIES ${Qt4_LINK_LIBRARIES} ${Grantlee_CORE_LIBRARIES} ${QScintilla_LIBRARY} ${CoreFoundation_LIBRARY} ${IOKit_LIBRARY} ${Win32_LIBRARIES} ${UDEV_LIBRARIES})

add_executable(arduino-ide
  ${GUI_TYPE}
  ${arduino_SOURCES}
  ${arduino_UI_SOURCES}
  ${arduino_QRC_SOURCES}
  ${arduino_MOC_SOURCES}
  ${utils_SOURCES}
  ${utils_OS_SOURCES}
  ${utils_MOC_SOURCES}
  ${rc_SOURCES}
  ${arduino_QM})
set_target_properties(arduino-ide PROPERTIES
  ENABLE_EXPORTS ON)
target_link_libraries(arduino-ide ${arduino_LINK_LIBRARIES})

# Plugins
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(arduino_PLUGIN_PATH "${CMAKE_BINARY_DIR}/plugins")
  add_definitions(-DPLUGIN_PATH="${arduino_PLUGIN_PATH}")
else()
  if(USE_FHS_PATHS)
    set(arduino_PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/share/arduino-ide/plugins")
    add_definitions(-DPLUGIN_PATH="${arduino_PLUGIN_PATH}")
  else()
    set(arduino_PLUGIN_PATH "/plugins")
    add_definitions(-DPLUGIN_PATH="plugins")
  endif()
endif()
add_subdirectory(plugins)

# Translations
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(arduino_TRANSLATION_PATH "${CMAKE_BINARY_DIR}")
  add_definitions(-DTRANSLATION_PATH="${arduino_TRANSLATION_PATH}")
else()
  if(USE_FHS_PATHS)
    set(arduino_TRANSLATION_PATH "${CMAKE_INSTALL_PREFIX}/share/arduino-ide/translations")
    add_definitions(-DTRANSLATION_PATH="${arduino_TRANSLATION_PATH}")
  else()
    set(arduino_TRANSLATION_PATH "/translations")
    add_definitions(-DTRANSLATION_PATH="translations")
  endif()
endif()

# Install
if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
  install(DIRECTORY "${CMAKE_SOURCE_DIR}/data/" DESTINATION "${arduino_DATA_PATH}")
  if(USE_FHS_PATHS)
    install(TARGETS arduino-ide DESTINATION bin)
  else()
    install(TARGETS arduino-ide DESTINATION "${arduino_DATA_PATH}")

    # in non-fhs mode we must also install the plugins in the application directory
  #   install(DIRECTORY "${Grantlee_PLUGIN_PATH}/"
  #     DESTINATION "${arduino_DATA_PATH}/grantlee" FILES_MATCHING
  #     PATTERN "*.so"
  #     PATTERN "*.dylib"
  #     PATTERN "*.dll")
  endif()
endif()

install(FILES ${arduino_QM} DESTINATION "${arduino_TRANSLATION_PATH}")

### Packages
set(CPACK_SET_DESTDIR ON)

# General
set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${CMAKE_PROJECT_NAME};ALL;/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} is a Qt-based IDE for the open-source Arduino electronics prototyping platform.")
set(CPACK_PACKAGE_EXECUTABLES "arduino-ide;The Qt Arduino IDE.")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_SHORT_NAME}-${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME} ${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${PROJECT_NAME} ${PROJECT_VERSION}")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "${PROJECT_ORGANIZATION}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_STRIP_FILES "")
set(CPACK_GENERATOR "TGZ;DEB;RPM")

set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README")

# Debian
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Martin PERES <martin.peres@ensi-bourges.fr>")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libgcc1 (>= 1:3.4.2-12), libqtgui4 (>=4.4.0-1)")
set(DEBIAN_PACKAGE_RECOMMENDS "")
set(DEBIAN_PACKAGE_SUGGESTS "")
set(DEBIAN_PACKAGE_BUILDS_DEPENDS "cmake (>=2.6), libqt4-dev")

# RPM
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_GROUP "${PROJECT_ORGANIZATION}")
set(CPACK_RPM_PACKAGE_LICENSE "${PROJECT_LICENSE}")
set(CPACK_RPM_PACKAGE_REQUIRES "qt >= 4.0")

# Binary TGZ
set(CPACK_PACKAGE_CONTACT "${PROJECT_URL}")
#SET(CPACK_RESOURCE_FILE_WELCOME "")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "CPackSourceConfig.cmake")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_SHORT_NAME}-${PROJECT_VERSION}.tar.gz")
set(CPACK_SOURCE_STRIP_FILES "")

include(CPack)

#Messages
message("")
message("-- Summary:")
message("* Arduino SDK compatible version: ${ARDUINO_SDK_VERSION}")
message("* Device listing method: ${DEV_LISTING} ")
message("* Use the Filesystem Hierarchy Standard: USE_FHS_PATHS=${USE_FHS_PATHS}")
message(" OFF: the software is installed in a single directory, good for binary distributions")
message(" ON: the software is installed in a Unix prefix, good for packagers")
message("* Compilation type: CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
message(" Debug: makes the application directly runnable from the build tree, for developers")
message("  - Note that you can't install in Debug mode.")
message(" Release/Other: you must install the application before you can run it")
message("* Update translations: UPDATE_TRANSLATIONS=${UPDATE_TRANSLATIONS}")
message("    WARNING: Do not run \"make clean\" when in update translation mode!")
message("    Run \"make translations_target\" to re-generate translations")
message("")
