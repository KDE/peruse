project(acbf)

set(acbf_SRCS
    AcbfAuthor.cpp
    AcbfBody.cpp
    AcbfBookinfo.cpp
    AcbfContentrating.cpp
    AcbfDatabaseref.cpp
    AcbfDocument.cpp
    AcbfDocumentinfo.cpp
    AcbfLanguage.cpp
    AcbfMetadata.cpp
    AcbfPage.cpp
    AcbfPublishinfo.cpp
    AcbfSequence.cpp
    AcbfTextarea.cpp
    AcbfTextlayer.cpp
)

add_library(acbf SHARED ${acbf_SRCS})
generate_export_header(acbf BASE_NAME acbf)

target_link_libraries(acbf
    PUBLIC
    Qt5::Core
)

install(TARGETS acbf ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
