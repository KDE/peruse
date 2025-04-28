# SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clauses

ecm_add_app_icon(CREATOR_SRCS ICONS "icon/128-apps-peruse-creator.png")
add_executable(perusecreator main.cpp ${CREATOR_SRCS})

ecm_add_qml_module(perusecreator
    GENERATE_PLUGIN_SOURCE
    URI org.kde.peruse.creator
    QML_FILES
        qml/AddPageArea.qml
        qml/AddPageSheet.qml
        qml/Book.qml
        qml/BookBasics.qml
        qml/BookBinaries.qml
        qml/BookBinaryEditor.qml
        qml/BookReferenceEditor.qml
        qml/BookReferences.qml
        qml/BookStylesheet.qml
        qml/BookMetainfoPage.qml
        qml/BookPage.qml
        qml/CreateNewBook.qml
        qml/EditPageArea.qml
        qml/LinkEditorSheet.qml
        qml/Main.qml
        qml/MessageBoxSheet.qml
        qml/PageMetaInfo.qml
        qml/SettingsPage.qml
        qml/WelcomePage.qml
        qml/metainfoeditors/AuthorEntryEditor.qml
        qml/metainfoeditors/LanguageTextEntryEditor.qml
)

target_link_libraries(perusecreator PRIVATE
    Qt::Core
    Qt::Quick
    Qt::Gui
    ${QtGui_OPENGL_LIBRARIES}
    Qt::Qml
    Qt::Widgets
    Qt::OpenGL
    KF6::I18n
    KF6::I18nQml
    peruse_helpers
)

install(TARGETS perusecreator ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.perusecreator.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.perusecreator.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

add_subdirectory(icon)

