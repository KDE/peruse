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

import QtQuick 2.2
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import org.kde.kirigami 2.1 as Kirigami
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

    Peruse.Config {
        id: peruseConfig;
    }
    function homeDir() {
        return peruseConfig.homeDir();
    }

    function openBook(bookFilename) {
        currentCategory = "";
        peruseConfig.bookOpened(bookFilename);
        mainWindow.pageStack.clear();
        mainWindow.pageStack.push(bookPage, { filename: bookFilename });
    }

    header: Kirigami.ApplicationHeader {}
    globalDrawer: Kirigami.GlobalDrawer {
        /// FIXME This causes the text to get cut off on the phone, however if the text is shorter
        /// it fails to expand the sidebar sufficiently to see all the action labels fully. Revisit
        /// this when switching to Kirigami
        title: i18nc("application title for the sidebar", "Peruse Creator");
        titleIcon: "peruse-creator";
        drawerOpen: true;
        modal: false;
        actions: [
            Kirigami.Action {
                text: i18nc("Switch to the welcome page", "Welcome");
                iconName: "start-over";
                checked: mainWindow.currentCategory === "welcomePage";
                checkable: true;
                onTriggered: {
                    changeCategory(welcomePage);
                }
            },
            Kirigami.Action {
            },
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
            },
            Kirigami.Action {
                text: i18nc("Open the settings page", "Settings");
                iconName: "configure"
                checked: mainWindow.currentCategory === "settingsPage";
                checkable: true;
                onTriggered: changeCategory(settingsPage);
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
        id: bookPage;
        Book {
        }
    }

    Component {
        id: settingsPage;
        Settings {
        }
    }

    property string currentCategory: "welcomePage";
    function changeCategory(categoryItem) {
        // Clear all the way to the welcome page if we change the category...
        mainWindow.pageStack.clear();
        mainWindow.pageStack.push(categoryItem);
        currentCategory = mainWindow.pageStack.currentItem.categoryName;
    }

    function openOther() {
        openDlg.open();
    }

    FileDialog {
        id: openDlg;
        title: i18nc("@title:window standard file open dialog used to open a book not in the collection", "Please Choose a Book to Open");
        folder: mainWindow.homeDir();
        nameFilters: [ "Comic Book Archive zip format (*.cbz)", "All files (*)" ]
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
