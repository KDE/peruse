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
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0

import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.peruse 0.1 as Peruse

Item {
    id: root;
    objectName: "bookshelf";
    property alias model: shelfList.model;
    signal bookSelected(string filename);
    property alias headerText: shelfTitle.text;
    property int group: Peruse.BookModel.GroupByNone;
    PlasmaExtras.Title {
        id: shelfTitle;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
    }
    ListView {
        id: shelfList;
        clip: true;
        anchors {
            top: shelfTitle.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        delegate: Item {
            height: 200;
            width: root.width;
            MouseArea {
                anchors.fill: parent;
                onClicked: root.bookSelected(model.filename);
            }
            Item {
                id: bookCover;
                anchors {
                    top: parent.top;
                    left: parent.left;
                    bottom: parent.bottom;
                }
                width: height;
                Rectangle {
                    radius: 2;
                    border.width: 1;
                    border.color: "black";
                    color: "silver";
                    height: parent.height - 6;
                    width: height * 2 / 3;
                    anchors {
                        centerIn: parent;
                        margins: 3;
                    }
                }
            }
            Text {
                id: bookTitle;
                anchors {
                    top: parent.top;
                    left: bookCover.right;
                    right: parent.right;
                }
                text: model.filename;
                font.bold: true;
            }
        }
    }
}
