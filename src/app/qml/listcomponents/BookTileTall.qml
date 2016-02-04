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

import QtQuick 2.1

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

Item {
    id: root;
    property bool selected: false;
    property alias title: bookTitle.text;
    property string author;
    property string filename;
    property int categoryEntriesCount;
    property string currentPage;
    signal bookSelected(string filename, int currentPage);

    property int neededHeight: bookCover.height + bookTitle.height + units.largeSpacing;
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
            horizontalCenter: parent.horizontalCenter;
            margins: units.largeSpacing;
        }
        width: Math.min(parent.width - units.largeSpacing * 2, units.iconSizes.enormous + units.largeSpacing * 2);
        height: width;
        Rectangle {
            anchors.centerIn: coverImage;
            width: coverImage.paintedWidth + units.smallSpacing * 2;
            height: coverImage.paintedHeight + units.smallSpacing * 2;
            color: theme.viewBackgroundColor;
            border {
                width: 2;
                color: theme.viewTextColor;
            }
            radius: 2;
        }
        Image {
            id: coverImage;
            anchors {
                fill: parent;
                margins: units.largeSpacing;
            }
            source: "image://preview/" + root.filename
            asynchronous: true;
            fillMode: Image.PreserveAspectFit;
        }
    }
    PlasmaComponents.Label {
        id: bookTitle;
        anchors {
            top: bookCover.bottom;
            left: parent.left;
            right: parent.right;
            margins: units.smallSpacing;
            topMargin: 0;
        }
        height: paintedHeight;
        maximumLineCount: 2;
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        elide: Text.ElideMiddle;
        horizontalAlignment: Text.AlignHCenter;
    }
}
