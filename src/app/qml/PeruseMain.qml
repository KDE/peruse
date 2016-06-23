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
import QtQuick.Controls 1.0
import QtQuick.Window 2.2

import org.kde.kirigami 1.0 as Kirigami
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.peruse 0.1 as Peruse
import org.kde.contentlist 0.1

Kirigami.ApplicationWindow {
    id: mainWindow;
    property int animationDuration: 200;
    property bool isLoading: true;
    pageStack.initialPage: welcomePage;
    visible: true;
    /// Which type of device we're running on. 0 is desktop, 1 is phone
    property int deviceType: PLASMA_PLATFORM.substring(0, 5) === "phone" ? 1 : 0;
    property int deviceTypeDesktop: 0;
    property int deviceTypePhone: 1;

    function showBook(filename, currentPage) {
        if(mainWindow.pageStack.lastItem.objectName === "bookViewer") {
            mainWindow.pageStack.pop();
        }
        mainWindow.pageStack.push(bookViewer, { focus: true, file: filename, currentPage: currentPage })
        peruseConfig.bookOpened(filename);
    }

    Peruse.BookListModel {
        id: contentList;
        contentModel: ContentList {
            onSearchCompleted: {
                mainWindow.isLoading = false;
            }
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
        /// FIXME This causes the text to get cut off on the phone, however if the text is shorter
        /// it fails to expand the sidebar sufficiently to see all the action labels fully. Revisit
        /// this when switching to Kirigami
        title: i18nc("application title for the sidebar", "Peruse Comic Viewer");
        titleIcon: "peruse";
        opened: PLASMA_PLATFORM.substring(0, 5) === "phone" ? false : true;
        modal: PLASMA_PLATFORM.substring(0, 5) === "phone" ? true : false;
        actions: [
            Kirigami.Action {
                text: "Welcome";
                iconName: "start-over";
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
                onTriggered: changeCategory(bookshelfAdded);
            },
            Kirigami.Action {
                text: i18nc("Switch to the listing page showing items grouped by title", "Group by Title");
                iconName: "view-media-title";
                onTriggered: changeCategory(bookshelfTitle);
            },
            Kirigami.Action {
                text: i18nc("Switch to the listing page showing items grouped by author", "Group by Author");
                iconName: "actor";
                onTriggered: changeCategory(bookshelfAuthor);
            },
            Kirigami.Action {
                text: i18nc("Switch to the listing page showing items grouped by series", "Group by Series");
                iconName: "edit-group";
                onTriggered: changeCategory(bookshelfSeries);
            },
            Kirigami.Action {
                text: i18nc("Switch to the listing page showing items grouped by publisher", "Group by Publisher");
                iconName: "view-media-publisher";
                onTriggered: changeCategory(bookshelfPublisher);
            },
            Kirigami.Action {
                text: i18nc("Switch to the listing page showing items grouped by their filesystem folder", "Filter by Folder");
                iconName: "tag-folder";
                onTriggered: changeCategory(bookshelfFolder);
            },
            Kirigami.Action {
            },
            Kirigami.Action {
                text: i18nc("Open a book from somewhere on disk (uses the open dialog, or a drilldown on touch devices)", "Open other...");
                iconName: "document-open";
                onTriggered: openOther();
            },
            Kirigami.Action {
            },
            Kirigami.Action {
                text: i18nc("Open the settings page", "Settings");
                iconName: "configure"
                onTriggered: mainWindow.pageStack.push(settingsPage);
            }
        ]
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
        }
    }

    Component {
        id: bookshelfSeries;
        Bookshelf {
            model: contentList.seriesCategoryModel;
            headerText: i18nc("Title of the page with books grouped by what series they are in", "Group by Series");
            onBookSelected: mainWindow.showBook(filename, currentPage);
        }
    }

    Component {
        id: bookshelfAuthor;
        Bookshelf {
            model: contentList.authorCategoryModel;
            headerText: i18nc("Title of the page with books grouped by author", "Group by Author");
            onBookSelected: mainWindow.showBook(filename, currentPage);
        }
    }

    Component {
        id: bookshelfPublisher;
        Bookshelf {
            model: contentList;
            headerText: i18nc("Title of the page with books grouped by who published them", "Group by Publisher");
            onBookSelected: mainWindow.showBook(filename, currentPage);
        }
    }

    Component {
        id: bookshelfFolder;
        Bookshelf {
            model: contentList.folderCategoryModel;
            headerText: i18nc("Title of the page with books grouped by what folder they are in", "Filter by Folder");
            onBookSelected: mainWindow.showBook(filename, currentPage);
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

    function changeCategory(categoryItem) {
        // Clear all the way to the welcome page if we change the category...
        mainWindow.pageStack.clear();
        mainWindow.pageStack.push(categoryItem);
        if (PLASMA_PLATFORM.substring(0, 5) === "phone") {
            globalDrawer.close();
        }
    }

    Component.onCompleted: {
        var bookLocations = peruseConfig.bookLocations;
        for(var i = 0; i < bookLocations.length; ++i) {
            contentList.contentModel.addLocation(bookLocations[i]);
        }
        contentList.contentModel.setSearchString("cbz OR cbr OR cb7 OR cbt OR cba OR chm OR djvu OR epub OR pdf");
        contentList.contentModel.addMimetype("application/x-cbz");
        contentList.contentModel.addMimetype("application/x-cbr");
        contentList.contentModel.addMimetype("application/x-cb7");
        contentList.contentModel.addMimetype("application/x-cbt");
        contentList.contentModel.addMimetype("application/x-cba");
        contentList.contentModel.addMimetype("application/vnd.ms-htmlhelp");
        contentList.contentModel.addMimetype("image/vnd.djvu");
        contentList.contentModel.addMimetype("image/x-djvu");
        contentList.contentModel.addMimetype("application/epub+zip");
        contentList.contentModel.addMimetype("application/pdf");
        contentList.contentModel.startSearch();
    }
}

