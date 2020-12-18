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
import QtQuick.Controls 2.15 as QtControls
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.14 as Kirigami

import org.kde.peruse 0.1 as Peruse
import "listcomponents" as ListComponents

/**
 * @brief This lays out the books and categories.
 * 
 * It uses BookTileTall for the books and CategoryTileTall
 * for showing the categories. Categories can be selected to open
 * a new bookshelf from the right, showing the entries in that
 * subcategory. This is particularly in use with the folder category.
 * 
 * There is also access to the SearchBox, and it is possible to access
 * a BookTile by press+holding the thumbnail.
 * This holds information about the book.
 */
Kirigami.ScrollablePage {
    id: root;
    property alias pageHeader: shelfList.header;
    title: headerText;
    property string categoryName: "bookshelf";
    objectName: "bookshelf";
    property QtObject model;
    /**
     * Allows you to override the default behaviour of searching inside the shelf (such as
     * for the welcome page where we want to search the global list)
     */
    property alias searchModel: searchFilterProxy.sourceModel;
    property string sectionRole: "title";
    property int sectionCriteria: ViewSection.FirstCharacter;
    signal bookSelected(string filename, int currentPage);
    property string headerText;
    property bool isSearching: searchFilterProxy.filterString.length > 0;

    function openBook(index) {
        applicationWindow().contextDrawer.close();
        var whatModel = isSearching ? searchFilterProxy.sourceModel : shelfList.model;
        var whatIndex = isSearching ? searchFilterProxy.sourceIndex(index) : index;
        if(whatModel.indexIsBook(whatIndex)) {
            var book = whatModel.get(whatIndex);
            root.bookSelected(book.readProperty("filename"), book.readProperty("currentPage"));
        }
        else {
            var catEntry = whatModel.getEntry(whatIndex);
            applicationWindow().pageStack.push(bookshelf, { focus: true, headerText: catEntry.readProperty("title"), model: catEntry.readProperty("entriesModel") });
        }
    }

    function closeShelf() {
        applicationWindow().contextDrawer.close();
        applicationWindow().pageStack.pop();
    }
    property bool isCurrentContext: isCurrentPage && !applicationWindow().bookOpen
    property list<QtObject> mobileActions;
    property list<QtObject> desktopActions: [
        Kirigami.Action {
            text: i18nc("Navigate one page back", "Back");
            shortcut: bookDetails.sheetOpen ? "" : "Esc";
            iconName: "dialog-close";
            onTriggered: closeShelf();
            enabled: root.isCurrentContext && !Kirigami.Settings.isMobile && applicationWindow().pageStack.currentIndex > 0;
        },
        Kirigami.Action {
            text: i18nc("Open the book which is currently selected in the list", "Open Selected Book");
            shortcut: bookDetails.sheetOpen? "" : "Return";
            iconName: "document-open";
            onTriggered: openBook(shelfList.currentIndex);
            enabled: root.isCurrentContext && !Kirigami.Settings.isMobile;
        }
    ]
    actions {
        contextualActions: Kirigami.Settings.isMobile ? mobileActions : desktopActions;
        main: bookDetails.sheetOpen ? bookDetailsAction : mainShelfAction;
    }
    Kirigami.Action {
        id: mainShelfAction;
        text: i18nc("search in the list of books (not inside the books)", "Search Books");
        iconName: "system-search";
        onTriggered: searchBox.activate();
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
                    searchFilterProxy.filterString = "";
                }
            }
            Timer {
                id: searchTimer;
                interval: 250;
                repeat: false;
                running: false;
                onTriggered: searchFilterProxy.filterString = searchField.text;
            }
        }
    }
    Kirigami.Action {
        id: bookDetailsAction;
        text: i18nc("Closes the book details drawer", "Close");
        shortcut: bookDetails.sheetOpen ? "Esc" : "";
        iconName: "dialog-cancel";
        onTriggered: bookDetails.close();
        enabled: root.isCurrentContext;
    }

    GridView {
        id: shelfList;
        model: isSearching ? searchFilterProxy : root.model;
        Peruse.FilterProxy {
            id: searchFilterProxy;
            sourceModel: shelfList.model;
        }
        keyNavigationEnabled: true;
        clip: true;
        footer: ColumnLayout {
            width: parent.width;
            spacing: Kirigami.Units.largeSpacing;
            opacity: 0.3
            Item { Layout.fillWidth: true; height: Kirigami.Units.iconSizes.large + Kirigami.Units.largeSpacing; }
            Rectangle {
                Layout.alignment: Qt.AlignHCenter;
                Layout.minimumWidth: parent.width * .7
                Layout.maximumWidth: Layout.minimumWidth
                height: 1;
                color: Kirigami.Theme.textColor;
            }
            Kirigami.Icon {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                height: Kirigami.Units.iconSizes.enormous
                source: "peruse"
            }
            Item { Layout.fillWidth: true; height: Kirigami.Units.iconSizes.large + Kirigami.Units.largeSpacing; }
        }

        readonly property int scrollBarSpace: root.flickable.QtControls.ScrollBar.vertical ? root.flickable.QtControls.ScrollBar.vertical.width : 0
        readonly property int availableWidth: shelfList.width - scrollBarSpace - 4
        readonly property int implicitCellWidth: Kirigami.Units.gridUnit * 15
        cellWidth: Math.floor(availableWidth / Math.max(Math.floor(availableWidth / (implicitCellWidth + Kirigami.Units.gridUnit)), 2))
        cellHeight: Kirigami.Units.gridUnit * 13;

        currentIndex: -1;

        delegate: Item {
            height: shelfList.cellHeight;
            width: shelfList.cellWidth;
            ListComponents.CategoryTileTall {
                id: categoryTile;
                visible: height > 0;
                height: model.categoryEntriesCount > 0 ? shelfList.cellHeight : 0;
                width: parent.width;
                count: model.categoryEntriesCount;
                title: model.title;
                entriesModel: model.categoryEntriesModel ? model.categoryEntriesModel : null;
                selected: shelfList.currentIndex === index;
            }
            ListComponents.BookTileTall {
                id: bookTile;
                visible: height > 0;
                height: model.categoryEntriesCount < 1 ? shelfList.cellHeight : 0;
                width: parent.width;
                author: model.author ? model.author : i18nc("used for the author data in book lists if author is empty", "(unknown)");
                title: model.title;
                filename: model.filename;
                thumbnail: model.categoryEntriesCount < 1 ? model.thumbnail : "";
                categoryEntriesCount: model.categoryEntriesCount;
                currentPage: model.currentPage;
                totalPages: model.totalPages;
                onBookSelected: root.bookSelected(filename, currentPage);
                selected: shelfList.currentIndex === index;
                onPressAndHold: bookDetails.showBookInfo(model.index);
                pressIndicator: true;
            }
        }

        Kirigami.OverlaySheet {
            id: bookDetails;
            function showBookInfo(index) {
                currentBook = root.model.getEntry(index);
                open();
            }
            property QtObject currentBook: fakeBook;
            property QtObject fakeBook: Peruse.PropertyContainer {
                property string author: "";
                property string title: "";
                property string filename: "";
                property string publisher: "";
                property string thumbnail: "";
                property string currentPage: "0";
                property string totalPages: "0";
                property string comment: "";
            }
            ListComponents.BookTile {
                id: detailsTile;
                height: neededHeight;
                width: shelfList.width - Kirigami.Units.largeSpacing * 2;
                author: bookDetails.currentBook.readProperty("author");
                publisher: bookDetails.currentBook.readProperty("publisher");
                title: bookDetails.currentBook.readProperty("title");
                filename: bookDetails.currentBook.readProperty("filename");
                thumbnail: bookDetails.currentBook.readProperty("thumbnail");
                categoryEntriesCount: 0;
                currentPage: bookDetails.currentBook.readProperty("currentPage");
                totalPages: bookDetails.currentBook.readProperty("totalPages");
                description: bookDetails.currentBook.readProperty("description");
                onBookSelected: {
                    bookDetails.close();
                    applicationWindow().showBook(fileSelected, currentPage);
                }
                onBookDeleteRequested: {
                    contentList.removeBook(fileSelected, true);
                    close();
                }
            }
        }
    }
}
