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
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0

import org.kde.kirigami 1.0 as Kirigami
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

import org.kde.peruse 0.1 as Peruse
import "listcomponents" as ListComponents

Kirigami.Page {
    id: root;
    title: "Welcome";
    signal bookSelected(string filename, int currentPage);

    property list<QtObject> mobileActions;
    property list<QtObject> desktopActions: [
        Kirigami.Action {
            text: "Open selected book";
            shortcut: "Return";
            iconName: "action-close";
            onTriggered: bookSelected(startWithThese.currentItem.filename, startWithThese.currentItem.currentPage);
            enabled: mainWindow.pageStack.currentItem == root;
        },
        Kirigami.Action {
            text: "Previous book";
            shortcut: StandardKey.MoveToPreviousChar
            iconName: "action-previous";
            onTriggered: startWithThese.selectPrevious();
            enabled: mainWindow.pageStack.currentItem == root;
        },
        Kirigami.Action {
            text: "Next book";
            shortcut: StandardKey.MoveToNextChar;
            iconName: "action-next";
            onTriggered: startWithThese.selectNext();
            enabled: mainWindow.pageStack.currentItem == root;
        }
    ]

    actions {
        contextualActions: PLASMA_PLATFORM.substring(0, 5) === "phone" ? mobileActions : desktopActions;
        main: Kirigami.Action {
            text: "Search Books";
            iconName: "system-search";
            onTriggered: searchBox.activate();
        }
    }

    Item {
        width: root.width - (root.leftPadding + root.rightPadding);
        height: root.height - (root.topPadding + root.bottomPadding);
        SearchBox {
            id: searchBox;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            model: contentList.newlyAddedCategoryModel;
            maxHeight: parent.height - titleContainer.height / 2;
        }

        Item {
            id: titleContainer;
            anchors {
                top: searchBox.bottom;
                left: parent.left;
                right: parent.right;
            }
            height: mainWindow.isLoading ? (parent.height / 2) : (appNameLabel.height + appDescriptionLabel.height + units.largeSpacing);
            Behavior on height { PropertyAnimation { duration: mainWindow.animationDuration; easing.type: Easing.InOutQuad; } }
            PlasmaExtras.Heading {
                id: appNameLabel;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.verticalCenter;
                }
                text: "Peruse";
                horizontalAlignment: Text.AlignHCenter;
            }
            PlasmaComponents.Label {
                id: appDescriptionLabel;
                anchors {
                    top: parent.verticalCenter;
                    left: parent.left;
                    right: parent.right;
                }
                text: "Comic Book Reader";
                horizontalAlignment: Text.AlignHCenter;
            }
            Rectangle {
                anchors.centerIn: parent;
                height: 1;
                color: theme.textColor;
                width: appDescriptionLabel.paintedWidth;
            }
        }

        Flickable {
            id: startWithThese;
            property QtObject mostRecentlyRead0: fakeBook;
            property QtObject mostRecentlyRead1: fakeBook;
            property QtObject mostRecentlyRead2: fakeBook;
            property QtObject mostRecentlyRead3: fakeBook;
            property QtObject mostRecentlyRead4: fakeBook;
            property QtObject mostRecentlyRead5: fakeBook;

            property int mostRecentlyAdded0: -1;
            function updateRecentlyRead() {
                mostRecentlyAdded0 = -1;
                mostRecentlyRead0 = mostRecentlyRead1 = mostRecentlyRead2 = mostRecentlyRead3 = mostRecentlyRead4 = mostRecentlyRead5 = fakeBook;
                startWithThese.mostRecentlyRead0 = contentList.bookFromFile(peruseConfig.recentlyOpened[0]);
                startWithThese.mostRecentlyRead1 = contentList.bookFromFile(peruseConfig.recentlyOpened[1]);
                startWithThese.mostRecentlyRead2 = contentList.bookFromFile(peruseConfig.recentlyOpened[2]);
                startWithThese.mostRecentlyRead3 = contentList.bookFromFile(peruseConfig.recentlyOpened[3]);
                startWithThese.mostRecentlyRead4 = contentList.bookFromFile(peruseConfig.recentlyOpened[4]);
                startWithThese.mostRecentlyRead5 = contentList.bookFromFile(peruseConfig.recentlyOpened[5]);
                // the model might be null, if we haven't actually got any entries... so, let's check that
                // and just leave the whole thing empty in that case :)
                if(contentList.newlyAddedCategoryModel) {
                    startWithThese.mostRecentlyAdded0 = 0;
                    newItemsRepeater.model = Math.min(10, Math.floor((contentList.newlyAddedCategoryModel.rowCount() - 5)));
                }
            }
            Connections {
                target: peruseConfig;
                onRecentlyOpenedChanged: startWithThese.updateRecentlyRead();
            }
            Connections {
                target: mainWindow;
                onIsLoadingChanged: {
                    if(mainWindow.isLoading === false) {
                        startWithThese.updateRecentlyRead();
                    }
                }
            }
            anchors {
                top: titleContainer.bottom;
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            opacity: mainWindow.isLoading ? 0 : 1;
            Behavior on opacity { PropertyAnimation { duration: mainWindow.animationDuration; } }
            contentWidth: width;
            contentHeight: recentItemsColumn.height;
            clip: true;

            property Item currentItem;
            property var itemArray: [rread0, rread1, rread2, rread3, rread4, rread5];
            function selectNext() {
                var index = itemArray.indexOf(currentItem);
                if(index < itemArray.length) {
                    var nextItem = itemArray[index + 1];
                    if(nextItem.height > 0) {
                        currentItem = nextItem;
                    }
                }
            }
            function selectPrevious() {
                var index = itemArray.indexOf(currentItem);
                if(index > 0) {
                    currentItem = itemArray[index - 1];
                }
            }
            Peruse.PropertyContainer {
                id: fakeBook;
                property string author: "unnamed";
                property string title: "unnamed";
                property string filename: "";
                property string currentPage: "0";
                property string totalPages: "0";
            }
            Column {
                id: recentItemsColumn;
                width: parent.width;
                height: childrenRect.height;
                ListComponents.Section {
                    text: "Continue reading";
                    width: startWithThese.width;
                    height: rread0.height > 0 ? paintedHeight : 0;
                    visible: height > 0;
                }
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    width: childrenRect.width;
                    height: childrenRect.height;
                    ListComponents.BookTileTall {
                        id: rread0;
                        height: startWithThese.mostRecentlyRead0.readProperty("filename") != "" ? neededHeight : 0;
                        width: startWithThese.width / 2;
                        author: startWithThese.mostRecentlyRead0.readProperty("author");
                        title: startWithThese.mostRecentlyRead0.readProperty("title");
                        filename: startWithThese.mostRecentlyRead0.readProperty("filename");
                        categoryEntriesCount: 0;
                        currentPage: startWithThese.mostRecentlyRead0.readProperty("currentPage");
                        totalPages: startWithThese.mostRecentlyRead0.readProperty("totalPages");
                        onBookSelected: root.bookSelected(filename, currentPage);
                        selected: startWithThese.currentItem === this;
                    }
                    ListComponents.BookTileTall {
                        id: rread1;
                        height: startWithThese.mostRecentlyRead1.readProperty("filename") != "" ? neededHeight : 0;
                        width: startWithThese.width / 2;
                        author: startWithThese.mostRecentlyRead1.readProperty("author");
                        title: startWithThese.mostRecentlyRead1.readProperty("title");
                        filename: startWithThese.mostRecentlyRead1.readProperty("filename");
                        categoryEntriesCount: 0;
                        currentPage: startWithThese.mostRecentlyRead1.readProperty("currentPage");
                        totalPages: startWithThese.mostRecentlyRead1.readProperty("totalPages");
                        onBookSelected: root.bookSelected(filename, currentPage);
                        selected: startWithThese.currentItem === this;
                    }
                }
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    width: childrenRect.width;
                    height: childrenRect.height;
                    ListComponents.BookTileTall {
                        id: rread2;
                        height: startWithThese.mostRecentlyRead2.readProperty("filename") != "" ? neededHeight : 0;
                        width: startWithThese.width / 4;
                        author: startWithThese.mostRecentlyRead2.readProperty("author");
                        title: startWithThese.mostRecentlyRead2.readProperty("title");
                        filename: startWithThese.mostRecentlyRead2.readProperty("filename");
                        categoryEntriesCount: 0;
                        currentPage: startWithThese.mostRecentlyRead2.readProperty("currentPage");
                        totalPages: startWithThese.mostRecentlyRead2.readProperty("totalPages");
                        onBookSelected: root.bookSelected(filename, currentPage);
                        selected: startWithThese.currentItem === this;
                    }
                    ListComponents.BookTileTall {
                        id: rread3;
                        height: startWithThese.mostRecentlyRead3.readProperty("filename") != "" ? neededHeight : 0;
                        width: startWithThese.width / 4;
                        author: startWithThese.mostRecentlyRead3.readProperty("author");
                        title: startWithThese.mostRecentlyRead3.readProperty("title");
                        filename: startWithThese.mostRecentlyRead3.readProperty("filename");
                        categoryEntriesCount: 0;
                        currentPage: startWithThese.mostRecentlyRead3.readProperty("currentPage");
                        totalPages: startWithThese.mostRecentlyRead3.readProperty("totalPages");
                        onBookSelected: root.bookSelected(filename, currentPage);
                        selected: startWithThese.currentItem === this;
                    }
                    ListComponents.BookTileTall {
                        id: rread4;
                        height: startWithThese.mostRecentlyRead4.readProperty("filename") != "" ? neededHeight : 0;
                        width: startWithThese.width / 4;
                        author: startWithThese.mostRecentlyRead4.readProperty("author");
                        title: startWithThese.mostRecentlyRead4.readProperty("title");
                        filename: startWithThese.mostRecentlyRead4.readProperty("filename");
                        categoryEntriesCount: 0;
                        currentPage: startWithThese.mostRecentlyRead4.readProperty("currentPage");
                        totalPages: startWithThese.mostRecentlyRead4.readProperty("totalPages");
                        onBookSelected: root.bookSelected(filename, currentPage);
                        selected: startWithThese.currentItem === this;
                    }
                    ListComponents.BookTileTall {
                        id: rread5;
                        height: startWithThese.mostRecentlyRead5.readProperty("filename") != "" ? neededHeight : 0;
                        width: startWithThese.width / 4;
                        author: startWithThese.mostRecentlyRead5.readProperty("author");
                        title: startWithThese.mostRecentlyRead5.readProperty("title");
                        filename: startWithThese.mostRecentlyRead5.readProperty("filename");
                        categoryEntriesCount: 0;
                        currentPage: startWithThese.mostRecentlyRead5.readProperty("currentPage");
                        totalPages: startWithThese.mostRecentlyRead5.readProperty("totalPages");
                        onBookSelected: root.bookSelected(filename, currentPage);
                        selected: startWithThese.currentItem === this;
                    }
                }
                ListComponents.Section {
                    text: "Recently added";
                    width: startWithThese.width;
                    height: paintedHeight;
                }
                PlasmaComponents.Label {
                    visible: !firstRecentlyAddedBook.visible;
                    height: visible ? paintedHeight : 0;
                    width: startWithThese.width;
                    text: "You have no comics on your device. Please put some into your Documents or Downloads folder (for example by downloading some) and they will show up here!";
                    wrapMode: Text.WordWrap;
                    horizontalAlignment: Text.AlignHCenter;
                }
                Row {
                    height: childrenRect.height;
                    width: childrenRect.width;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    ListComponents.BookTileTall {
                        id: firstRecentlyAddedBook;
                        visible: filename !== "";
                        height: visible ? neededHeight : 0;
                        width: startWithThese.width / 2;
                        property QtObject book: contentList.newlyAddedCategoryModel ? contentList.newlyAddedCategoryModel.get(startWithThese.mostRecentlyAdded0) : fakeBook;
                        author: book.readProperty("author");
                        title: book.readProperty("title");
                        filename: book.readProperty("filename");
                        categoryEntriesCount: 0;
                        currentPage: book.readProperty("currentPage");
                        totalPages: book.readProperty("totalPages");
                        onBookSelected: root.bookSelected(filename, currentPage);
                    }
                    ListComponents.BookTileTall {
                        visible: filename !== "";
                        height: visible ? neededHeight : 0;
                        width: startWithThese.width / 2;
                        property QtObject book: contentList.newlyAddedCategoryModel ? contentList.newlyAddedCategoryModel.get(startWithThese.mostRecentlyAdded0 + 1) : fakeBook;
                        author: book.readProperty("author");
                        title: book.readProperty("title");
                        filename: book.readProperty("filename");
                        categoryEntriesCount: 0;
                        currentPage: book.readProperty("currentPage");
                        totalPages: book.readProperty("totalPages");
                        onBookSelected: root.bookSelected(filename, currentPage);
                    }
                }
                Row {
                    height: childrenRect.height;
                    width: childrenRect.width;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    ListComponents.BookTileTall {
                        visible: title !== "";
                        height: visible ? neededHeight : 0;
                        width: startWithThese.width / 3;
                        property QtObject book: contentList.newlyAddedCategoryModel ? contentList.newlyAddedCategoryModel.get(startWithThese.mostRecentlyAdded0 + 2) : fakeBook;
                        author: book.readProperty("author");
                        title: book.readProperty("title");
                        filename: book.readProperty("filename");
                        categoryEntriesCount: 0;
                        currentPage: book.readProperty("currentPage");
                        totalPages: book.readProperty("totalPages");
                        onBookSelected: root.bookSelected(filename, currentPage);
                    }
                    ListComponents.BookTileTall {
                        visible: filename !== "";
                        height: visible ? neededHeight : 0;
                        width: startWithThese.width / 3;
                        property QtObject book: contentList.newlyAddedCategoryModel ? contentList.newlyAddedCategoryModel.get(startWithThese.mostRecentlyAdded0 + 3) : fakeBook;
                        author: book.readProperty("author");
                        title: book.readProperty("title");
                        filename: book.readProperty("filename");
                        categoryEntriesCount: 0;
                        currentPage: book.readProperty("currentPage");
                        totalPages: book.readProperty("totalPages");
                        onBookSelected: root.bookSelected(filename, currentPage);
                    }
                    ListComponents.BookTileTall {
                        visible: filename !== "";
                        height: visible ? neededHeight : 0;
                        width: startWithThese.width / 3;
                        property QtObject book: contentList.newlyAddedCategoryModel ? contentList.newlyAddedCategoryModel.get(startWithThese.mostRecentlyAdded0 + 4) : fakeBook;
                        author: book.readProperty("author");
                        title: book.readProperty("title");
                        filename: book.readProperty("filename");
                        categoryEntriesCount: 0;
                        currentPage: book.readProperty("currentPage");
                        totalPages: book.readProperty("totalPages");
                        onBookSelected: root.bookSelected(filename, currentPage);
                    }
                }
                Repeater {
                    id: newItemsRepeater;
                    model: 0;
                    Row {
                        width: childrenRect.width;
                        height: childrenRect.height;
                        anchors.horizontalCenter: parent.horizontalCenter;
                        ListComponents.BookTileTall {
                            visible: filename != "";
                            height: visible ? neededHeight : 0;
                            width: startWithThese.width / 4;
                            property QtObject book: contentList.newlyAddedCategoryModel ? contentList.newlyAddedCategoryModel.get((index * 4) + 5) : fakeBook;
                            author: book.readProperty("author");
                            title: book.readProperty("title");
                            filename: book.readProperty("filename");
                            categoryEntriesCount: 0;
                            currentPage: book.readProperty("currentPage");
                            totalPages: book.readProperty("totalPages");
                            onBookSelected: root.bookSelected(filename, currentPage);
                        }
                        ListComponents.BookTileTall {
                            visible: filename != "";
                            height: visible ? neededHeight : 0;
                            width: startWithThese.width / 4;
                            property QtObject book: contentList.newlyAddedCategoryModel ? contentList.newlyAddedCategoryModel.get((index * 4) + 6) : fakeBook;
                            author: book.readProperty("author");
                            title: book.readProperty("title");
                            filename: book.readProperty("filename");
                            categoryEntriesCount: 0;
                            currentPage: book.readProperty("currentPage");
                            totalPages: book.readProperty("totalPages");
                            onBookSelected: root.bookSelected(filename, currentPage);
                        }
                        ListComponents.BookTileTall {
                            visible: filename != "";
                            height: visible ? neededHeight : 0;
                            width: startWithThese.width / 4;
                            property QtObject book: contentList.newlyAddedCategoryModel ? contentList.newlyAddedCategoryModel.get((index * 4) + 7) : fakeBook;
                            author: book.readProperty("author");
                            title: book.readProperty("title");
                            filename: book.readProperty("filename");
                            categoryEntriesCount: 0;
                            currentPage: book.readProperty("currentPage");
                            totalPages: book.readProperty("totalPages");
                            onBookSelected: root.bookSelected(filename, currentPage);
                        }
                        ListComponents.BookTileTall {
                            visible: filename != "";
                            height: visible ? neededHeight : 0;
                            width: startWithThese.width / 4;
                            property QtObject book: contentList.newlyAddedCategoryModel ? contentList.newlyAddedCategoryModel.get((index * 4) + 8) : fakeBook;
                            author: book.readProperty("author");
                            title: book.readProperty("title");
                            filename: book.readProperty("filename");
                            categoryEntriesCount: 0;
                            currentPage: book.readProperty("currentPage");
                            totalPages: book.readProperty("totalPages");
                            onBookSelected: root.bookSelected(filename, currentPage);
                        }
                    }
                }
            }
        }
        Item {
            id: loadingProgress;
            anchors {
                top: parent.verticalCenter;
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            opacity: mainWindow.isLoading ? 1 : 0;
            Behavior on opacity { PropertyAnimation { duration: mainWindow.animationDuration; } }
            PlasmaComponents.Label {
                anchors {
                    bottom: parent.verticalCenter;
                    left: parent.left;
                    right: parent.right;
                }
                horizontalAlignment: Text.AlignHCenter;
                text: "Please wait while we find your comics...";
            }
            PlasmaComponents.BusyIndicator {
                id: loadingSpinner;
                anchors {
                    top: parent.verticalCenter;
                    left: parent.left;
                    right: parent.right;
                }
                running: mainWindow.isLoading;
            }
            PlasmaComponents.Label {
                anchors {
                    top: loadingSpinner.bottom;
                    left: parent.left;
                    right: parent.right;
                }
                horizontalAlignment: Text.AlignHCenter;
                text: contentList.count;
            }
        }
    }
}
