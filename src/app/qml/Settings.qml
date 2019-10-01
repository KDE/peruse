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

import QtQuick 2.12
import QtQuick.Controls 2.12 as QtControls
import QtQuick.Dialogs 1.3

import org.kde.kirigami 2.7 as Kirigami

import org.kde.peruse 0.1 as Peruse

import "listcomponents" as ListComponents
/**
 * @brief This holds toggles and dials to configure Peruse.
 * 
 * Its main purpose is to add and remove entries from the list of booklocations.
 */
Kirigami.Page {
    id: root;
    property string categoryName: "settingsPage";
    title: i18nc("title of the settings page", "Settings");

    Item {
        width: root.width - (root.leftPadding + root.rightPadding);
        height: root.height - (root.topPadding + root.bottomPadding);
        Column {
            width: parent.width;
            height: childrenRect.height;
            clip: true;
            Item {
                width: parent.width;
                height: Kirigami.Units.largeSpacing;
            }
            Item {
                height: folderHeader.height;
                width: parent.width - folderAdd.width - Kirigami.Units.smallSpacing;
                ListComponents.Section { id: folderHeader; text: i18nc("title of the list of search folders", "Search Folders"); }
                QtControls.ToolButton {
                    id: folderAdd;
                    icon.name: "list-add";
                    display: AbstractButton.IconOnly;
                    onClicked: {
                        if(PLASMA_PLATFORM.substring(0, 5) === "phone") {
                            applicationWindow().pageStack.push(folderDlg);
                        }
                        else {
                            desktopFolderDlg.open();
                        }
                    }
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        left: parent.right;
                    }
                }
            }
            Item {
                width: parent.width;
                height: Kirigami.Units.largeSpacing;
            }
            Repeater {
                model: peruseConfig.bookLocations;
                delegate: Kirigami.SwipeListItem {
                    id: listItem;
                    actions: [
                        Kirigami.Action {
                            text: i18nc("remove the search folder from the list", "Delete");
                            iconName: "list-remove"
                            onTriggered: peruseConfig.removeBookLocation(peruseConfig.bookLocations[index]);
                        }
                    ]
                    QtControls.Label {
                        anchors {
                            verticalCenter: parent.verticalCenter;
                            left: parent.left;
                            leftMargin: Kirigami.Units.largeSpacing;
                        }
                        text: peruseConfig.bookLocations[index];
                    }
                }
            }
        }

        Rectangle {
            id: addingNewBooksProgress;
            color: Kirigami.Theme.backgroundColor;
            anchors.fill: parent;
            opacity: 0;
            Behavior on opacity { NumberAnimation { duration: applicationWindow().animationDuration; } }
            Connections {
                target: contentList.contentModel;
                onSearchCompleted: addingNewBooksProgress.opacity = 0;
            }
            enabled: opacity > 0;
            MouseArea {
                enabled: parent.enabled;
                anchors.fill: parent;
            }
            QtControls.Label {
                anchors {
                    bottom: parent.verticalCenter;
                    left: parent.left;
                    right: parent.right;
                }
                horizontalAlignment: Text.AlignHCenter;
                text: i18nc("shown with a throbber when searching for books on the device", "Please wait while we find your books...");
            }
        QtControls.BusyIndicator {
                id: loadingSpinner;
                anchors {
                    top: parent.verticalCenter;
                    left: parent.left;
                    right: parent.right;
                }
                running: addingNewBooksProgress.enabled;
            }
            QtControls.Label {
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
                id: root;
                title: "Select a folder"
                FileFinder {
                    anchors.fill: parent;
                    folder: peruseConfig.homeDir();
                    showFiles: false;
                    onAccepted: {
                        peruseConfig.addBookLocation(selectedItem());
                        applicationWindow().pageStack.pop();
                        root.doSearch();
                    }
                    onAborted: applicationWindow().pageStack.pop();
                }
            }
        }
    }

    FileDialog {
        id: desktopFolderDlg;
        title: i18nc("@title:window dialogue used to add a new search folder", "Select a Folder");
        selectFolder: true;
        folder: shortcuts.home;
        onAccepted: {
            peruseConfig.addBookLocation(desktopFolderDlg.fileUrl);
            root.doSearch();
        }
    }

    function doSearch() {
        // Now search for new items in that locations...
        var locations = peruseConfig.bookLocations;
        addingNewBooksProgress.opacity = 1;
        contentList.contentModel.startSearch();
    }
}
