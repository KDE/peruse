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
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
)
target_link_libraries (peruseqmlplugin
    PUBLIC
    acbf
    Qt5::Core
    Qt5::Qml
    Qt5::Quick
    Qt5::Sql
    KF5::Archive
    KF5::IconThemes
    KF5::I18n
    KF5::ConfigCore
    KF5::GuiAddons
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::FileMetaData
    KF5::NewStuffCore
)

if (ZLIB_FOUND)
    target_include_directories(peruseqmlplugin PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(peruseqmlplugin PRIVATE ${ZLIB_LIBRARIES})
    add_definitions(-DHAVE_ZLIB)
endif(ZLIB_FOUND)

install (TARGETS peruseqmlplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/peruse)
install (FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/peruse)
install (FILES peruse.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})
