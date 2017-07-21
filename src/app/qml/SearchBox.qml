/*
 * Copyright (C) 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
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
import QtQuick.Controls 1.4 as QtControls

import org.kde.kirigami 2.1 as Kirigami

import org.kde.peruse 0.1 as Peruse
import "listcomponents" as ListComponents

Item {
    id: root;
    property int maxHeight: parent.height;
    property alias model: searchFilterProxy.sourceModel;
    function activate() {
        searchField.forceActiveFocus();
        Qt.inputMethod.show(); // Would be nice if this happened automatically, but... no such luck
    }
    signal bookSelected(string filename, int currentPage);
    clip: true;
    height: searchField.focus || searchField.text.length > 0 ? searchHeight : 0;
    Behavior on height { PropertyAnimation { duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
    property int searchHeight: searchField.text.length > 0 ? maxHeight : searchField.height;
    QtControls.TextField {
        id: searchField;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        placeholderText: i18nc("placeholder text for the search field", "Tap and type to search");
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
        footer: Item { width: parent.width; height: Kirigami.Units.iconSizes.large + Kirigami.Units.largeSpacing; }
        cellWidth: width / 2;
        cellHeight: Math.max(
            (root.height * 2 / 7),
            Math.min(cellWidth, (Kirigami.Units.iconSizes.enormous + Kirigami.Units.largeSpacing * 3 + Kirigami.Theme.defaultFont.pixelSize))
        );
        currentIndex: -1;
        model: Peruse.FilterProxy {
            id: searchFilterProxy;
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
                author: model.author ? model.author : i18nc("used for the author data in book lists if autor is empty", "(unknown)");
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
