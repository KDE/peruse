/*
 * SPDX-FileCopyrightText: 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick 2.12
import QtQuick.Controls 2.12 as QtControls
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15

import org.kde.kirigami 2.12 as Kirigami

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
 * It uses BookTileTall to show the selectable books, SearchFiedl to search books
 * and Section to indicate a subsection.
 */
Kirigami.Page {
    id: root;

    property string categoryName: "welcomePage";
    property bool isCurrentContext: isCurrentPage && !applicationWindow().bookOpen
    property real heightBook: Kirigami.Units.gridUnit * 8
    property string searchText: ""
    property bool searching: searchText.length > 0

    signal bookSelected(string filename, int currentPage);

    function updateRecent() {
        startWithThese.updateRecentlyRead();
    }

    title: i18nc("title of the welcome page", "Welcome");
    leftPadding: searching ? 0 : Kirigami.Units.gridUnit
    rightPadding: searching ? 0 : Kirigami.Units.gridUnit
    topPadding: searching ? 0 : Kirigami.Units.gridUnit
    bottomPadding: searching ? 0 : Kirigami.Units.gridUnit

    actions.main: Kirigami.Action {
        text: i18nc("search in the list of books (not inside the books)", "Search Books");
        iconName: "system-search";
        enabled: root.isCurrentContext;
        displayComponent: Kirigami.SearchField {
            id: searchField
            focus: true
            placeholderText: i18nc("placeholder text for the search field", "Tap and type to search");
            onTextChanged: {
                searchText = text
                if(text.length > 0) {
                    searchTimer.start();
                } else {
                    searchTimer.stop();
                }
            }
        }
    }

    Timer {
        id: searchTimer;
        interval: 250;
        repeat: false;
        running: false;
        onTriggered: searchFilterProxy.setFilterFixedString(searchText);
    }

    QtControls.ScrollView {
        id: searchList;
        anchors.fill: parent;
        visible: searching
        GridView {
            footer: Item { width: parent.width; height: Kirigami.Units.gridUnit; }
            header: Item { width: parent.width; height: Kirigami.Units.gridUnit; }
            currentIndex: -1;
            cellWidth: Kirigami.Units.gridUnit * 10
            cellHeight: Kirigami.Units.gridUnit * 10
            model: Peruse.FilterProxy {
                id: searchFilterProxy;
                sourceModel: contentList.newlyAddedCategoryModel
            }

            function previousEntry() {
                if(currentIndex > 0) {
                    currentIndex--;
                }
            }
            function nextEntry() {
                if(currentIndex < model.rowCount() - 1) {
                    currentIndex++;
                }
            }
            delegate: ListComponents.BookTileTall {
                id: bookTile;
                width: Kirigami.Units.gridUnit * 10
                height: width
                author: model.author ? model.author : i18nc("used for the author data in book lists if author is empty", "(unknown)");
                title: model.title;
                filename: model.filename;
                thumbnail: model.thumbnail;
                categoryEntriesCount: model.categoryEntriesCount;
                currentPage: model.currentPage;
                totalPages: model.totalPages;
                onBookSelected: root.bookSelected(filename, currentPage);
                selected: searchList.currentIndex === index;
            }
        }
    }


    ColumnLayout {
        id: startWithThese;
        property Item currentItem: null;
        property var itemArray: [rread0, rread1, rread2, rread3];
        property QtObject mostRecentlyRead0: fakeBook;
        property QtObject mostRecentlyRead1: fakeBook;
        property QtObject mostRecentlyRead2: fakeBook;
        property QtObject mostRecentlyRead3: fakeBook;
        property int mostRecentlyAdded0: -1;

        clip: true;
        visible: !searching

        function updateRecentlyRead() {
            mostRecentlyAdded0 = -1;
            mostRecentlyRead0 = mostRecentlyRead1 = mostRecentlyRead2 = mostRecentlyRead3 = fakeBook;
            startWithThese.mostRecentlyRead0 = contentList.bookFromFile(peruseConfig.recentlyOpened[0]);
            startWithThese.mostRecentlyRead1 = contentList.bookFromFile(peruseConfig.recentlyOpened[1]);
            startWithThese.mostRecentlyRead2 = contentList.bookFromFile(peruseConfig.recentlyOpened[2]);
            startWithThese.mostRecentlyRead3 = contentList.bookFromFile(peruseConfig.recentlyOpened[3]);
            if(startWithThese.currentItem != null) {
                startWithThese.currentItem = rread0;
            }
            // the model might be null, if we haven't actually got any entries... so, let's check that
            // and just leave the whole thing empty in that case :)
            if(contentList.newlyAddedCategoryModel) {
                startWithThese.mostRecentlyAdded0 = 0;
            }
        }

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

        anchors.fill: parent
        Layout.margins: Kirigami.Units.largeSpacing

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
        Peruse.PropertyContainer {
            id: fakeBook;
            property string author: "unnamed";
            property string title: "unnamed";
            property string filename: "";
            property string thumbnail: "";
            property string currentPage: "0";
            property string totalPages: "0";
        }
        Kirigami.Heading {
            level: 2
            text: i18nc("title of list of recently opened books", "Continue Reading");
            Layout.fillWidth: true
        }
        FocusScope {
            Layout.fillHeight: true
            Layout.fillWidth: true
            GridLayout {
                anchors.fill: parent
                columns: 3
                ListComponents.BookTileTall {
                    id: rread0;
                    Layout.row: 0
                    Layout.column: 0
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    author: startWithThese.mostRecentlyRead0.readProperty("author");
                    title: startWithThese.mostRecentlyRead0.readProperty("title");
                    filename: startWithThese.mostRecentlyRead0.readProperty("filename");
                    thumbnail: startWithThese.mostRecentlyRead0.readProperty("thumbnail");
                    categoryEntriesCount: 0;
                    currentPage: startWithThese.mostRecentlyRead0.readProperty("currentPage");
                    totalPages: startWithThese.mostRecentlyRead0.readProperty("totalPages");
                    onBookSelected: root.bookSelected(filename, currentPage);
                    selected: startWithThese.currentItem === this;
                    focus: true
                }
                ListComponents.BookTileTall {
                    id: rread1;
                    Layout.column: 1
                    Layout.row: 0
                    Layout.fillHeight: true
                    Layout.fillWidth: true
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
                ListComponents.BookTileTall {
                    id: rread2;
                    Layout.column: 0
                    Layout.row: 1
                    Layout.fillHeight: true
                    Layout.fillWidth: true
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
                    Layout.column: 1
                    Layout.row: 1
                    Layout.fillHeight: true
                    Layout.fillWidth: true
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
                Item {
                    id: titleContainer;
                    Layout.column: 2
                    Layout.row: 0
                    Layout.rowSpan: 2
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    visible: Window.window.width > Kirigami.Units.gridUnit * 30
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
            }
        }

        Kirigami.Heading {
            text: i18nc("title of list of recently discovered books", "Recently Added")
            level: 2
            Layout.fillWidth: true
            visible: !searching
        }
    }

    footer: QtControls.ScrollView {
        height: searching ? 0 : Kirigami.Units.gridUnit * 10 
        visible: !searching
        ListView {
            id: newAddedListView
            orientation: Qt.Horizontal
            model: contentList.newlyAddedCategoryModel

            delegate: ListComponents.BookTileTall {
                id: firstRecentlyAddedBook;
                property QtObject book: model
                visible: filename !== "";
                height: visible ? newAddedListView.height : 0;
                width: Kirigami.Units.gridUnit * 10
                author: book.author;
                title: book.title;
                filename: book.filename;
                thumbnail: book.thumbnail;
                categoryEntriesCount: 0;
                currentPage: book.currentPage;
                totalPages: book.totalPages;
                onBookSelected: root.bookSelected(filename, currentPage);
            }
        }
    }
}
