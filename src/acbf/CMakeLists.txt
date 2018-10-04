project(acbf)

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
    AcbfJump.cpp
    AcbfLanguage.cpp
    AcbfMetadata.cpp
    AcbfPage.cpp
    AcbfPublishinfo.cpp
    AcbfReferences.cpp
    AcbfReference.cpp
    AcbfSequence.cpp
    AcbfStyleSheet.cpp
    AcbfTextarea.cpp
    AcbfTextlayer.cpp
)

ecm_qt_declare_logging_category(acbf_SRCS
    HEADER acbf_debug.h
    IDENTIFIER ACBF_LOG
    CATEGORY_NAME org.kde.peruse.acbf
    DEFAULT_SEVERITY Warning
)

add_library(acbf SHARED ${acbf_SRCS})
generate_export_header(acbf BASE_NAME acbf)

target_link_libraries(acbf
    PUBLIC
    Qt5::Core
)

set_target_properties(acbf PROPERTIES
   VERSION 0.1
   SOVERSION 0.1
   EXPORT_NAME AdvancedComicBookFormat
)

install(TARGETS acbf ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
