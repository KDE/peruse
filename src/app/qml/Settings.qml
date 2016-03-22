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

import org.kde.kirigami 1.0 as Kirigami
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

import org.kde.peruse 0.1 as Peruse

import "listcomponents" as ListComponents

Kirigami.Page {
    id: root;
    title: "Settings";

    Item {
        width: root.width - (root.leftPadding + root.rightPadding);
        height: root.height - (root.topPadding + root.bottomPadding);
        Column {
            width: parent.width;
            height: childrenRect.height;
            clip: true;
            Item {
                width: parent.width;
                height: units.largeSpacing;
            }
            Item {
                height: folderHeader.height;
                width: parent.width - folderAdd.width - units.smallSpacing;
                ListComponents.Section { id: folderHeader; text: "Search Folders"; }
                ToolButton {
                    id: folderAdd;
                    iconName: "list-add";
                    onClicked: mainWindow.pageStack.push(folderDlg);
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        left: parent.right;
                    }
                }
            }
            Repeater {
                model: peruseConfig.bookLocations;
                delegate: Kirigami.SwipeListItem {
                    actions: [
                        Kirigami.Action {
                            text: "Delete";
                            iconName: "list-remove"
                            onTriggered: peruseConfig.removeBookLocation(peruseConfig.bookLocations[index]);
                        }
                    ]
                    Kirigami.BasicListItem {
                        label: peruseConfig.bookLocations[index];
                    }
                }
            }
        }

        Rectangle {
            id: addingNewBooksProgress;
            color: Kirigami.Theme.viewBackgroundColor;
            anchors.fill: parent;
            opacity: 0;
            Behavior on opacity { PropertyAnimation { duration: mainWindow.animationDuration; } }
            Connections {
                target: contentList.contentModel;
                onSearchCompleted: addingNewBooksProgress.opacity = 0;
            }
            enabled: opacity > 0;
            MouseArea {
                enabled: parent.enabled;
                anchors.fill: parent;
            }
            PlasmaComponents.Label {
                anchors {
                    bottom: parent.verticalCenter;
                    left: parent.left;
                    right: parent.right;
                }
                horizontalAlignment: Text.AlignHCenter;
                text: "Please wait while we find your comics...";
            }
            PlasmaComponents.BusyIndicator {
                id: loadingSpinner;
                anchors {
                    top: parent.verticalCenter;
                    left: parent.left;
                    right: parent.right;
                }
                running: addingNewBooksProgress.enabled;
            }
            PlasmaComponents.Label {
                anchors {
                    top: loadingSpinner.bottom;
                    left: parent.left;
                    right: parent.right;
                }
                horizontalAlignment: Text.AlignHCenter;
                text: contentList.count;
            }
        }

        Component {
            id: folderDlg;
            Kirigami.Page {
                FileFinder {
                    width: root.width - (root.leftPadding + root.rightPadding);
                    height: root.height - (root.topPadding + root.bottomPadding);
                    folder: peruseConfig.homeDir();
                    showFiles: false;
                    onAccepted: {
                        peruseConfig.addBookLocation(selectedItem());
                        mainWindow.pageStack.pop();

                        // Now search for new items in that locations...
                        var locations = peruseConfig.bookLocations;
                        addingNewBooksProgress.opacity = 1;
                        contentList.contentModel.addLocation(locations[locations.length - 1]);
                        contentList.contentModel.startSearch();
                    }
                    onAborted: mainWindow.pageStack.pop();
                }
            }
        }
    }
}
