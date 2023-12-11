cmake_policy(SET CMP0048 NEW)
project(peruse VERSION "1.80.0")
cmake_minimum_required(VERSION 3.16.0)

set(KF_DEP_VERSION "5.98.0") # handled by release scripts
set(QT_DEP_VERSION "5.15")

if(POLICY CMP0063)
    cmake_policy (SET CMP0063 NEW)
endif(POLICY CMP0063)

include(FeatureSummary)

option(USE_PERUSE_PDFTHUMBNAILER "Use the internal thumbnail generator for PDF files (defaults to true on windows)" OFF)
add_feature_info(USE_PERUSE_PDFTHUMBNAILER ${USE_PERUSE_PDFTHUMBNAILER} "Use the internal thumbnail generator for PDF files (defaults to true on windows)")
if(WIN32)
    set(USE_PERUSE_PDFTHUMBNAILER ON)
endif()

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

find_package(ECM ${KF_DEP_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH})

# Android has no DBus, so we can't use that. Sniff things and tell the code
if(CMAKE_SYSTEM_NAME STREQUAL Android)
    message("Building for Android - this means no dbus, and other small details. Work with that")
    add_definitions(-DANDROID)
    SET(QT_QMAKE_EXECUTABLE "$ENV{Qt6_android}/bin/qmake")
elseif(WIN32)
    message("Building for Windows - this means no dbus, and other small details. Work with that")
    add_definitions(-DWINDOWS)
endif()

find_package(Qt6 ${QT_DEP_VERSION} REQUIRED NO_MODULE COMPONENTS Qml Quick Gui Widgets OpenGL Sql)

find_package(KF6 ${KF_DEP_VERSION} REQUIRED
    COMPONENTS
    Archive
    Config
    Crash
    Declarative
    GuiAddons
    FileMetaData
    I18n
    IconThemes
    KIO
    # For some reason, these show up in the OPTIONAL section if they're not explicitly listed, and that's just confusing, so...
    Attica
    Codecs
    Completion
    ConfigWidgets
    CoreAddons
    ItemViews
    JobWidgets
    NewStuffCore
    Package
    Service
    Solid
    WidgetsAddons
    WindowSystem
    XmlGui
)

if (NOT WIN32)
    find_package(KF6 ${KF_DEP_VERSION} REQUIRED COMPONENTS Auth)
endif()

find_package(KF6Baloo ${KF_DEP_VERSION}) # optional - we'll just let it pass if it's missing
set_package_properties(KF5Baloo PROPERTIES
    DESCRIPTION "KDE's Framework which implements file indexing"
    PURPOSE "Used by the ContentLister library to find books on the system if available (if unavailable, we will fall back to a classic file system scraper)"
    TYPE OPTIONAL
)
find_package(KF6Kirigami ${KF_DEP_VERSION})
set_package_properties(KF6Kirigami PROPERTIES
    DESCRIPTION "KDE's lightweight user interface framework for mobile and convergent applications"
    URL "https://techbase.kde.org/Kirigami"
    PURPOSE "Required at runtime to display the GUI"
    TYPE RUNTIME
)

find_package(KF6KirigamiAddons 0.11)
set_package_properties(KF6KirigamiAddons PROPERTIES
    TYPE REQUIRED
    PURPOSE "Provides additional visual components"
)

find_package(ZLIB)
set_package_properties(ZLIB PROPERTIES
    PURPOSE "Required for the unarr based rar support used for reading books in the CBR format"
    TYPE REQUIRED
)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(GenerateExportHeader)
include(ECMAddAppIcon)
include(ECMAddQch)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)

set(CMAKE_AUTORCC ON)

kde_enable_exceptions()

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX PERUSE
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION_HEADER "${CMAKE_BINARY_DIR}/config-peruse.h"
)

add_subdirectory(src)

ki18n_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
