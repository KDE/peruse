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

import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4 as QtControls
import QtQuick.Window 2.2

import org.kde.kirigami 2.1 as Kirigami
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
    title: "Comic Book Reader";
    property int animationDuration: 200;
    property bool isLoading: true;
    pageStack.initialPage: welcomePage;
    visible: true;
    // If the controls are not visible, being able to drag the pagestack feels really weird,
    // so we just turn that ability off :)
    pageStack.interactive: controlsVisible;
    /// Which type of device we're running on. 0 is desktop, 1 is phone
    property int deviceType: PLASMA_PLATFORM.substring(0, 5) === "phone" ? 1 : 0;
    property int deviceTypeDesktop: 0;
    property int deviceTypePhone: 1;

    function showBook(filename, currentPage) {
        if(mainWindow.pageStack.lastItem.objectName === "bookViewer") {
            mainWindow.pageStack.pop();
        }
        mainWindow.pageStack.layers.push(bookViewer, { focus: true, file: filename, currentPage: currentPage })
        peruseConfig.bookOpened(filename);
    }

    Peruse.BookListModel {
        id: contentList;
        contentModel: ContentList {
            autoSearch: false

            onSearchCompleted: {
                mainWindow.isLoading = false;
                mainWindow.globalDrawer.actions = globalDrawerActions;
            }

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

    header: Kirigami.ApplicationHeader {}

    contextDrawer: PeruseContextDrawer {
        id: contextDrawer;
    }

    globalDrawer: Kirigami.GlobalDrawer {
        title: i18nc("application title for the sidebar", "Peruse");
        titleIcon: "peruse";
        drawerOpen: PLASMA_PLATFORM.substring(0, 5) === "phone" ? false : true;
        modal: PLASMA_PLATFORM.substring(0, 5) === "phone" ? true : false;
        actions: []
    }
    property list<QtObject> globalDrawerActions: [
            Kirigami.Action {
                text: i18nc("Switch to the welcome page", "Welcome");
                iconName: "start-over";
                checked: mainWindow.currentCategory === "welcomePage";
                onTriggered: {
                    changeCategory(welcomePage);
                    pageStack.currentItem.updateRecent();
                }
            },
            Kirigami.Action {
            },
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
                text: i18nc("Switch to the listing page showing items grouped by their filesystem folder", "Filter by Folder");
                iconName: "tag-folder";
                checked: mainWindow.currentCategory === "bookshelfFolder";
                onTriggered: changeCategory(bookshelfFolder);
            },
            Kirigami.Action {
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
            },
            Kirigami.Action {
                text: i18nc("Open the settings page", "Settings");
                iconName: "configure"
                checked: mainWindow.currentCategory === "settingsPage";
                onTriggered: changeCategory(settingsPage);
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

    property string currentCategory: "welcomePage";
    function changeCategory(categoryItem) {
        // Clear all the way to the welcome page if we change the category...
        mainWindow.pageStack.clear();
        mainWindow.pageStack.push(categoryItem);
        currentCategory = mainWindow.pageStack.currentItem.categoryName;
        if (PLASMA_PLATFORM.substring(0, 5) === "phone") {
            globalDrawer.close();
        }
    }
}

