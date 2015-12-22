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

import org.kde.plasma.mobilecomponents 0.2 as MobileComponents
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.peruse 0.1 as Peruse
import org.kde.contentlist 0.1

MobileComponents.ApplicationWindow {
    id: mainWindow;
    property int animationDuration: 200;
    property bool isLoading: true;
    initialPage: welcomePage;
    width: 750;
    height: 1100;
    visible: true;

    Peruse.BookListModel {
        id: contentList;
        contentModel: ContentList {
            onSearchCompleted: {
                mainWindow.isLoading = false;
            }
        }
    }
    contextDrawer: MobileComponents.ContextDrawer {
        id: contextDrawer;
    }

    globalDrawer: MobileComponents.GlobalDrawer {
        title: "Navigation";
        titleIcon: "system-search";
        actions: [
            Action {
                text: "All Comics";
                iconName: "system-search";
                onTriggered: changeCategory(bookshelfTitle);
            },
            Action {
                text: "Group by Authors";
                iconName: "system-search";
                onTriggered: changeCategory(bookshelfAuthor);
            },
            Action {
                text: "Group by Series";
                iconName: "system-search";
                onTriggered: changeCategory(bookshelfSeries);
            },
            Action {
                text: "Group by Publisher";
                iconName: "system-search";
                onTriggered: changeCategory(bookshelfPublisher);
            },
            Action {
                text: "Group by Folder";
                iconName: "system-search";
                onTriggered: changeCategory(bookshelfFolder);
            }
        ]
    }

    Component {
        id: welcomePage;
        WelcomePage { }
    }

    Component {
        id: bookViewer;
        Book { }
    }

    Component {
        id: bookshelfTitle;
        Bookshelf {
            model: contentList;
            headerText: "All Comics";
            onBookSelected: mainWindow.pageStack.push(bookViewer, { focus: true, file: filename, currentPage: currentPage });
        }
    }

    Component {
        id: bookshelfSeries;
        Categoryshelf {
            model: contentList.seriesCategoryModel;
            headerText: "Comics by Series";
            onBookSelected: mainWindow.pageStack.push(bookViewer, { focus: true, file: filename, currentPage: currentPage });
        }
    }

    Component {
        id: bookshelfAuthor;
        Categoryshelf {
            model: contentList.authorCategoryModel;
            headerText: "Comics by Author";
            onBookSelected: mainWindow.pageStack.push(bookViewer, { focus: true, file: filename, currentPage: currentPage });
        }
    }

    Component {
        id: bookshelfPublisher;
        Bookshelf {
            model: contentList;
            headerText: "Comics by Publisher";
            onBookSelected: mainWindow.pageStack.push(bookViewer, { focus: true, file: filename, currentPage: currentPage });
        }
    }

    Component {
        id: bookshelfFolder;
        Categoryshelf {
            model: contentList.folderCategoryModel;
            headerText: "Comics by Folder";
            onBookSelected: mainWindow.pageStack.push(bookViewer, { focus: true, file: filename, currentPage: currentPage });
        }
    }

    Component {
        id: bookshelf;
        Bookshelf {
            onBookSelected: mainWindow.pageStack.push(bookViewer, { focus: true, file: filename, currentPage: currentPage });
        }
    }

    function changeCategory(categoryItem) {
        // Clear all the way to the welcome page if we change the category...
        mainWindow.pageStack.pop(welcomePage);
        mainWindow.pageStack.push(categoryItem);
        globalDrawer.close();
    }

    Component.onCompleted: {
        for(var i = 0; i < bookLocations.length; ++i) {
            contentList.contentModel.addLocation(bookLocations[i]);
        }
        contentList.contentModel.setSearchString("cbz OR cbr OR cb7 OR cbt OR cba OR chm OR djvu OR epub OR pdf");
        contentList.contentModel.addMimetype("application/pdf");
        contentList.contentModel.addMimetype("application/x-cbz");
        contentList.contentModel.startSearch();
    }
}
