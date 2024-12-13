# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(peruse main.cpp)

ecm_add_qml_module(peruse
    URI "org.kde.peruse.app"
    GENERATE_PLUGIN_SOURCE
    DEPENDENCIES
        "org.kde.peruse.thumbnail"
        "org.kde.kirigami"
        "org.kde.kirigamiaddons.formcard"
        "org.kde.kirigamiaddons.delegates"
    QML_FILES
        qml/Book.qml
        qml/BookDetails.qml
        qml/BookInfoPage.qml
        qml/Bookshelf.qml
        qml/FileFinder.qml
        qml/Main.qml
        qml/MobileMain.qml
        qml/PeruseContextDrawer.qml
        qml/PeruseMain.qml
        qml/Settings.qml
        qml/Store.qml
        qml/WelcomePage.qml
)

ecm_target_qml_sources(peruse
    PRIVATE
    PATH qml/listcomponents
    SOURCES
        qml/listcomponents/BookTile.qml
        qml/listcomponents/BookTileTall.qml
        qml/listcomponents/CategoryTile.qml
        qml/listcomponents/CategoryTileTall.qml
        qml/listcomponents/ListPageHeader.qml
        qml/listcomponents/Section.qml
)

ecm_target_qml_sources(peruse
    PRIVATE
    PATH qml/viewers
    SOURCES
        qml/viewers/ImageBrowser.qml
        qml/viewers/ViewerBase.qml
        qml/viewers/cbr.qml
        qml/viewers/folderofimages.qml
        qml/viewers/okular.qml
)

ecm_target_qml_sources(peruse
    PRIVATE
    PATH qml/viewers/helpers
    SOURCES
        qml/viewers/helpers/HolyRectangle.qml
        qml/viewers/helpers/JumpHandler.qml
        qml/viewers/helpers/Navigator.qml
        qml/viewers/helpers/TextAreaHandler.qml
)

ecm_add_app_icon(peruse ICONS "icon/128-apps-peruse.png")

ecm_qt_declare_logging_category(peruse
    HEADER app_debug.h
    IDENTIFIER APP_LOG
    CATEGORY_NAME org.kde.peruse.app
    DEFAULT_SEVERITY Warning
)

target_link_libraries(peruse PRIVATE
    Qt::Core
    Qt::Quick
    Qt::Gui
    ${QtGui_OPENGL_LIBRARIES}
    Qt::Qml
    Qt::Widgets
    Qt::OpenGL
    KF${QT_MAJOR_VERSION}::I18n
    peruse_helpers
)

target_include_directories(peruse PRIVATE "${CMAKE_BINARY_DIR}")

install(TARGETS peruse ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.peruse.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.peruse.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

add_subdirectory(icon)
