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

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.peruse 0.1 as Peruse
import org.kde.contentlist 0.1

MobileMainWindow {
    id: mainWindow;
    property int animationDuration: 200;
    property bool isLoading: true;
    width: 750
    height: 1100

    SystemPalette { id: sysPal; }

    Peruse.BookListModel {
        id: contentList;
        contentModel: ContentList {
            onSearchCompleted: {
                mainWindow.isLoading = false;
            }
        }
    }

    toolBar: ToolBar {
        RowLayout {
            PlasmaComponents.ToolButton {
                iconName: "format-justify-fill"
                onClicked: mainWindow.toggleSidebar();
            }
            PlasmaComponents.ToolButton {
                iconName: "draw-arrow-back"
                text: "Back"
                enabled: view.depth > 1

                onClicked: goUp();
            }
        }
    }

    sidebar: ColumnLayout {
        spacing: 0
        PlasmaExtras.Heading {
            text: "Navigation"
            font.bold: true
            level: 2
        }
        PlasmaComponents.ToolButton {
            text: "All Comics"
            iconName: "system-search"
            Layout.fillWidth: true
            onClicked: {
//                 view.clear()
                view.push({
                    item: bookshelfTitle,
                    properties: { focus: true }
                })
                mainWindow.toggleSidebar();
            }
        }
        PlasmaComponents.ToolButton {
            text: "Group by Authors"
            iconName: "system-search"
            Layout.fillWidth: true
            onClicked: {
//                 view.clear()
                view.push({
                    item: bookshelfAuthor,
                    properties: { focus: true }
                })
                mainWindow.toggleSidebar();
            }
        }
        PlasmaComponents.ToolButton {
            text: "Group by Publisher"
            iconName: "system-search"
            Layout.fillWidth: true
            onClicked: {
//                 view.clear()
                view.push({
                    item: bookshelfPublisher,
                    properties: { focus: true }
                })
                mainWindow.toggleSidebar();
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
    mainItem: StackView {
        id: view
        focus: true
        initialItem: welcomePage;

        delegate: StackViewDelegate {
            pushTransition: StackViewTransition {
                PropertyAnimation {
                    duration: mainWindow.animationDuration;
                    target: enterItem
                    property: "x"
                    from: exitItem.width
                    to: 0
                }
            }
            popTransition: StackViewTransition {
                PropertyAnimation {
                    duration: mainWindow.animationDuration;
                    target: exitItem
                    property: "x"
                    from: 0
                    to: enterItem.width
                }
            }
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
                width: view.width;
                model: contentList;
                onBookSelected: {
                    view.push({
                        item: bookViewer,
                        properties: { focus: true, file: filename, currentPage: currentPage, contentList: contentList }
                    })
                }
                headerText: "All Comics";
                group: Peruse.BookListModel.GroupByTitle;
            }
        }

        Component {
            id: bookshelfAuthor;
            Bookshelf {
                width: view.width;
                model: contentList;
                onBookSelected: {
                    view.push({
                        item: bookViewer,
                        properties: { focus: true, file: filename, currentPage: currentPage, contentList: contentList }
                    })
                }
                headerText: "Comics by Author";
                group: Peruse.BookListModel.GroupByAuthor;
            }
        }

        Component {
            id: bookshelfPublisher;
            Bookshelf {
                width: view.width;
                model: contentList;
                onBookSelected: {
                    view.push({
                        item: bookViewer,
                        properties: { focus: true, file: filename, currentPage: currentPage, contentList: contentList }
                    })
                }
                headerText: "Comics by Publisher";
                group: Peruse.BookListModel.GroupByPublisher;
            }
        }
    }

    function goUp() {
        view.pop()
        view.currentItem.focus = true
    }

    Component.onCompleted: {
        for(var i = 0; i < bookLocations.length; ++i) {
            contentList.contentModel.addLocation(bookLocations[i]);
        }
        contentList.contentModel.setSearchString("cbz OR cbr OR cb7 OR cbt OR cba OR chm OR djvu OR epub OR pdf");
        contentList.contentModel.startSearch();
    }
}
