set(acbf_SRCS
    AcbfAuthor.cpp
    AcbfBinary.cpp
    AcbfBody.cpp
    AcbfBookinfo.cpp
    AcbfContentrating.cpp
    AcbfData.cpp
    AcbfDatabaseref.cpp
    AcbfDocument.cpp
    AcbfDocumentinfo.cpp
    AcbfFrame.cpp
    AcbfIdentifiedObjectModel.cpp
    AcbfInternalReference.cpp
    AcbfInternalReferenceObject.cpp
    AcbfJump.cpp
    AcbfLanguage.cpp
    AcbfMetadata.cpp
    AcbfPage.cpp
    AcbfPublishinfo.cpp
    AcbfReferences.cpp
    AcbfReference.cpp
    AcbfSequence.cpp
    AcbfStyle.cpp
    AcbfStyleSheet.cpp
    AcbfTextarea.cpp
    AcbfTextlayer.cpp
)

set(acbf_HEADERS
    AcbfAuthor.h
    AcbfBinary.h
    AcbfBody.h
    AcbfBookinfo.h
    AcbfContentrating.h
    AcbfData.h
    AcbfDatabaseref.h
    AcbfDocument.h
    AcbfDocumentinfo.h
    AcbfFrame.h
    AcbfIdentifiedObjectModel.h
    AcbfInternalReference.h
    AcbfInternalReferenceObject.h
    AcbfJump.h
    AcbfLanguage.h
    AcbfMetadata.h
    AcbfPage.h
    AcbfPublishinfo.h
    AcbfReferences.h
    AcbfReference.h
    AcbfSequence.h
    AcbfStyle.h
    AcbfStyleSheet.h
    AcbfTextarea.h
    AcbfTextlayer.h
)

ecm_qt_declare_logging_category(acbf_SRCS
    HEADER acbf_debug.h
    IDENTIFIER ACBF_LOG
    CATEGORY_NAME org.kde.peruse.acbf
    DEFAULT_SEVERITY Warning
    DESCRIPTION "Advanced Comic Book Format (aka ACBF)"
    EXPORT AdvancedComicBookFormat
)

add_library(acbf SHARED ${acbf_SRCS})
include(ECMGenerateExportHeader)
ecm_generate_export_header(acbf
    EXPORT_FILE_NAME acbf_export.h
    BASE_NAME acbf
    VERSION ${PROJECT_VERSION}
)

target_link_libraries(acbf
    PUBLIC
    Qt6::Core
)
target_include_directories(acbf
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF6}/AdvancedComicBookFormat>"
)

set_target_properties(acbf PROPERTIES
   VERSION ${PROJECT_VERSION}
   SOVERSION ${PERUSE_SOVERSION}
   OUTPUT_NAME acbf
   EXPORT_NAME AdvancedComicBookFormat
)

ecm_setup_version(PROJECT VARIABLE_PREFIX AdvancedComicBookFormat
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/acbf_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/AdvancedComicBookFormatConfigVersion.cmake"
    SOVERSION ${PERUSE_SOVERSION}
)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/AdvancedComicBookFormat")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS acbf_QCH
        FILE AdvancedComicBookFormatQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/AdvancedComicBookFormatQchTargets.cmake\")")

    ecm_add_qch(
        acbf_QCH
        NAME AdvancedComicBookFormat
        BASE_NAME AdvancedComicBookFormat
        VERSION ${PROJECT_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${acbf_HEADERS}
        MD_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
        IMAGE_DIRS "${CMAKE_SOURCE_DIR}/docs/pics"
        LINK_QCHS
            Qt6Core_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            ACBF_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/AdvancedComicBookFormatConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/AdvancedComicBookFormatConfig.cmake"
    INSTALL_DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/AdvancedComicBookFormatConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/AdvancedComicBookFormatConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)
install(FILES
    ${acbf_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/acbf_version.h
    DESTINATION "${KDE_INSTALL_INCLUDEDIR}/AdvancedComicBookFormat" COMPONENT Devel
)

install(TARGETS acbf EXPORT AdvancedComicBookFormatTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(EXPORT AdvancedComicBookFormatTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE AdvancedComicBookFormatTargets.cmake )
ecm_qt_install_logging_categories(
    EXPORT AdvancedComicBookFormat
    FILE AdvancedComicBookFormat.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
