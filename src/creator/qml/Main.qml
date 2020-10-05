/*
 * Copyright (C) 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12 as QQC2
import QtQuick.Window 2.12
import QtQuick.Dialogs 1.3

import org.kde.kirigami 2.7 as Kirigami

import org.kde.peruse 0.1 as Peruse
/**
 * @brief Main window of the application.
 * 
 * This splits the window in two sections:
 * - A section where you can modify a comic.
 * - A "global drawer" which can be used to select actions.
 * 
 * WelcomePage is the opening page and holds options to create a new comic.
 * CreateNewBook gives options for creating a new book from scratch.
 * Book is the actual book being modified.
 * Settings is the page with toggles and knobs for Peruse Creator config.
 */
Kirigami.ApplicationWindow {
    id: mainWindow;
    property int animationDuration: 200;
    width: Screen.desktopAvailableWidth * 0.6;
    height: Screen.desktopAvailableHeight * 0.7;
    pageStack.initialPage: welcomePage;
    pageStack.defaultColumnWidth: pageStack.width

    Peruse.Config {
        id: peruseConfig;
    }
    function homeDir() {
        return peruseConfig.homeDir();
    }

    Peruse.ArchiveBookModel {
        id: bookModel;
        qmlEngine: globalQmlEngine;
        readWrite: true;
    }

    function openBook(bookFilename) {
        bookModel.filename = bookFilename;
        peruseConfig.bookOpened(bookFilename);
        mainWindow.changeCategory(bookPage);
        mainWindow.pageStack.currentItem.model = bookModel;
    }

    globalDrawer: Kirigami.GlobalDrawer {
        /// FIXME This causes the text to get cut off on the phone, however if the text is shorter
        /// it fails to expand the sidebar sufficiently to see all the action labels fully. Revisit
        /// this when switching to Kirigami
        title: i18nc("application title for the sidebar", "Peruse Creator");
        titleIcon: "peruse-creator";
        drawerOpen: true;
        modal: false;
        header: Kirigami.AbstractApplicationHeader {
            topPadding: Kirigami.Units.smallSpacing / 2;
            bottomPadding: Kirigami.Units.smallSpacing / 2;
            leftPadding: Kirigami.Units.smallSpacing
            rightPadding: Kirigami.Units.smallSpacing
            RowLayout {
                anchors.fill: parent

                Kirigami.Heading {
                    level: 1
                    text: i18n("Navigation")
                    Layout.fillWidth: true
                }

                QQC2.ToolButton {
                    icon.name: "go-home"

                    enabled: mainWindow.currentCategory !== "welcomePage";
                    onClicked: {
                        if (changeCategory(welcomePage)) {
                            pageStack.currentItem.updateRecent();
                        }
                    }

                    QQC2.ToolTip {
                        text: i18n("Switch to the welcome page")
                    }
                }
            }
        }
        actions: [
            Kirigami.Action {
                text: i18nc("Create a book", "Create a New Book...");
                iconName: "document-new";
                onTriggered: changeCategory(createNewBookPage);
            },
            Kirigami.Action {
                text: i18nc("Open a book from somewhere on disk (uses the open dialog, or a drilldown on touch devices)", "Open Other...");
                iconName: "document-open";
                onTriggered: openOther();
            },

            Kirigami.Action {
                id: bookActions;
                visible: bookModel.filename !== "";
                separator: true;
            },
            Kirigami.Action {
                visible: bookActions.visible;
                checked: mainWindow.currentCategory === "bookBasics";
                text: bookModel.hasUnsavedChanges ? i18nc("The book's title when there are unsaved changes", "%1 (unsaved)", bookModel.title) : bookModel.title;
                icon.source: bookModel.filename === "" ? "" : "image://comiccover/" + bookModel.filename;
                onTriggered: changeCategory(bookBasicsPage, {model: bookModel});
            },
            Kirigami.Action {
                visible: bookActions.visible;
                checked: mainWindow.currentCategory === "book";
                text: i18nc("Switch to the page which displays the pages in the current book", "Pages");
                iconName: "view-pages-overview"
                onTriggered: changeCategory(bookPage, {model: bookModel});
            },
            Kirigami.Action {
                visible: bookActions.visible;
                checked: mainWindow.currentCategory === "bookMetaInfo";
                text: i18nc("Switch to the page where the user can edit the meta information for the entire book", "Edit Metainfo");
                iconName: "document-edit";
                onTriggered: changeCategory(editMetaInfo, {model: bookModel});
            },
            Kirigami.Action {
                visible: bookActions.visible;
                checked: mainWindow.currentCategory === "bookReferences";
                text: i18nc("Switch to the page where the user can edit the references (that is, snippets of information) found in the book", "Edit References");
                iconName: "documentation";
                onTriggered: changeCategory(editReferences, {model: bookModel});
            },

            Kirigami.Action {
                separator: true;
            },
            Kirigami.Action {
                text: i18nc("Open the settings page", "Settings");
                iconName: "configure"
                checked: mainWindow.currentCategory === "settingsPage";
                onTriggered: changeCategory(settingsPage);
            },
            Kirigami.Action {
                text: i18nc("Open the about page", "About");
                iconName: "help-about"
                checked: mainWindow.currentCategory === "aboutPage";
                onTriggered: changeCategory(aboutPage);
            }
        ]
    }

    Component {
        id: welcomePage;
        WelcomePage {
        }
    }

    Component {
        id: createNewBookPage;
        CreateNewBook {
        }
    }

    Component {
        id: bookBasicsPage;
        BookBasics {
            onRequestCategoryChange: {
                if (categoryName === "book") {
                    changeCategory(bookPage, {model: bookModel});
                }
            }
        }
    }

    Component {
        id: bookPage;
        Book {
        }
    }
    Component {
        id: editMetaInfo;
        BookMetainfoPage {
        }
    }

    Component {
        id: editReferences;
        BookReferences {
        }
    }

    Component {
        id: settingsPage;
        Settings {
        }
    }

    Component {
        id: aboutPage;
        About {
        }
    }

    property string currentCategory: "welcomePage";
    function changeCategory(categoryItem, parameters) {
        // Clear all the way to the welcome page if we change the category...
        mainWindow.pageStack.clear();
        if (parameters === undefined) {
            mainWindow.pageStack.push(categoryItem);
        } else {
            mainWindow.pageStack.push(categoryItem, parameters);
        }
        currentCategory = mainWindow.pageStack.currentItem.categoryName;
    }

    function openOther() {
        openDlg.open();
    }

    FileDialog {
        id: openDlg;
        title: i18nc("@title:window standard file open dialog used to open a book not in the collection", "Please Choose a Book to Open");
        folder: mainWindow.homeDir();
        nameFilters: [
            i18nc("The file type filter for comic book archives", "Comic Book Archive zip format %1", "(*.cbz)"),
            i18nc("The file type filter for showing all files", "All files %1", "(*)")
        ]
        property int splitPos: osIsWindows ? 8 : 7;
        onAccepted: {
            if(openDlg.fileUrl.toString().substring(0, 7) === "file://") {
                mainWindow.openBook(openDlg.fileUrl.toString().substring(splitPos), 0);
            }
        }
        onRejected: {
            // Just do nothing, we don't really care...
        }
    }

    Component.onCompleted: {
        if (fileToOpen !== "") {
             mainWindow.openBook(fileToOpen);
        }
    }
}
