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

import QtCore
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kcmutils as KCM

import org.kde.peruse as Peruse

import "listcomponents" as ListComponents

/**
 * @brief This holds toggles and dials to configure Peruse.
 * 
 * Its main purpose is to add and remove entries from the list of booklocations.
 */
KCM.ScrollViewKCM {
    id: root;
    property string categoryName: "settingsPage";
    title: i18nc("title of the settings page", "Configure the indexed folders");

    actions: Kirigami.Action {
        id: addPathAction
        icon.name: "list-add"
        text: i18n("Add folder")
        onTriggered: {
            if (Kirigami.Settings.isMobile) {
                applicationWindow().pageStack.push(folderDlg);
            } else {
                desktopFolderDlg.open();
            }
        }
    }

    view: ListView {
        id:pathList

        clip: true

        model: DelegateModel {
            model: peruseConfig.bookLocations
            delegate: Delegates.RoundedItemDelegate {
                id: delegate

                background: null
                text: modelData

                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    QQC2.Label {
                        text: delegate.text
                        Accessible.ignored: true
                        Layout.fillWidth: true
                    }

                    QQC2.ToolButton {
                        text: i18nc("@action:button", "Delete");
                        icon.name: "list-remove-symbolic"
                        display: QQC2.ToolButton.IconOnly
                        onClicked: peruseConfig.removeBookLocation(peruseConfig.bookLocations[index]);
                    }
                }
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
            visible: pathList.count === 0
            text: i18n("There are no folder currently indexed.")

            helpfulAction: addPathAction
        }
    }

    Component {
        id: folderDlg;
        Kirigami.Page {
            id: root;
            title: i18nc("@title:window Dialogue used to add a new search folder", "Select a folder")
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

    FolderDialog {
        id: desktopFolderDlg;
        title: i18nc("@title:window dialogue used to add a new search folder", "Select a Folder");
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        onAccepted: {
            peruseConfig.addBookLocation(desktopFolderDlg.selectedFolder);
            root.doSearch();
        }
    }

    function doSearch(): void {
        // Now search for new items in that locations...
        const locations = peruseConfig.bookLocations;
        contentList.contentModel.startSearch();
    }
}
