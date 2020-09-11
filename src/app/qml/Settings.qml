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
import QtQuick.Controls 2.12 as QQC2
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.3
import QtQml.Models 2.3

import org.kde.kirigami 2.12 as Kirigami
import org.kde.kcm 1.2 as KCM

import org.kde.peruse 0.1 as Peruse

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

    actions.main: Kirigami.Action {
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

        Layout.fillWidth: true
        Layout.fillHeight: true
        boundsBehavior: Flickable.StopAtBounds

        clip: true

        model: DelegateModel {
            model: peruseConfig.bookLocations
            delegate: pathDelegate
        }

        QQC2.ScrollBar.vertical: QQC2.ScrollBar {
            id: scrollBar
        }

        Component {
            id: pathDelegate
            
            Kirigami.SwipeListItem {
                id: delegateItem
                QQC2.Label {
                    text: modelData
                }
                actions: [
                    Kirigami.Action {
                        text: i18nc("remove the search folder from the list", "Delete");
                        iconName: "list-remove"
                        onTriggered: peruseConfig.removeBookLocation(peruseConfig.bookLocations[index]);
                    }
                ]
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
