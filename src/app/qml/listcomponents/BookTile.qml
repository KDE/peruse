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

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

Item {
    id: root;
    property bool selected: false;
    property alias title: bookTitle.text;
    property alias author: bookAuthor.text;
    property alias filename: bookFile.text;
    property int categoryEntriesCount;
    property string currentPage;
    signal bookSelected(string filename, int currentPage);

    property int neededHeight: bookTitle.height + bookAuthorLabel.height + bookFile.height + units.smallSpacing * 4;
    visible: height > 0;
    enabled: visible;
    clip: true;
    MouseArea {
        anchors.fill: parent;
        onClicked: root.bookSelected(root.filename, root.currentPage);
    }
    Rectangle {
        anchors.fill: parent;
        color: theme.highlightColor;
        opacity: root.selected ? 1 : 0;
        Behavior on opacity { NumberAnimation { duration: units.shortDuration; } }
    }
    Item {
        id: bookCover;
        anchors {
            top: parent.top;
            left: parent.left;
            bottom: parent.bottom;
            margins: units.smallSpacing;
        }
        width: units.iconSizes.huge + units.smallSpacing * 2;
        Image {
            id: coverImage;
            anchors.fill: parent;
            source: "image://preview/" + root.filename
            asynchronous: true;
            fillMode: Image.PreserveAspectFit;
        }
    }
    PlasmaComponents.Label {
        id: bookTitle;
        anchors {
            top: parent.top;
            left: bookCover.right;
            right: parent.right;
        }
        maximumLineCount: 1;
        elide: Text.ElideMiddle;
        font.weight: Font.Bold;
    }
    PlasmaComponents.Label {
        id: bookAuthorLabel;
        anchors {
            top: bookTitle.bottom;
            topMargin: units.smallSpacing;
            left: bookCover.right;
        }
        width: paintedWidth;
        text: "by";
        font.bold: true;
    }
    PlasmaComponents.Label {
        id: bookAuthor;
        anchors {
            top: bookTitle.bottom;
            topMargin: units.smallSpacing;
            left: bookAuthorLabel.right;
            leftMargin: units.smallSpacing;
            right: parent.right;
        }
        elide: Text.ElideRight;
    }
    PlasmaComponents.Label {
        id: bookFile;
        anchors {
            bottom: parent.bottom;
            left: bookCover.right;
            right: parent.right;
        }
        elide: Text.ElideMiddle;
        maximumLineCount: 1;
    }
}
