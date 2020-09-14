/*
 * SPDX-File-Copyright-Text: 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
 * SPDX-File-Copyright-Text: 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick 2.12
import QtQuick.Controls 2.12 as QtControls
import QtQuick.Layouts 1.15

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
    property real heightBook: Kirigami.Units.gridUnit * 8
    property list<QtObject> mobileActions;
    property list<QtObject> desktopActions: [
        Kirigami.Action {
            text: i18n("Open Selected Book");
            shortcut: "Return";
            iconName: "document-open";
            onTriggered: bookSelected(startWithThese.currentItem.filename, startWithThese.currentItem.currentPage);
            enabled: root.isCurrentContext && !Kirigami.Settings.isMobile;
        },
        Kirigami.Action {
            text: i18nc("select the previous book entry in the list", "Previous Book");
            shortcut: StandardKey.MoveToPreviousChar;
            iconName: "go-previous";
            onTriggered: startWithThese.selectPrevious();
            enabled: root.isCurrentContext && !Kirigami.Settings.isMobile;
        },
        Kirigami.Action {
            text: i18nc("select the next book entry in the list", "Next Book");
            shortcut: StandardKey.MoveToNextChar;
            iconName: "go-next";
            onTriggered: startWithThese.selectNext();
            enabled: root.isCurrentContext && !Kirigami.Settings.isMobile;
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

    ColumnLayout {
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
            }
        }

        anchors.fill: parent
        Layout.margins: Kirigami.Units.largeSpacing

        SearchBox {
            id: searchBox;
            Layout.fillWidth: true
            model: contentList.newlyAddedCategoryModel;
            Layout.maximumHeight: parent.height - titleContainer.height / 2;
            onBookSelected: root.bookSelected(filename, currentPage);
        }

        Item {
            id: titleContainer;
            Layout.fillWidth: true
            Layout.preferredHeight: applicationWindow().isLoading ? (parent.height / 3) : (appNameLabel.height + appDescriptionLabel.height + Kirigami.Units.largeSpacing);
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
        visible: opacity > 0
        opacity: applicationWindow().isLoading ? 0 : 1;
        Behavior on opacity { NumberAnimation { duration: applicationWindow().animationDuration; } }
        //contentWidth: width;
        //contentHeight: recentItemsColumn.height;
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
        ListComponents.Section {
            text: i18nc("title of list of recently opened books", "Continue Reading");
            width: startWithThese.width;
            height: rread0.height > 0 ? paintedHeight : 0;
            visible: height > 0;
        }
        Row {
            Layout.fillWidth: true
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
        }
    }

    footer: ColumnLayout {
        Kirigami.Heading {
            text: i18nc("title of list of recently discovered books", "Recently Added");
            level: 2
            Layout.fillWidth: true
        }

        ListView {
            Layout.fillWidth: true
            Layout.minimumHeight: heightBook
            orientation: Qt.Horizontal
            model: contentList.newlyAddedCategoryModel

            delegate: ListComponents.BookTileTall {
                id: firstRecentlyAddedBook;
                property QtObject book: model
                visible: filename !== "";
                height: visible ? heightBook : 0;
                width: startWithThese.width / 2;
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
