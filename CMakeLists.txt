project(peruse)
cmake_minimum_required(VERSION 2.8.12)

if(POLICY CMP0063)
    cmake_policy (SET CMP0063 NEW)
endif(POLICY CMP0063)

find_package(ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH})

# Android has no DBus, so we can't use that. Sniff things and tell the code
if(CMAKE_SYSTEM_NAME STREQUAL Android)
    message("Building for Android - this means no dbus, and other small details. Work with that")
    add_definitions(-DANDROID)
    SET(QT_QMAKE_EXECUTABLE "$ENV{Qt5_android}/bin/qmake")
elseif(WIN32)
    message("Building for Windows - this means no dbus, and other small details. Work with that")
    add_definitions(-DWINDOWS)
endif()

find_package(Qt5 REQUIRED NO_MODULE COMPONENTS Qml Quick Gui Widgets OpenGL)
find_package(KF5 5.5 REQUIRED COMPONENTS Plasma I18n Declarative Config FileMetaData IconThemes KIO)

find_package(KF5Baloo) # optional - we'll just let it pass if it's missing
find_package(KF5Archive ${KF5_VERSION} REQUIRED)
find_package(KF5Kirigami ${KF5_VERSION} REQUIRED)

find_package(KF5Kirigami)
set_package_properties(KF5Kirigami PROPERTIES
    PURPOSE "Required at runtime to display the GUI"
    TYPE RUNTIME
)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(GenerateExportHeader)
include(ECMInstallIcons)
include(ECMAddAppIcon)

kde_enable_exceptions()

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
