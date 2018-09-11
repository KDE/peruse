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
import QtQuick.Controls 1.4 as QtControls

import org.kde.kirigami 2.1 as Kirigami

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
    title: headerText;
    property string categoryName: "bookshelf";
    objectName: "bookshelf";
    property alias model: shelfList.model;
    property string sectionRole: "title";
    property int sectionCriteria: ViewSection.FirstCharacter;
    signal bookSelected(string filename, int currentPage);
    property string headerText;

    function openBook(index) {
        applicationWindow().contextDrawer.close();
        if(shelfList.model.indexIsBook(index)) {
            var book = shelfList.model.get(index);
            root.bookSelected(book.readProperty("filename"), book.readProperty("currentPage"));
        }
        else {
            var catEntry = shelfList.model.getEntry(index);
            applicationWindow().pageStack.push(bookshelf, { focus: true, headerText: catEntry.readProperty("title"), model: catEntry.readProperty("entriesModel") });
        }
    }

    function closeShelf() {
        applicationWindow().contextDrawer.close();
        applicationWindow().pageStack.pop();
    }
    property list<QtObject> mobileActions;
    property list<QtObject> desktopActions: [
        Kirigami.Action {
            text: i18nc("Navigate one page back", "Back");
            shortcut: bookDetails.sheetOpen ? "" : "Esc";
            iconName: "dialog-close";
            onTriggered: closeShelf();
            enabled: root.isCurrentPage && applicationWindow().deviceType === applicationWindow().deviceTypeDesktop && applicationWindow().pageStack.currentIndex > 0;
        },
//         Kirigami.Action {
//             text: i18nc("Select the previous book in the list", "Select previous book");
//             shortcut: StandardKey.MoveToPreviousChar
//             iconName: "go-previous";
//             onTriggered: shelfList.previousEntry();
//             enabled: root.isCurrentPage && applicationWindow().deviceType === applicationWindow().deviceTypeDesktop;
//         },
//         Kirigami.Action {
//             text: i18nc("Select the next book in the list", "Select next book");
//             shortcut: StandardKey.MoveToNextChar;
//             iconName: "go-next";
//             onTriggered: shelfList.nextEntry();
//             enabled: root.isCurrentPage && applicationWindow().deviceType === applicationWindow().deviceTypeDesktop;
//         },
        Kirigami.Action {
            text: i18nc("Open the book which is currently selected in the list", "Open selected book");
            shortcut: "Return";
            iconName: "document-open";
            onTriggered: openBook(shelfList.currentIndex);
            enabled: root.isCurrentPage && applicationWindow().deviceType === applicationWindow().deviceTypeDesktop;
        }
    ]
    actions {
        contextualActions: PLASMA_PLATFORM.substring(0, 5) === "phone" ? mobileActions : desktopActions;
        main: bookDetails.sheetOpen ? bookDetailsAction : mainShelfAction;
    }
    Kirigami.Action {
        id: mainShelfAction;
        text: i18nc("search in the list of books (not inside the books)", "Search Books");
        iconName: "system-search";
        onTriggered: searchBox.activate();
        enabled: root.isCurrentPage;
    }
    Kirigami.Action {
        id: bookDetailsAction;
        text: i18n("Closes the book details drawer", "Close");
        shortcut: bookDetails.sheetOpen ? "Esc" : "";
        iconName: "dialog-cancel";
        onTriggered: bookDetails.close();
        enabled: root.isCurrentPage;
    }

    GridView {
        id: shelfList;
        SearchBox {
            id: searchBox;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            maxHeight: parent.height;
            model: root.model;
            onBookSelected: root.bookSelected(filename, currentPage);
        }
        clip: true;
        footer: Item { width: parent.width; height: Kirigami.Units.iconSizes.large + Kirigami.Units.largeSpacing; }
        cellWidth: width / 2;
        cellHeight: root.height * 3 / 8;
        currentIndex: -1;

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
        delegate: Item {
            height: model.categoryEntriesCount === 0 ? bookTile.neededHeight : categoryTile.neededHeight;
            width: root.width / 2;
            ListComponents.CategoryTileTall {
                id: categoryTile;
                height: model.categoryEntriesCount > 0 ? neededHeight : 0;
                width: parent.width;
                count: model.categoryEntriesCount;
                title: model.title;
                entriesModel: model.categoryEntriesModel ? model.categoryEntriesModel : null;
                selected: shelfList.currentIndex === index;
            }
            ListComponents.BookTileTall {
                id: bookTile;
                height: model.categoryEntriesCount < 1 ? neededHeight : 0;
                width: parent.width;
                author: model.author ? model.author : i18nc("used for the author data in book lists if autor is empty", "(unknown)");
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
            onBookSelected: {
                bookDetails.close();
                applicationWindow().showBook(filename, currentPage);
            }
            onBookDeleteRequested: {
                contentList.removeBook(detailsTile.filename, true);
                close();
            }
        }
    }
}
