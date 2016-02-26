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
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import Qt.labs.folderlistmodel 2.1

import org.kde.plasma.mobilecomponents 0.2 as MobileComponents
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

import "listcomponents" as ListComponents

Item {
    id: root;
    signal accepted();
    signal aborted();
    function selectedItem() {
        var theItem = folderModel.folder;
        if(folderView.currentIndex > -1)
        {
            theItem = theItem + "/" + folderModel.get(folderView.currentIndex, "fileName");
        }
        return theItem;
    }
    property alias folder: folderModel.folder;
    property alias showDirs: folderModel.showDirs;
    property alias showFiles: folderModel.showFiles;
    property alias nameFilters: folderModel.nameFilters;

    FolderListModel {
        id: folderModel
    }

    ListComponents.ListPageHeader {
        id: titleContainer;
        text: folderModel.folder;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        clip: true;
        height: units.gridUnit * 2;
    }
    ToolButton {
        anchors {
            top: parent.top;
            right: parent.right;
            margins: units.smallSpacing;
        }
        iconName: "dialog-ok-apply";
        onClicked: root.accepted();
    }

    ListView {
        id: folderView;
        anchors {
            top: titleContainer.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        clip: true;
        model: folderModel;
        Component.onCompleted: folderView.currentIndex = -1;
        header: MobileComponents.ListItem {
            enabled: true;
            clip: true;
            onClicked: folderModel.folder = folderModel.parentFolder;
            MobileComponents.Label {
                anchors {
                    left: parent.left;
                    leftMargin: units.largeSpacing;
                    top: parent.top;
                }
                text: "(go up one level)";
                height: paintedHeight + units.largeSpacing * 2;
                width: root.width - units.largeSpacing * 2;
                verticalAlignment: Text.AlignVCenter;
            }
        }
        delegate: MobileComponents.ListItem {
            enabled: true;
            onClicked: {
                if(fileIsDir) {
                    folderView.currentIndex = -1;
                    folderModel.folder = fileURL;
                }
                else {
                    folderView.currentIndex = index;
                }
            }
            checked: folderView.currentIndex === index;
            MobileComponents.Label {
                anchors {
                    left: parent.left;
                    leftMargin: units.largeSpacing;
                    top: parent.top;
                }
                text: fileName;
                height: paintedHeight + units.largeSpacing * 2;
                width: root.width - units.largeSpacing * 2;
                elide: Text.ElideMiddle;
                verticalAlignment: Text.AlignVCenter;
            }
        }
    }
}
