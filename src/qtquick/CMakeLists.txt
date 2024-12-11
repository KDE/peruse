# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clause

add_subdirectory(karchive-rar)

add_library(qtquick_internal STATIC)

target_sources(qtquick_internal PUBLIC
    karchive-rar/KRar.cpp
    karchive-rar/KRarFileEntry.cpp
    $<TARGET_OBJECTS:karchive-c-unarr>
)


ecm_qt_declare_logging_category(qtquick_internal
    HEADER qtquick_debug.h
    IDENTIFIER QTQUICK_LOG
    CATEGORY_NAME org.kde.peruse.qml
    DEFAULT_SEVERITY Warning
)

target_include_directories(qtquick_internal PUBLIC
    karchive-rar
    karchive-rar/unarr
)

target_link_libraries(qtquick_internal PUBLIC
    acbf
    KF6::Archive
    KF6::IconThemes
    KF6::KIOCore
    KF6::KIOWidgets
)

#### Next Target: perusethumbnail

ecm_add_qml_module(perusethumbnail
    URI "org.kde.peruse.thumbnail"
)

target_link_libraries(perusethumbnail PUBLIC
    acbf
    qtquick_internal
    Qt::Quick
    KF6::CoreAddons
    KF6::GuiAddons
    KF6::Archive
)

target_sources(perusethumbnail PRIVATE
    qmlplugin.cpp
    qmlplugin.h
    PreviewImageProvider.cpp
    ComicCoverImageProvider.cpp
)

if(USE_PERUSE_PDFTHUMBNAILER)
    target_sources(perusethumbnail PRIVATE
        PDFCoverImageProvider.cpp
    )

    target_compile_definitions(peruseqmlplugin PRIVATE
        -DUSE_PERUSE_PDFTHUMBNAILER
    )
endif()

ecm_finalize_qml_module(perusethumbnail DESTINATION ${KDE_INSTALL_QMLDIR})

#### Next Target: peruseqmlplugin

ecm_add_qml_module(peruseqmlplugin
    URI "org.kde.peruse"
    GENERATE_PLUGIN_SOURCE
    IMPORTS
        "org.kde.peruse.thumbnail"
)

target_sources(peruseqmlplugin PRIVATE
    ArchiveBookModel.cpp
    ArchiveImageProvider.cpp
    BookDatabase.cpp
    BookModel.cpp
    BookListModel.cpp
    CategoryEntriesModel.cpp
    FilterProxy.cpp
    FolderBookModel.cpp
    PeruseConfig.cpp
    PropertyContainer.cpp
    TextDocumentEditor.cpp
    TextViewerItem.cpp

    types.cpp
)

target_include_directories(peruseqmlplugin PRIVATE acbf)

target_link_libraries (peruseqmlplugin PUBLIC
    acbf
    qtquick_internal
    Qt::Core
    Qt::Qml
    Qt::Quick
    Qt::Sql
    KF6::Archive
    KF6::I18n
    KF6::ConfigCore
    KF6::GuiAddons
    KF6::FileMetaData
    KF6::NewStuffCore
)

if (Qt_VERSION VERSION_GREATER_EQUAL 6.7.0)
    target_link_libraries(peruseqmlplugin PUBLIC Qt::QuickPrivate)
endif()

if (ZLIB_FOUND)
    target_include_directories(peruseqmlplugin PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(peruseqmlplugin PRIVATE ${ZLIB_LIBRARIES})
    add_definitions(-DHAVE_ZLIB)
endif(ZLIB_FOUND)

ecm_finalize_qml_module(peruseqmlplugin DESTINATION ${KDE_INSTALL_QMLDIR})

install(FILES peruse.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})
