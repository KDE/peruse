# SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clauses

ecm_add_qml_module(contentlistqmlplugin
    URI "org.kde.peruse.contentlist"
    GENERATE_PLUGIN_SOURCE
)

target_sources(contentlistqmlplugin PRIVATE
    ContentList.cpp
    ContentList.h
    ContentListerBase.cpp
    ContentListerBase.h
    FilesystemContentLister.cpp
    FilesystemContentLister.h
    ContentQuery.cpp
    ContentQuery.h
)

target_link_libraries(contentlistqmlplugin PRIVATE
    Qt::Gui
    Qt::Qml
    KF6::FileMetaData
)

if (TARGET KF::Baloo)
    target_sources(contentlistqmlplugin PRIVATE
        BalooContentLister.cpp
        BalooContentLister.h
    )
    target_compile_definitions(contentlistqmlplugin PRIVATE -DBALOO_FOUND=1)
    target_link_libraries(contentlistqmlplugin PRIVATE KF6::Baloo)
endif()

ecm_finalize_qml_module(contentlistqmlplugin DESTINATION ${KDE_INSTALL_QMLDIR})
