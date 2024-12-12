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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.peruse as Peruse
import org.kde.contentlist 0.1

/**
 * @brief main application window.
 * 
 * This splits the window in two sections:
 * - A section where you can select comics.
 * - A "global drawer" which can be used to switch between categories
 *   and access settings and the open book dialog.
 * 
 * The global drawer controls which is the main component on the left.
 * It initializes on WelcomePage. The category filters are each handled
 * by a BookShelf. The store page by Store and the settings by Settings.
 * 
 * This also controls the bookViewer, which is a Book object where the
 * main reading of comics is done.
 * 
 * There is also the PeruseContextDrawer, which is only accessible on the book
 * page and requires flicking in from the right.
 */
Kirigami.ApplicationWindow {
    id: mainWindow;

    property int animationDuration: 200
    property bool isLoading: true
    property bool bookOpen: mainWindow.pageStack.layers.currentItem.objectName === "bookViewer"

    title: i18nc("@title:window the generic descriptive title of the application", "Comic Book Reader");

    pageStack {
        defaultColumnWidth: Kirigami.Units.gridUnit * 30
        initialPage: welcomePage
        globalToolBar {
            canContainHandles: true
            style: Kirigami.ApplicationHeaderStyle.ToolBar
            showNavigationButtons: applicationWindow().pageStack.currentIndex > 0 ? Kirigami.ApplicationHeaderStyle.ShowBackButton : 0
        }
    }

    function showBook(filename: url, currentPage: int): void {
        if(bookOpen) {
            mainWindow.pageStack.layers.pop();
        }
        mainWindow.pageStack.layers.push(bookViewer, {
            focus: true,
            file: filename,
            currentPage: currentPage,
        });
        peruseConfig.bookOpened(filename);
    }

    Peruse.BookListModel {
        id: contentList;
        contentModel: ContentList {
            autoSearch: false

            onSearchStarted: { mainWindow.isLoading = true; }
            onSearchCompleted: { mainWindow.isLoading = false; }

            ContentQuery {
                type: ContentQuery.Comics
                locations: peruseConfig.bookLocations
            }
        }
        onCacheLoadedChanged: {
            if(!cacheLoaded) {
                return;
            }
            contentList.contentModel.setKnownFiles(contentList.knownBookFiles());
            contentList.contentModel.startSearch()
        }
    }

    Peruse.Config {
        id: peruseConfig;
    }
    function homeDir() {
        return peruseConfig.homeDir();
    }

    contextDrawer: PeruseContextDrawer {
        id: contextDrawer;
    }

    globalDrawer: Kirigami.OverlayDrawer {
        edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
        modal: Kirigami.Settings.isMobile || (applicationWindow().width < Kirigami.Units.gridUnit * 50 && !collapsed) // Only modal when not collapsed, otherwise collapsed won't show.
        z: modal ? Math.round(position * 10000000) : 100
        drawerOpen: !Kirigami.Settings.isMobile && enabled
        width: Kirigami.Units.gridUnit * 16
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
                        text: i18n("Peruse")
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
                    spacing: 1
                    width: scrollView.width
                    PlaceItem {
                        text: i18nc("Switch to the listing page showing the most recently read books", "Home");
                        icon.name: "go-home";
                        checked: true
                        QQC2.ButtonGroup.group: placeGroup
                        onTriggered: {
                            if (changeCategory(welcomePage)) {
                                pageStack.currentItem.updateRecent();
                            }
                        }
                    }
                    PlaceItem {
                        text: i18nc("Switch to the listing page showing the most recently discovered books", "Recently Added Books");
                        icon.name: "appointment-new";
                        QQC2.ButtonGroup.group: placeGroup
                        onTriggered: changeCategory(bookshelfAdded);
                    }
                    PlaceItem {
                        text: i18nc("Open a book from somewhere on disk (uses the open dialog, or a drilldown on touch devices)", "Open Other...");
                        icon.name: "document-open";
                        onClicked: openOther();
                        QQC2.ButtonGroup.group: undefined
                        checkable: false
                    }
                    Kirigami.ListSectionHeader {
                        text: i18nc("Heading for switching to listing page showing items grouped by some properties", "Group By")
                    }
                    PlaceItem {
                        text: i18nc("Switch to the listing page showing items grouped by title", "Title");
                        icon.name: "view-media-title";
                        onTriggered: changeCategory(bookshelfTitle);
                        QQC2.ButtonGroup.group: placeGroup
                    }
                    PlaceItem {
                        text: i18nc("Switch to the listing page showing items grouped by author", "Author");
                        icon.name: "actor";
                        onTriggered: changeCategory(bookshelfAuthor);
                        QQC2.ButtonGroup.group: placeGroup
                    }
                    PlaceItem {
                        text: i18nc("Switch to the listing page showing items grouped by series", "Series");
                        icon.name: "edit-group";
                        onTriggered: changeCategory(bookshelfSeries);
                        QQC2.ButtonGroup.group: placeGroup
                    }
                    PlaceItem {
                        text: i18nc("Switch to the listing page showing items grouped by publisher", "Publisher");
                        icon.name: "view-media-publisher";
                        onTriggered: changeCategory(bookshelfPublisher);
                        QQC2.ButtonGroup.group: placeGroup
                    }
                    PlaceItem {
                        text: i18nc("Switch to the listing page showing items grouped by keywords, characters or genres", "Keywords");
                        icon.name: "tag";
                        onTriggered: changeCategory(bookshelfKeywords);
                        QQC2.ButtonGroup.group: placeGroup
                    }
                    PlaceItem {
                        text: i18nc("Switch to the listing page showing items grouped by their filesystem folder", "Folder");
                        icon.name: "tag-folder";
                        onTriggered: changeCategory(bookshelfFolder);
                        QQC2.ButtonGroup.group: placeGroup
                    }

                }
            }

            Item {
                Layout.fillHeight: true
            }

            QQC2.Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter;
                text: i18nc("shown with a throbber when searching for books on the device", "Please wait while we find your books...")
                visible: mainWindow.isLoading
            }

            QQC2.BusyIndicator {
                Layout.fillWidth: true
                running: mainWindow.isLoading
            }

            PlaceItem {
                text: i18nc("Open the settings page", "Settings");
                icon.name: "configure"
                onTriggered: changeCategory(settingsPage);
                QQC2.ButtonGroup.group: placeGroup
            }

            PlaceItem {
                text: i18nc("Open the about page", "About");
                icon.name: "help-about"
                onTriggered: root.pageStack.pushDialogLayer(Qt.createComponent('org.kde.kirigamiaddons.formcard', "AboutPage"))
                QQC2.ButtonGroup.group: placeGroup
            }
        }
    }

    Component {
        id: welcomePage;
        WelcomePage {
            onBookSelected: mainWindow.showBook(filename, currentPage);
        }
    }

    Component {
        id: bookViewer;
        Book {
            id: viewerRoot;
            onCurrentPageChanged: {
                contentList.setBookData(viewerRoot.file, "currentPage", viewerRoot.currentPage);
            }
            onTotalPagesChanged: {
                contentList.setBookData(viewerRoot.file, "totalPages", viewerRoot.totalPages);
            }
        }
    }

    Component {
        id: bookshelfTitle;
        Bookshelf {
            model: contentList.titleCategoryModel;
            headerText: i18nc("Title of the page with books grouped by the title start letters", "Group by Title");
            onBookSelected: mainWindow.showBook(filename, currentPage);
            categoryName: "bookshelfTitle";
        }
    }

    Component {
        id: bookshelfAdded;
        Bookshelf {
            model: contentList.newlyAddedCategoryModel;
            headerText: i18nc("Title of the page with all books ordered by which was added most recently", "Recently Added Books");
            sectionRole: "created";
            sectionCriteria: ViewSection.FullString;
            onBookSelected: mainWindow.showBook(filename, currentPage);
            categoryName: "bookshelfAdded";
        }
    }

    Component {
        id: bookshelfSeries;
        Bookshelf {
            model: contentList.seriesCategoryModel;
            headerText: i18nc("Title of the page with books grouped by what series they are in", "Group by Series");
            onBookSelected: mainWindow.showBook(filename, currentPage);
            categoryName: "bookshelfSeries";
        }
    }

    Component {
        id: bookshelfAuthor;
        Bookshelf {
            model: contentList.authorCategoryModel;
            headerText: i18nc("Title of the page with books grouped by author", "Group by Author");
            onBookSelected: mainWindow.showBook(filename, currentPage);
            categoryName: "bookshelfAuthor";
        }
    }

    Component {
        id: bookshelfPublisher;
        Bookshelf {
            model: contentList.publisherCategoryModel;
            headerText: i18nc("Title of the page with books grouped by who published them", "Group by Publisher");
            onBookSelected: mainWindow.showBook(filename, currentPage);
            categoryName: "bookshelfPublisher";
        }
    }

    Component {
        id: bookshelfKeywords;
        Bookshelf {
            model: contentList.keywordCategoryModel;
            headerText: i18nc("Title of the page with books grouped by keywords, character or genres", "Group by Keywords, Characters and Genres");
            onBookSelected: mainWindow.showBook(filename, currentPage);
            categoryName: "bookshelfKeywords";
        }
    }

    Component {
        id: bookshelfFolder;
        Bookshelf {
            model: contentList.folderCategoryModel;
            headerText: i18nc("Title of the page with books grouped by what folder they are in", "Filter by Folder");
            onBookSelected: mainWindow.showBook(filename, currentPage);
            categoryName: "bookshelfFolder";
        }
    }

    Component {
        id: bookshelf;
        Bookshelf {
            onBookSelected: mainWindow.showBook(filename, currentPage);
        }
    }

    Component {
        id: settingsPage;
        Settings {
        }
    }

    property string currentCategory: "welcomePage";
    property Component currentCategoryItem: welcomePage;
    function changeCategory(categoryItem) {
        if (categoryItem === mainWindow.currentCategoryItem) {
            return false;
        }
        // Clear all the way to the welcome page if we change the category...
        mainWindow.pageStack.clear();
        mainWindow.pageStack.push(categoryItem);
        currentCategory = mainWindow.pageStack.currentItem.categoryName;
        currentCategoryItem = categoryItem;
        if (Kirigami.Settings.isMobile) {
            globalDrawer.close();
        }
        return true;
    }


    Component.onCompleted: {
        if (fileToOpen !== "") {
            mainWindow.showBook(fileToOpen, 0);
        }
    }
}

