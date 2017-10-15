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
import QtQuick.Controls 2.2 as QtControls

import org.kde.kirigami 2.1 as Kirigami

Item {
    id: root;
    property bool selected: false;
    property alias count: categoryCount.text;
    property alias title: categoryTitle.text
    property QtObject entriesModel;
    property int neededHeight: categoryImage.height + categoryTitle.height + Kirigami.Units.largeSpacing;
    visible: height > 0;
    enabled: visible;
    clip: true;
    MouseArea {
        anchors.fill: parent;
        onClicked: {
            applicationWindow().pageStack.push(bookshelf, { focus: true, headerText: root.title, model: root.entriesModel })
        }
    }
    Rectangle {
        anchors.fill: parent;
        color: Kirigami.Theme.highlightColor;
        opacity: root.selected ? 1 : 0;
        Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
    }
    Item {
        id: categoryImage;
        anchors {
            top: parent.top;
            horizontalCenter: parent.horizontalCenter;
            margins: Kirigami.Units.largeSpacing;
        }
        width: Math.min(parent.width - Kirigami.Units.largeSpacing * 2, Kirigami.Units.iconSizes.enormous + Kirigami.Units.largeSpacing * 2);
        height: width;
        Rectangle {
            anchors.centerIn: coverImage;
            width: tileBg.width;
            height: tileBg.height;
            color: Kirigami.Theme.viewBackgroundColor;
            border {
                width: 2;
                color: Kirigami.Theme.viewTextColor;
            }
            rotation: 16;
            radius: 2;
            Rectangle {
                anchors {
                    fill: parent;
                    margins: Kirigami.Units.smallSpacing;
                }
                color: Kirigami.Theme.textColor;
            }
        }
        Rectangle {
            anchors.centerIn: coverImage;
            width: tileBg.width;
            height: tileBg.height;
            color: Kirigami.Theme.viewBackgroundColor;
            border {
                width: 2;
                color: Kirigami.Theme.viewTextColor;
            }
            rotation: 8;
            radius: 2;
            Rectangle {
                anchors {
                    fill: parent;
                    margins: Kirigami.Units.smallSpacing;
                }
                color: Kirigami.Theme.textColor;
            }
        }
        Rectangle {
            id: tileBg;
            anchors.centerIn: coverImage;
            width: Math.max(coverImage.paintedWidth, Kirigami.Units.iconSizes.large) + Kirigami.Units.smallSpacing * 2;
            height: Math.max(coverImage.paintedHeight, Kirigami.Units.iconSizes.large) + Kirigami.Units.smallSpacing * 2;
            color: Kirigami.Theme.viewBackgroundColor;
            border {
                width: 2;
                color: Kirigami.Theme.viewTextColor;
            }
            radius: 2;
        }
        Image {
            id: coverImage;
            anchors {
                fill: parent;
                margins: Kirigami.Units.largeSpacing;
            }
            source: root.entriesModel ? root.entriesModel.get(0).readProperty("thumbnail") : "";
            asynchronous: true;
            fillMode: Image.PreserveAspectFit;
        }
        Rectangle {
            anchors {
                fill: categoryCount;
                margins: -Kirigami.Units.smallSpacing;
            }
            radius: height / 2;
            color: Kirigami.Theme.highlightColor;
        }
        QtControls.Label {
            id: categoryCount;
            anchors {
                bottom: tileBg.bottom;
                right: tileBg.right;
            }
            height: paintedHeight;
            width: paintedWidth;
            color: Kirigami.Theme.highlightedTextColor;
        }
    }
    QtControls.Label {
        id: categoryTitle;
        anchors {
            top: categoryImage.bottom;
            left: parent.left;
            right: parent.right;
            margins: Kirigami.Units.smallSpacing;
            topMargin: 0;
        }
        height: paintedHeight;
        maximumLineCount: 2;
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        elide: Text.ElideMiddle;
        horizontalAlignment: Text.AlignHCenter;
    }
}
