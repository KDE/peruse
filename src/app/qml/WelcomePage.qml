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
import QtQuick.Controls 2.12 as QtControls

import org.kde.kirigami 2.7 as Kirigami

import org.kde.peruse 0.1 as Peruse
import "listcomponents" as ListComponents
/**
 * @brief The page that Peruse opens up on.
 * 
 * The WelcomePage shares some resemblance to the
 * BookShelf pages in that it allows the user to select a comic,
 * but where BookShelf pages are really for discovery and searching
 * through categories and subcategories, the WelcomePage is primarily
 * for selecting the recently opened and new books, which the user is most likely
 * to look at when they want to read.
 * 
 * It uses BookTileTall to show the selectable books, SearchBox to search books
 * and Section to indicate a subsection.
 */
Kirigami.Page {
    id: root;
    property string categoryName: "welcomePage";
    title: i18nc("title of the welcome page", "Welcome");
    signal bookSelected(string filename, int currentPage);
    function updateRecent() {
        startWithThese.updateRecentlyRead();
    }

    property bool isCurrentContext: isCurrentPage && !applicationWindow().bookOpen
    property list<QtObject> mobileActions;
    property list<QtObject> desktopActions: [
        Kirigami.Action {
            text: i18n("Open Selected Book");
            shortcut: "Return";
            iconName: "document-open";
            onTriggered: bookSelected(startWithThese.currentItem.filename, startWithThese.currentItem.currentPage);
            enabled: root.isCurrentContext && applicationWindow().deviceType === applicationWindow().deviceTypeDesktop;
        },
        Kirigami.Action {
            text: i18nc("select the previous book entry in the list", "Previous Book");
            shortcut: StandardKey.MoveToPreviousChar;
            iconName: "go-previous";
            onTriggered: startWithThese.selectPrevious();
            enabled: root.isCurrentContext && applicationWindow().deviceType === applicationWindow().deviceTypeDesktop;
        },
        Kirigami.Action {
            text: i18nc("select the next book entry in the list", "Next Book");
            shortcut: StandardKey.MoveToNextChar;
            iconName: "go-next";
            onTriggered: startWithThese.selectNext();
            enabled: root.isCurrentContext && applicationWindow().deviceType === applicationWindow().deviceTypeDesktop;
        }
    ]

    actions {
        contextualActions: PLASMA_PLATFORM.substring(0, 5) === "phone" ? mobileActions : desktopActions;
        main: Kirigami.Action {
            text: i18nc("search in the list of books (not inside the books)", "Search Books");
            iconName: "system-search";
            onTriggered: searchBox.activate();
            enabled: root.isCurrentContext;
        }
    }

    Item {
        width: root.width - (root.leftPadding + root.rightPadding);
        height: root.height - root.topPadding;
        SearchBox {
            id: searchBox;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            model: contentList.newlyAddedCategoryModel;
            maxHeight: parent.height - titleContainer.height / 2;
            onBookSelected: root.bookSelected(filename, currentPage);
        }

        Item {
            id: titleContainer;
            anchors {
                top: searchBox.bottom;
                left: parent.left;
                right: parent.right;
            }
            height: applicationWindow().isLoading ? (parent.height / 3) : (appNameLabel.height + appDescriptionLabel.height + Kirigami.Units.largeSpacing);
            Behavior on height { NumberAnimation { duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
            Kirigami.Heading {
                id: appNameLabel;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.verticalCenter;
                }
                text: "Peruse";
                horizontalAlignment: Text.AlignHCenter;
            }
            QtControls.Label {
                id: appDescriptionLabel;
                anchors {
                    top: parent.verticalCenter;
                    left: parent.left;
                    right: parent.right;
                }
                text: i18nc("application subtitle", "Comic Book Reader");
                horizontalAlignment: Text.AlignHCenter;
            }
            Rectangle {
                anchors.centerIn: parent;
                height: 1;
                color: Kirigami.Theme.textColor;
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
                if(startWithThese.currentItem != null) {
                    startWithThese.currentItem = rread0;
                }
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
                target: applicationWindow();
                onIsLoadingChanged: {
                    if(applicationWindow().isLoading === false) {
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
            visible: opacity > 0
            opacity: applicationWindow().isLoading ? 0 : 1;
            Behavior on opacity { NumberAnimation { duration: applicationWindow().animationDuration; } }
            contentWidth: width;
            contentHeight: recentItemsColumn.height;
            clip: true;

            property Item currentItem: null;
            property var itemArray: [rread0, rread1, rread2, rread3, rread4, rread5];
            function selectNext() {
                var index = itemArray.indexOf(currentItem);
                if(index < itemArray.length) {
                    var nextItem = itemArray[index + 1];
                    if(nextItem !== undefined && nextItem.height > 0) {
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
                property string thumbnail: "";
                property string currentPage: "0";
                property string totalPages: "0";
            }
            Column {
                id: recentItemsColumn;
                width: parent.width;
                height: childrenRect.height;
                ListComponents.Section {
                    text: i18nc("title of list of recently opened books", "Continue Reading");
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
                        thumbnail: startWithThese.mostRecentlyRead0.readProperty("thumbnail");
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
                        thumbnail: startWithThese.mostRecentlyRead1.readProperty("thumbnail");
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
                        thumbnail: startWithThese.mostRecentlyRead2.readProperty("thumbnail");
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
                        thumbnail: startWithThese.mostRecentlyRead3.readProperty("thumbnail");
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
                        thumbnail: startWithThese.mostRecentlyRead4.readProperty("thumbnail");
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
                        thumbnail: startWithThese.mostRecentlyRead5.readProperty("thumbnail");
                        categoryEntriesCount: 0;
                        currentPage: startWithThese.mostRecentlyRead5.readProperty("currentPage");
                        totalPages: startWithThese.mostRecentlyRead5.readProperty("totalPages");
                        onBookSelected: root.bookSelected(filename, currentPage);
                        selected: startWithThese.currentItem === this;
                    }
                }
                ListComponents.Section {
                    text: i18nc("title of list of recently discovered books", "Recently Added");
                    width: startWithThese.width;
                    height: paintedHeight;
                }
                QtControls.Label {
                    visible: !firstRecentlyAddedBook.visible;
                    height: visible ? paintedHeight : 0;
                    width: startWithThese.width;
                    text: i18nc("description text for the recently discovered list, shown when no items exist in the search paths", "You have no comics on your device. Please put some into your Documents or Downloads folder (for example by downloading some) and they will show up here.");
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
                        thumbnail: book.readProperty("thumbnail");
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
                        thumbnail: book.readProperty("thumbnail");
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
                        visible: filename !== "";
                        height: visible ? neededHeight : 0;
                        width: startWithThese.width / 3;
                        property QtObject book: contentList.newlyAddedCategoryModel ? contentList.newlyAddedCategoryModel.get(startWithThese.mostRecentlyAdded0 + 2) : fakeBook;
                        author: book.readProperty("author");
                        title: book.readProperty("title");
                        filename: book.readProperty("filename");
                        thumbnail: book.readProperty("thumbnail");
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
                        thumbnail: book.readProperty("thumbnail");
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
                        thumbnail: book.readProperty("thumbnail");
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
                            thumbnail: book.readProperty("thumbnail");
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
                            thumbnail: book.readProperty("thumbnail");
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
                            thumbnail: book.readProperty("thumbnail");
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
                            thumbnail: book.readProperty("thumbnail");
                            categoryEntriesCount: 0;
                            currentPage: book.readProperty("currentPage");
                            totalPages: book.readProperty("totalPages");
                            onBookSelected: root.bookSelected(filename, currentPage);
                        }
                    }
                }
                Item {
                    width: parent.width;
                    height: Kirigami.Units.iconSizes.large + Kirigami.Units.largeSpacing;
                }
            }
        }
        Item {
            id: loadingProgress;
            anchors {
                top: titleContainer.bottom;
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            visible: opacity > 0
            opacity: applicationWindow().isLoading ? 1 : 0;
            Behavior on opacity { NumberAnimation { duration: applicationWindow().animationDuration; } }
            QtControls.Label {
                anchors {
                    bottom: loadingSpinner.top;
                    left: parent.left;
                    right: parent.right;
                    margins: Kirigami.Units.smallSpacing
                }
                horizontalAlignment: Text.AlignHCenter;
                text: i18nc("shown with a throbber when searching for books on the device", "Please wait while we find your books...");
            }
            QtControls.BusyIndicator {
                id: loadingSpinner;
                anchors {
                    verticalCenter: parent.verticalCenter
                    horizontalCenter: parent.horizontalCenter
                }
                width: Kirigami.Units.iconSizes.huge
                height: width
                running: applicationWindow().isLoading;
            }
            QtControls.Label {
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
