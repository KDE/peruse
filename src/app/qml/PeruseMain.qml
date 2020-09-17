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

import org.kde.kirigami 2.7 as Kirigami
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1 as QQC2

import org.kde.peruse 0.1 as Peruse
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
 * There is also the PeruseContextDrawer, which is only accesible on the book
 * page and requires flicking in from the right.
 */
Kirigami.ApplicationWindow {
    id: mainWindow;
    title: i18nc("@title:window the generic descriptive title of the application", "Comic Book Reader");
    property int animationDuration: 200;
    property bool isLoading: false;
    pageStack.initialPage: welcomePage;
    visible: true;
    // If the controls are not visible, being able to drag the pagestack feels really weird,
    // so we just turn that ability off :)
    pageStack.interactive: controlsVisible;

    property bool bookOpen: mainWindow.pageStack.layers.currentItem.objectName === "bookViewer";
    function showBook(filename, currentPage) {
        if(bookOpen) {
            mainWindow.pageStack.layers.pop();
        }
        mainWindow.pageStack.layers.push(bookViewer, { focus: true, file: filename, currentPage: currentPage })
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

    globalDrawer: Kirigami.GlobalDrawer {
        id: globalDrawer
        title: i18nc("application title for the sidebar", "Peruse");
        titleIcon: "peruse";
        drawerOpen: !Kirigami.Settings.isMobile && mainWindow.width > globalDrawer.availableWidth * 3
        modal: Kirigami.Settings.isMobile || mainWindow.width <= globalDrawer.availableWidth * 3
        actions: globalDrawerActions

        // HACK: this is needed because when clicking on the close button, drawerOpen get set to false (instead of the binding)
        // and when !Kirigami.Settings.isMobile && mainWindow.width > globalDrawer.availableWidth * 3 change, the Binding element
        // overwrite the last assigment to false and set drawerOpen to true or false depending on the value of the condition
        Binding {
            target: globalDrawer
            property: "drawerOpen"
            value: !Kirigami.Settings.isMobile && mainWindow.width > globalDrawer.availableWidth * 3
        }

        header: Kirigami.AbstractApplicationHeader {
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Kirigami.Units.smallSpacing * 2
                anchors.rightMargin: Kirigami.Units.smallSpacing * 2

                Kirigami.Heading {
                    level: 2
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
                        text: i18n("Show intro page")
                    }
                }
            }
        }
        ColumnLayout {
            opacity: globalDrawer.drawerOpen && mainWindow.isLoading ? 1 : 0
            visible: opacity > 0
            Behavior on opacity { NumberAnimation { duration: Kirigami.Units.longDuration } }
            Layout.fillWidth: true
            Layout.bottomMargin: Kirigami.Units.largeSpacing
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            QQC2.Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter;
                text: i18nc("shown with a throbber when searching for books on the device", "Please wait while we find your books...");
            }
            QQC2.BusyIndicator {
                Layout.fillWidth: true
                running: mainWindow.isLoading;
            }
            QQC2.Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter;
                text: contentList.count;
            }
        }
    }
    property list<QtObject> globalDrawerActions: [
            Kirigami.Action {
                text: i18nc("Switch to the listing page showing the most recently discovered books", "Recently Added Books");
                iconName: "appointment-new";
                checked: mainWindow.currentCategory === "bookshelfAdded";
                onTriggered: changeCategory(bookshelfAdded);
            },
            Kirigami.Action {
                text: i18nc("Switch to the listing page showing items grouped by title", "Group by Title");
                iconName: "view-media-title";
                checked: mainWindow.currentCategory === "bookshelfTitle";
                onTriggered: changeCategory(bookshelfTitle);
            },
            Kirigami.Action {
                text: i18nc("Switch to the listing page showing items grouped by author", "Group by Author");
                iconName: "actor";
                checked: mainWindow.currentCategory === "bookshelfAuthor";
                onTriggered: changeCategory(bookshelfAuthor);
            },
            Kirigami.Action {
                text: i18nc("Switch to the listing page showing items grouped by series", "Group by Series");
                iconName: "edit-group";
                checked: currentCategory === "bookshelfSeries";
                onTriggered: changeCategory(bookshelfSeries);
            },
            Kirigami.Action {
                text: i18nc("Switch to the listing page showing items grouped by publisher", "Group by Publisher");
                iconName: "view-media-publisher";
                checked: mainWindow.currentCategory === "bookshelfPublisher";
                onTriggered: changeCategory(bookshelfPublisher);
            },
            Kirigami.Action {
                text: i18nc("Switch to the listing page showing items grouped by keywords, characters or genres", "Group by Keywords");
                iconName: "tag";
                checked: mainWindow.currentCategory === "bookshelfKeywords";
                onTriggered: changeCategory(bookshelfKeywords);
            },
            Kirigami.Action {
                text: i18nc("Switch to the listing page showing items grouped by their filesystem folder", "Filter by Folder");
                iconName: "tag-folder";
                checked: mainWindow.currentCategory === "bookshelfFolder";
                onTriggered: changeCategory(bookshelfFolder);
            },
            Kirigami.Action {
                separator: true;
            },
            Kirigami.Action {
                text: i18nc("Open a book from somewhere on disk (uses the open dialog, or a drilldown on touch devices)", "Open Other...");
                iconName: "document-open";
                onTriggered: openOther();
            },
            Kirigami.Action {
                text: i18nc("Switch to the book store page", "Get Hot New Books");
                iconName: "get-hot-new-stuff";
                checked: mainWindow.currentCategory === "storePage";
                onTriggered: changeCategory(storePage);
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
        id: storePage;
        Store {
        }
    }

    Component {
        id: settingsPage;
        Settings {
        }
    }

    Component {
        id: aboutPage
        About {
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
        if (PLASMA_PLATFORM.substring(0, 5) === "phone") {
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

