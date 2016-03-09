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

import org.kde.plasma.mobilecomponents 0.2 as MobileComponents
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

import org.kde.peruse 0.1 as Peruse

import "listcomponents" as ListComponents

MobileComponents.Page {
    id: root;
    color: MobileComponents.Theme.viewBackgroundColor;
    objectName: "bookshelf";
    property alias model: shelfList.model;
    property string sectionRole: "title";
    property int sectionCriteria: ViewSection.FirstCharacter;
    signal bookSelected(string filename, int currentPage);
    property string headerText;

    function openBook(index) {
        if(shelfList.model.indexIsBook(index)) {
            var book = shelfList.model.get(index);
            root.bookSelected(book.readProperty("filename"), book.readProperty("currentPage"));
        }
        else {
            var catEntry = shelfList.model.getEntry(index);
            mainWindow.pageStack.push(bookshelf, { focus: true, headerText: "Comics in folder: " + catEntry.readProperty("title"), model: catEntry.readProperty("entriesModel") });
        }
    }

    function closeShelf() {
        mainWindow.pageStack.pop();
    }
    property list<QtObject> mobileActions;
    property list<QtObject> desktopActions: [
        Action {
            text: "Back";
            shortcut: "Esc";
            iconName: "action-close";
            onTriggered: closeShelf();
            enabled: mainWindow.pageStack.currentPage == root;
        },
        Action {
            text: "Select previous book";
            shortcut: StandardKey.MoveToPreviousChar
            iconName: "action-previous";
            onTriggered: shelfList.previousEntry();
            enabled: mainWindow.pageStack.currentPage == root;
        },
        Action {
            text: "Select next book";
            shortcut: StandardKey.MoveToNextChar;
            iconName: "action-next";
            onTriggered: shelfList.nextEntry();
            enabled: mainWindow.pageStack.currentPage == root;
        },
        Action {
            text: "Open selected book";
            shortcut: "Return";
            iconName: "action-open";
            onTriggered: openBook(shelfList.currentIndex);
            enabled: mainWindow.pageStack.currentPage == root;
        }
    ]
    contextualActions: PLASMA_PLATFORM.substring(0, 5) === "phone" ? mobileActions : desktopActions;

    mainAction: Action {
        text: "Search Books";
        iconName: "system-search";
        onTriggered: {
            searchField.forceActiveFocus();
            Qt.inputMethod.show(); // Would be nice if this happened automatically, but... no such luck
        }
    }

    Item {
        id: searchContainer;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        clip: true;
        height: searchField.focus || searchField.text.length > 0 ? searchHeight : 0;
        Behavior on height { PropertyAnimation { duration: mainWindow.animationDuration; easing.type: Easing.InOutQuad; } }
        property int searchHeight: searchField.text.length > 0 ? parent.height : searchField.height;
        PlasmaComponents.TextField {
            id: searchField;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            placeholderText: "Tap and type to search";
            onTextChanged: {
                if(text.length > 0) {
                    searchTimer.start();
                }
                else {
                    searchTimer.stop();
                }
            }
        }
        Timer {
            id: searchTimer;
            interval: 250;
            repeat: false;
            running: false;
            onTriggered: searchFilterProxy.setFilterFixedString(searchField.text);
        }
        GridView {
            id: searchList;
            clip: true;
            anchors {
                top: searchField.bottom;
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            cellWidth: width / 2;
            cellHeight: Math.max(
                (root.height * 2 / 7),
                Math.min(cellWidth, (units.iconSizes.enormous + units.largeSpacing * 3 + MobileComponents.Theme.defaultFont.pixelSize))
            );
            currentIndex: -1;
            model: Peruse.FilterProxy {
                id: searchFilterProxy;
                sourceModel: root.model;
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
                    selected: searchList.currentIndex === index;
                }
                ListComponents.BookTileTall {
                    id: bookTile;
                    height: model.categoryEntriesCount < 1 ? neededHeight : 0;
                    width: parent.width;
                    author: model.author ? model.author : "(unknown)";
                    title: model.title;
                    filename: model.filename;
                    categoryEntriesCount: model.categoryEntriesCount;
                    currentPage: model.currentPage;
                    totalPages: model.totalPages;
                    onBookSelected: root.bookSelected(filename, currentPage);
                    selected: searchList.currentIndex === index;
                }
            }
        }
    }

    GridView {
        id: shelfList;
        clip: true;
        anchors {
            top: searchContainer.bottom;
            left: parent.left;
            right: parent.right;
        }
        height: parent.height;
        cellWidth: width / 2;
        cellHeight: root.height * 2 / 7;
        header: ListComponents.ListPageHeader { text: root.headerText; }
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
                author: model.author ? model.author : "(unknown)";
                title: model.title;
                filename: model.filename;
                categoryEntriesCount: model.categoryEntriesCount;
                currentPage: model.currentPage;
                totalPages: model.totalPages;
                onBookSelected: root.bookSelected(filename, currentPage);
                selected: shelfList.currentIndex === index;
            }
        }
    }
}
