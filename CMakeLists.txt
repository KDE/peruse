project(peruse)
cmake_minimum_required(VERSION 2.8.12)

if(POLICY CMP0063)
    cmake_policy (SET CMP0063 NEW)
endif(POLICY CMP0063)

find_package(ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED NO_MODULE COMPONENTS Qml Quick Gui Widgets)
find_package(KF5 5.5 REQUIRED COMPONENTS Plasma I18n Declarative Config DBusAddons)

find_package(KF5Baloo)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(GenerateExportHeader)
include(ECMInstallIcons)

kde_enable_exceptions()

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
