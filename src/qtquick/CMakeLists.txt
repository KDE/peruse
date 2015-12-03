set(qmlplugin_SRCS
    qmlplugin.cpp
    ArchiveBookModel.cpp
    ArchiveImageProvider.cpp
    BookModel.cpp
    BookListModel.cpp
    FolderBookModel.cpp
    PreviewFetcher.cpp
)

add_library (peruseqmlplugin SHARED ${qmlplugin_SRCS})

target_link_libraries (peruseqmlplugin
    Qt5::Core
    Qt5::Qml
    Qt5::Quick
    KF5::Archive
    KF5::FileMetaData
    KF5::KIOWidgets
)

install (TARGETS peruseqmlplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/peruse)
install (FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/peruse)
