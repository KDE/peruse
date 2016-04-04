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
    onControlsVisibleChanged: {
        if(controlsVisible === false) {
            visibility = Window.FullScreen;
        }
        else {
            visibility = Window.AutomaticVisibility;
        }
    }

    function showBook(filename, currentPage) {
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

    contextDrawer: PeruseContextDrawer {
        id: contextDrawer;
    }

    globalDrawer: Kirigami.GlobalDrawer {
        /// FIXME This causes the text to get cut off on the phone, however if the text is shorter
        /// it fails to expand the sidebar sufficiently to see all the action labels fully. Revisit
        /// this when switching to Kirigami
        title: "Peruse Comic Viewer";
        titleIcon: "peruse";
        actions: [
            Kirigami.Action {
                text: "Recently Added Comics";
                iconName: "appointment-new";
                onTriggered: changeCategory(bookshelfAdded);
            },
            Kirigami.Action {
                text: "Group by Title";
                iconName: "view-media-title";
                onTriggered: changeCategory(bookshelfTitle);
            },
            Kirigami.Action {
                text: "Group by Authors";
                iconName: "actor";
                onTriggered: changeCategory(bookshelfAuthor);
            },
            Kirigami.Action {
                text: "Group by Series";
                iconName: "edit-group";
                onTriggered: changeCategory(bookshelfSeries);
            },
            Kirigami.Action {
                text: "Group by Publisher";
                iconName: "view-media-publisher";
                onTriggered: changeCategory(bookshelfPublisher);
            },
            Kirigami.Action {
                text: "Filter by Folder";
                iconName: "tag-folder";
                onTriggered: changeCategory(bookshelfFolder);
            },
            Kirigami.Action {
            },
            Kirigami.Action {
                text: "Settings"
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
            headerText: "Group by Title";
            onBookSelected: mainWindow.showBook(filename, currentPage);
        }
    }

    Component {
        id: bookshelfAdded;
        Bookshelf {
            model: contentList.newlyAddedCategoryModel;
            headerText: "Recently Added Comics";
            sectionRole: "created";
            sectionCriteria: ViewSection.FullString;
            onBookSelected: mainWindow.showBook(filename, currentPage);
        }
    }

    Component {
        id: bookshelfSeries;
        Bookshelf {
            model: contentList.seriesCategoryModel;
            headerText: "Comics by Series";
            onBookSelected: mainWindow.showBook(filename, currentPage);
        }
    }

    Component {
        id: bookshelfAuthor;
        Bookshelf {
            model: contentList.authorCategoryModel;
            headerText: "Comics by Author";
            onBookSelected: mainWindow.showBook(filename, currentPage);
        }
    }

    Component {
        id: bookshelfPublisher;
        Bookshelf {
            model: contentList;
            headerText: "Comics by Publisher";
            onBookSelected: mainWindow.showBook(filename, currentPage);
        }
    }

    Component {
        id: bookshelfFolder;
        Bookshelf {
            model: contentList.folderCategoryModel;
            headerText: "Comics by Folder";
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
        mainWindow.pageStack.pop(welcomePage);
        mainWindow.pageStack.push(categoryItem);
        globalDrawer.close();
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

