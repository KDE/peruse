add_subdirectory(karchive-rar)

set(qmlplugin_SRCS
    qmlplugin.cpp
    ArchiveBookModel.cpp
    ArchiveImageProvider.cpp
    CategoryEntriesModel.cpp
    BookModel.cpp
    BookListModel.cpp
    FilterProxy.cpp
    FolderBookModel.cpp
    PeruseConfig.cpp
    PreviewImageProvider.cpp
    PropertyContainer.cpp
)

add_library (peruseqmlplugin SHARED ${qmlplugin_SRCS})
target_include_directories(peruseqmlplugin
    PRIVATE
    karchive-rar
)
target_link_libraries (peruseqmlplugin
    karchive-rar
    Qt5::Core
    Qt5::Qml
    Qt5::Quick
    KF5::Archive
    KF5::IconThemes
    KF5::ConfigCore
    KF5::KIOWidgets
    KF5::FileMetaData
)

install (TARGETS peruseqmlplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/peruse)
install (FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/peruse)
