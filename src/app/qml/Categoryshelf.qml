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

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

Item {
    id: root;
    property alias model: shelfList.model;
    signal bookSelected(string filename, int currentPage);
    property alias headerText: shelfTitle.text;
    PlasmaExtras.Heading {
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
        section {
            property: "categoryName";
            criteria: ViewSection.FirstCharacter;
            delegate: PlasmaExtras.Heading {
                anchors.leftMargin: 5;
                width: root.width;
                text: section;
                Rectangle {
                    anchors {
                        right: parent.right;
                        rightMargin: 5;
                        verticalCenter: parent.verticalCenter;
                    }
                    height: 2;
                    radius: 2;
                    width: parent.width - parent.paintedWidth - 10;
                    color: parent.color;
                }
            }
        }
        delegate: Item {
            height: 50;
            width: root.width;
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    view.push({
                        item: bookshelf,
                        properties: { focus: true, headerText: "Comics in folder: " + model.categoryName, model: model.entriesModel }
                    })
                }
            }
            PlasmaExtras.Title {
                id: categoryTitle;
                anchors {
                    margins: 5;
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                }
                text: model.categoryName === "" ? "(unknown)" : model.categoryName;
            }
            PlasmaComponents.Label {
                id: categoryCount;
                anchors {
                    margins: 5;
                    verticalCenter: parent.verticalCenter;
                    right: parent.right;
                }
                text: model.entryCount;
            }
        }
    }
}
