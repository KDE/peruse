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
import QtQuick.Layouts
import QtQuick.Controls as QtControls
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as Components

/**
 * @brief overlay with options for adding a page.
 * 
 * It is accessed from Book
 */
Components.MessageDialog {
    id: root;

    property int addPageAfter: 0;
    property QtObject model;

    title: i18nc("@title", "Add Page")

    iconName: "edit-image-face-add"

    leftPadding: 0
    rightPadding: 0

    width: Math.min(parent.width - Kirigami.Units.gridUnit * 2, Kirigami.Units.gridUnit * 20)
    height: Math.min(parent.height - Kirigami.Units.gridUnit * 2, Kirigami.Units.gridUnit * 20)

    ColumnLayout {
        spacing: 0

        QtControls.Label {
            text: i18nc("help text for the add page sheet", "Please select the method you want to add the new page. No changes will be made outside of the project by performing these actions.")
            wrapMode: Text.WordWrap

            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.gridUnit
        }

        Item {
            Layout.fillHeight: true
        }

        FormCard.FormButtonDelegate {
            icon.name: "document-open";
            text: i18nc("@action:button add a page by finding an image on the filesystem and copying it into the book", "Copy Image from Device");
            onClicked: openDlg.open();

            FileDialog {
                id: openDlg

                title: i18nc("@title:window standard file open dialog used to find a page to add to the book", "Please Choose an Image to Add");
                property int splitPos: osIsWindows ? 8 : 7;
                onAccepted: {
                    root.model.addPageFromFile(openDlg.selectedFile.toString().substring(splitPos), addPageAfter);
                    root.close();
                }
                currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            icon.name: "document-new"
            text: i18nc("@action:button add a page by creating a new image using an image editor", "Create a New Image Using an Image Editor")
            enabled: false
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            icon.name: "camera"
            text: i18nc("@action:button add a page by taking a photo with a camera", "Take a Photo and Add That")
            enabled: false
        }
    }

    footer: null
}
