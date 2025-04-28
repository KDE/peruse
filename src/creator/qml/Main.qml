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

import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import QtQuick.Window
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.peruse as Peruse
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
    width: Math.min(Screen.desktopAvailableWidth * 0.6, Kirigami.Units.gridUnit * 80);
    height: Math.min(Screen.desktopAvailableHeight * 0.7, Kirigami.Units.gridUnit * 60);

    pageStack {
        initialPage: welcomePage;
        defaultColumnWidth: pageStack.width
    }

    Peruse.Config {
        id: peruseConfig;
    }
    function homeDir() {
        return peruseConfig.homeDir();
    }

    Peruse.ArchiveBookModel {
        id: bookModel;
        readWrite: true;
    }

    function openBook(bookFilename) {
        bookModel.filename = bookFilename;
        peruseConfig.bookOpened(bookFilename);
        mainWindow.changeCategory(bookPage);
        mainWindow.pageStack.currentItem.model = bookModel;
    }

    contextDrawer: Kirigami.ContextDrawer {}
    globalDrawer: Kirigami.OverlayDrawer {
        edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
        modal: Kirigami.Settings.isMobile || (applicationWindow().width < Kirigami.Units.gridUnit * 50 && !collapsed) // Only modal when not collapsed, otherwise collapsed won't show.
        z: modal ? Math.round(position * 10000000) : 100
        drawerOpen: !Kirigami.Settings.isMobile && enabled
        width: Kirigami.Units.gridUnit * 16
        enabled: true
        Behavior on width {
            NumberAnimation {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
        Kirigami.Theme.colorSet: Kirigami.Theme.Window

        handleClosedIcon.source: modal ? null : "sidebar-expand-left"
        handleOpenIcon.source: modal ? null : "sidebar-collapse-left"
        handleVisible: modal
        onModalChanged: if (!modal) {
            drawerOpen = true;
        }

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        contentItem: ColumnLayout {
            spacing: 0

            QQC2.ToolBar {
                Layout.fillWidth: true
                Layout.preferredHeight: mainWindow.pageStack.globalToolBar.preferredHeight

                leftPadding: Kirigami.Units.smallSpacing
                rightPadding: Kirigami.Units.smallSpacing
                topPadding: Kirigami.Units.smallSpacing
                bottomPadding: Kirigami.Units.smallSpacing

                contentItem: RowLayout {
                    Kirigami.Heading {
                        text: i18n("Peruse Creator")
                        Layout.fillWidth: true
                    }

                    QQC2.ToolButton {
                        icon.name: "go-home"

                        enabled: mainWindow.currentCategory !== "welcomePage";
                        onClicked: if (changeCategory(welcomePage)) {
                            pageStack.currentItem.updateRecent();
                        }

                        QQC2.ToolTip.text: i18n("Show intro page")
                        QQC2.ToolTip.visible: hovered
                        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    }
                }
            }

            QQC2.ButtonGroup {
                id: placeGroup
            }

            QQC2.ScrollView {
                id: scrollView

                Layout.fillHeight: true
                Layout.fillWidth: true

                contentWidth: availableWidth
                topPadding: Kirigami.Units.smallSpacing / 2

                component PlaceItem : Delegates.RoundedItemDelegate {
                    id: item
                    signal triggered;
                    checkable: true
                    Layout.fillWidth: true
                    Keys.onDownPressed: nextItemInFocusChain().forceActiveFocus(Qt.TabFocusReason)
                    Keys.onUpPressed: nextItemInFocusChain(false).forceActiveFocus(Qt.TabFocusReason)
                    Accessible.role: Accessible.MenuItem
                    highlighted: checked || activeFocus
                    onToggled: if (checked) {
                        item.triggered();
                    }
                }

                ColumnLayout {
                    spacing: 0

                    width: scrollView.width

                    PlaceItem {
                        text: i18nc("Create a book", "Create a New Book...");
                        icon.name: "document-new";
                        onClicked: changeCategory(createNewBookPage);
                    }

                    PlaceItem {
                        text: i18nc("Open a book from somewhere on disk (uses the open dialog, or a drilldown on touch devices)", "Open Other...");
                        icon.name: "document-open";
                        onClicked: openOther();
                    }

                    Kirigami.Separator {
                        id: bookActions;
                        visible: bookModel.filename !== "";
                        Layout.fillWidth: true
                        Layout.topMargin: 2
                        Layout.bottomMargin: 2
                    }

                    PlaceItem {
                        visible: bookActions.visible;
                        checked: mainWindow.currentCategory === "bookBasics";
                        text: bookModel.hasUnsavedChanges ? i18nc("The book's title when there are unsaved changes", "%1 (unsaved)", bookModel.title) : bookModel.title;
                        icon.source: bookModel.filename === "" ? "" : "image://comiccover/" + bookModel.filename;
                        onClicked: changeCategory(bookBasicsPage, {model: bookModel});
                    }

                    PlaceItem {
                        visible: bookActions.visible;
                        checked: mainWindow.currentCategory === "book";
                        text: i18nc("Switch to the page which displays the pages in the current book", "Pages");
                        icon.name: "view-pages-overview"
                        onClicked: changeCategory(bookPage, {model: bookModel});
                    }

                    PlaceItem {
                        visible: bookActions.visible;
                        checked: mainWindow.currentCategory === "bookMetaInfo";
                        text: i18nc("Switch to the page where the user can edit the meta information for the entire book", "Metainfo");
                        icon.name: "document-edit";
                        onClicked: changeCategory(editMetaInfo, {model: bookModel});
                    }

                    PlaceItem {
                        visible: bookActions.visible;
                        checked: mainWindow.currentCategory === "bookReferences";
                        text: i18nc("Switch to the page where the user can edit the references (that is, snippets of information) found in the book", "References");
                        icon.name: "documentation";
                        onClicked: changeCategory(editReferences, {model: bookModel});
                    }

                    PlaceItem {
                        visible: bookActions.visible;
                        checked: mainWindow.currentCategory === "bookBinaries";
                        text: i18nc("Switch to the page where the user can work with the bits of binary data found in the book", "Embedded Data");
                        icon.name: "document-multiple";
                        onClicked: changeCategory(editBinaries, {model: bookModel});
                    }

                    PlaceItem {
                        visible: bookActions.visible;
                        checked: mainWindow.currentCategory === "bookStylesheet";
                        text: i18nc("Switch to the page where the user can work with the book's stylesheet", "Stylesheet");
                        icon.name: "edit-paste-style";
                        onClicked: changeCategory(editStylesheet, {model: bookModel});
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
                Layout.topMargin: 2
                Layout.bottomMargin: 2
            }

            PlaceItem {
                text: i18nc("Open the settings page", "Settings");
                icon.name: "configure"
                checked: mainWindow.currentCategory === "settingsPage";
                onClicked: changeCategory(settingsPage);
            }

            PlaceItem {
                text: i18nc("Open the about page", "About");
                icon.name: "help-about"
                checked: mainWindow.currentCategory === "aboutPage";
                onClicked: changeCategory(aboutPage);
                Layout.bottomMargin: 2
            }
        }
    }

    Component {
        id: welcomePage;
        WelcomePage { }
    }

    Component {
        id: createNewBookPage;
        CreateNewBook { }
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
        Book { }
    }
    Component {
        id: editMetaInfo;
        BookMetainfoPage { }
    }
    Component {
        id: editReferences;
        BookReferences { }
    }
    Component {
        id: editBinaries;
        BookBinaries { }
    }
    Component {
        id: editStylesheet;
        BookStylesheet { }
    }

    Component {
        id: settingsPage
        SettingsPage { }
    }

    Component {
        id: aboutPage
        FormCard.AboutPage {}
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
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        nameFilters: [
            i18nc("The file type filter for comic book archives", "Comic Book Archive zip format %1", "(*.cbz)"),
            i18nc("The file type filter for showing all files", "All files %1", "(*)")
        ]
        property int splitPos: Qt.platform.os === "windows" ? 8 : 7;
        onAccepted: {
            if(openDlg.selectFile.toString().substring(0, 7) === "file://") {
                mainWindow.openBook(openDlg.selectFile.toString().substring(splitPos), 0);
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
