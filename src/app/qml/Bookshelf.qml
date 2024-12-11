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

import QtQuick
import QtQuick.Controls as QtControls
import QtQuick.Layouts
import Qt.labs.qmlmodels

import org.kde.kirigami as Kirigami

import org.kde.peruse as Peruse
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

    signal bookSelected(string filename, int currentPage);

    property bool isCurrentContext: isCurrentPage && !applicationWindow().bookOpen
    property alias pageHeader: shelfList.header;
    property string categoryName: "bookshelf";
    property QtObject model;
    /**
     * Allows you to override the default behaviour of searching inside the shelf (such as
     * for the welcome page where we want to search the global list)
     */
    property alias searchModel: searchFilterProxy.sourceModel;
    property string sectionRole: "title";
    property int sectionCriteria: ViewSection.FirstCharacter;
    property string headerText;
    property string searchText: ""
    property bool searching: searchText.length > 0
    property bool isSearching: searchFilterProxy.filterString.length > 0;

    function openBook(index: int): void {
        applicationWindow().contextDrawer.close();
        const whatModel = isSearching ? searchFilterProxy.sourceModel : shelfList.model;
        const whatIndex = isSearching ? searchFilterProxy.sourceIndex(index) : index;
        if (!whatModel) {
            return;
        }
        if (whatModel.indexIsBook(whatIndex)) {
            const book = whatModel.get(whatIndex);
            root.bookSelected(book.readProperty("filename"), book.readProperty("currentPage"));
        } else {
            const catEntry = whatModel.getEntry(whatIndex);
            applicationWindow().pageStack.push(bookshelf, { focus: true, headerText: catEntry.readProperty("title"), model: catEntry.readProperty("entriesModel") });
        }
    }

    function closeShelf(): void {
        applicationWindow().contextDrawer.close();
        applicationWindow().pageStack.pop();
    }

    title: headerText;
    objectName: "bookshelf";

    actions: {
        var ret = [];
        if (bookDetails.visible) {
            ret.push(bookDetailsAction);
        } else {
            ret.push(mainShelfAction);
        }
        if (!Kirigami.Settings.isMobile) {
            ret.push(backAction);
            ret.push(openAction);
        }

        return ret;
    }
    Kirigami.Action {
        id: backAction
        text: i18nc("Navigate one page back", "Back");
        shortcut: bookDetails.visible ? "" : "Esc";
        icon.name: "dialog-close";
        onTriggered: closeShelf();
        enabled: root.isCurrentContext && !Kirigami.Settings.isMobile && applicationWindow().pageStack.currentIndex > 0;
    }

    Kirigami.Action {
        id: openAction
        text: i18nc("Open the book which is currently selected in the list", "Open Selected Book");
        shortcut: bookDetails.visible? "" : "Return";
        icon.name: "document-open";
        onTriggered: openBook(shelfList.currentIndex);
        enabled: root.isCurrentContext && !Kirigami.Settings.isMobile;
    }

    Kirigami.Action {
        id: mainShelfAction;
        text: i18nc("search in the list of books (not inside the books)", "Search Books");
        icon.name: "system-search";
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
        shortcut: bookDetails.visible ? "Esc" : "";
        icon.name: "dialog-cancel";
        onTriggered: bookDetails.close();
        enabled: root.isCurrentContext;
    }

    GridView {
        id: shelfList;
        model: isSearching ? searchFilterProxy : root.model;
        Peruse.FilterProxy {
            id: searchFilterProxy;
            sourceModel: root.model;
        }
        keyNavigationEnabled: true;
        clip: true;

        Kirigami.PlaceholderMessage {
            id: placeholderMessage
            width: parent.width - (Kirigami.Units.gridUnit * 4)
            visible: shelfList.count === 0
            anchors.centerIn: parent
            text: i18nc("Placeholder Text when there are no comics in the library that match the filter", "No matches")
        }

        readonly property int scrollBarSpace: root.flickable.QtControls.ScrollBar.vertical ? root.flickable.QtControls.ScrollBar.vertical.width : 0
        readonly property int availableWidth: shelfList.width - scrollBarSpace - 4
        readonly property int implicitCellWidth: Kirigami.Units.gridUnit * 15
        cellWidth: Math.floor(availableWidth / Math.max(Math.floor(availableWidth / (implicitCellWidth + Kirigami.Units.gridUnit)), 2))
        cellHeight: Kirigami.Units.gridUnit * 13;

        currentIndex: -1;

        delegate: DelegateChooser {
            role: "type"
            DelegateChoice {
                roleValue: "category"
                ListComponents.CategoryTileTall {
                    height: shelfList.cellHeight
                    width: shelfList.cellWidth

                    selected: shelfList.currentIndex === index
                }
            }

            DelegateChoice {
                roleValue: "book"
                ListComponents.BookTileTall {
                    height: shelfList.cellHeight
                    width: shelfList.cellWidth
                    //author: model.author ? model.author : i18nc("used for the author data in book lists if author is empty", "(unknown)");
                    selected: shelfList.currentIndex === index
                    onBookSelected: root.bookSelected(filename, currentPage);
                    onPressAndHold: bookDetails.showBookInfo(model.index);
                }
            }
        }

        Kirigami.OverlaySheet {
            id: bookDetails;
            function showBookInfo(index) {
                var whatModel = isSearching ? searchFilterProxy.sourceModel : shelfList.model;
                var whatIndex = isSearching ? searchFilterProxy.sourceIndex(index) : index;
                currentBook = whatModel.getEntry(whatIndex);
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
