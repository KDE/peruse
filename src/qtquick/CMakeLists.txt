add_subdirectory(karchive-rar)

set(qmlplugin_SRCS
    qmlplugin.cpp
    ArchiveBookModel.cpp
    ArchiveImageProvider.cpp
    BookDatabase.cpp
    BookModel.cpp
    BookListModel.cpp
    CategoryEntriesModel.cpp
    ComicCoverImageProvider.cpp
    FilterProxy.cpp
    FolderBookModel.cpp
    PeruseConfig.cpp
    PreviewImageProvider.cpp
    PropertyContainer.cpp
)

if(USE_PERUSE_PDFTHUMBNAILER)
    set(qmlplugin_SRCS
        ${qmlplugin_SRCS}
        PDFCoverImageProvider.cpp
    )
endif()

ecm_qt_declare_logging_category(qmlplugin_SRCS
    HEADER qtquick_debug.h
    IDENTIFIER QTQUICK_LOG
    CATEGORY_NAME org.kde.peruse.qml
    DEFAULT_SEVERITY Warning
)

add_library (peruseqmlplugin SHARED ${qmlplugin_SRCS})
if(USE_PERUSE_PDFTHUMBNAILER)
target_compile_definitions(peruseqmlplugin
    PRIVATE
    -DUSE_PERUSE_PDFTHUMBNAILER
)
endif()
target_include_directories(peruseqmlplugin
    PRIVATE
    karchive-rar
    acbf
)
target_link_libraries (peruseqmlplugin
    acbf
    karchive-rar
    Qt5::Core
    Qt5::Qml
    Qt5::Quick
    Qt5::Sql
    KF5::Archive
    KF5::IconThemes
    KF5::ConfigCore
    KF5::GuiAddons
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::FileMetaData
    KF5::NewStuffCore
)

install (TARGETS peruseqmlplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/peruse)
install (FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/peruse)
install (FILES peruse.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})
