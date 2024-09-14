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
    TextDocumentEditor.cpp
    TextViewerItem.cpp
)

set(karchive_rar_SRCS
    karchive-rar/KRar.cpp
    karchive-rar/KRarFileEntry.cpp
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

add_library (peruseqmlplugin SHARED ${qmlplugin_SRCS} ${karchive_rar_SRCS} $<TARGET_OBJECTS:karchive-c-unarr>)
if(USE_PERUSE_PDFTHUMBNAILER)
target_compile_definitions(peruseqmlplugin
    PRIVATE
    -DUSE_PERUSE_PDFTHUMBNAILER
)
endif()
target_include_directories(peruseqmlplugin
    PRIVATE
    karchive-rar
    karchive-rar/unarr
    acbf
)
target_link_libraries (peruseqmlplugin
    PUBLIC
    acbf
    Qt::Core
    Qt::Qml
    Qt::Quick
    Qt::Sql
    KF${QT_MAJOR_VERSION}::Archive
    KF${QT_MAJOR_VERSION}::IconThemes
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::ConfigCore
    KF${QT_MAJOR_VERSION}::GuiAddons
    KF${QT_MAJOR_VERSION}::KIOCore
    KF${QT_MAJOR_VERSION}::KIOWidgets
    KF${QT_MAJOR_VERSION}::FileMetaData
    KF${QT_MAJOR_VERSION}::NewStuffCore
)

if (Qt_VERSION VERSION_GREATER_EQUAL 6.7.0)
    target_link_libraries(peruseqmlplugin PUBLIC Qt::QuickPrivate)
endif()

if (ZLIB_FOUND)
    target_include_directories(peruseqmlplugin PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(peruseqmlplugin PRIVATE ${ZLIB_LIBRARIES})
    add_definitions(-DHAVE_ZLIB)
endif(ZLIB_FOUND)

install (TARGETS peruseqmlplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/peruse)
install (FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/peruse)
install (FILES peruse.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})
