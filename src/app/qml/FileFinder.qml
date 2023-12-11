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

import QtQuick
import QtQuick.Controls as QtControls
import Qt.labs.folderlistmodel 2.1

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

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
        height: Kirigami.Units.gridUnit * 2;
    }
    QtControls.ToolButton {
        anchors {
            top: parent.top;
            right: parent.right;
            margins: Kirigami.Units.smallSpacing;
        }
        icon.name: "dialog-ok-apply";
        display: AbstractButton.TextBesideIcon;
        onClicked: root.accepted();
    }

    ListView {
        id: folderView;
        anchors {
            top: titleContainer.bottom;
            topMargin: Kirigami.Units.largeSpacing;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        clip: true;
        model: folderModel;
        Component.onCompleted: folderView.currentIndex = -1;
        header: Delegates.RoundedItemDelegate {
            clip: true;
            width: folderView.width;
            onClicked: folderModel.folder = folderModel.parentFolder;
            text: i18nc("A fake folder item which if activated will cause the folder list to navigate up through the folder structure by one step", "(go up one level)");
            icon.name: "go-up";
        }
        delegate: Delegates.RoundedItemDelegate {
            text: fileName;
            icon.name: fileIsDir ? "folder" : "";
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
        }
    }
}
