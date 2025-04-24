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

import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.peruse as Peruse

/**
 * @brief page with a form for creating a new comic.
 * 
 * It asks for the default title, folder and cover image,
 * and when done it open the new book in Book.
 */
FormCard.FormCardPage {
    id: root

    property string categoryName: "createNewBook";

    title: i18nc("title of the new book creation page", "Create New Book");

    actions: Kirigami.Action {
        text: i18nc("Accept button which will create a new book", "Create Book");
        icon.name: "dialog-ok";
        enabled: folderField.text.length > 0
        onTriggered: {
            newBookModel.createBook(folderField.bookUrl, titleEdit.text, getCoverDlg.selectedFile)
            mainWindow.openBook(folderField.text);
        }
    }

    readonly property Peruse.ArchiveBookModel newBookModel: Peruse.ArchiveBookModel {
        id: newBookModel;
        qmlEngine: globalQmlEngine;
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit

        FormCard.FormTextFieldDelegate {
            id: titleEdit
            label: i18nc("label text for the edit field for the book title", "Title")
            text: i18nc("Default name for new books", "Untitled")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.AbstractFormDelegate {
            id: folderDelegate

            text: i18nc("label text for the edit field for the file system location for the book", "Folder")

            background: null
            contentItem: ColumnLayout {
                spacing: Kirigami.Units.smallSpacing

                QQC2.Label {
                    text: folderDelegate.text
                }

                RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    // TODO we need a QML file completion model...
                    QQC2.Label {
                        id: folderField
                        Layout.fillWidth: true
                        readonly property int splitPos: Qt.platform.os === "windows" ? 8 : 7
                        property url bookUrl: StandardPaths.standardLocations(StandardPaths.DocumentLocation)[0] + '/' + titleEdit.text + '.cbz'
                        text: bookUrl.toString().substring(splitPos)
                    }

                    QQC2.ToolButton {
                        id: getFolderButton;
                        icon.name: "folder-open-symbolic"
                        text: i18nc("@action:button", "Select Folder")
                        onClicked: getFolderDlg.open();
                        display: QQC2.Button.IconOnly

                        QQC2.ToolTip.text: text
                        QQC2.ToolTip.visible: hovered
                        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                        FileDialog {
                            id: getFolderDlg

                            fileMode: FileDialog.SaveFile
                            nameFilters: [i18nc("File filter cbz file", "*.cbz")]

                            title: i18nc("@title:window folder dialog used to select the location of a new book", "Please Choose the Location for the Book")
                            selectedFile: folderField.bookUrl
                            currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentLocation)[0]
                            onAccepted: {
                                folderField.bookUrl = selectedFile;
                            }
                        }
                    }
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.AbstractFormDelegate {
            id: coverDelegate

            text: i18nc("label text for the edit field for the cover image for the book", "Cover Image");
            background: null

            contentItem: ColumnLayout {
                spacing: Kirigami.Units.smallSpacing

                QQC2.Label {
                    text: coverDelegate.text
                    Layout.fillWidth: true
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: missingCoverPlaceholder.visible ? missingCoverPlaceholder.implicitHeight : Kirigami.Units.iconSizes.enormous + Kirigami.Units.smallSpacing;
                    Image {
                        anchors.centerIn: parent;
                        height: Kirigami.Units.iconSizes.enormous;
                        width: Kirigami.Units.iconSizes.enormous;
                        asynchronous: true;
                        fillMode: Image.PreserveAspectFit;
                        source: getCoverDlg.selectedFile;
                    }

                    Kirigami.PlaceholderMessage {
                        id: missingCoverPlaceholder

                        icon.name: "fileview-preview-symbolic"
                        text: i18nc("placeholder message for missing cover image", "Missing cover image")
                        helpfulAction: Kirigami.Action {
                            id: selectImageAction
                            text: i18nc("@action:button", "Select image")
                            icon.name: "folder-open-symbolic"
                            onTriggered: getCoverDlg.open();
                        }
                        visible: getCoverDlg.selectedFile.toString().length === 0

                        anchors.centerIn: parent
                        width: parent.width - Kirigami.Units.gridUnit * 2
                    }

                    QQC2.RoundButton {
                        id: getCoverButton;

                        anchors {
                            top: parent.top
                            right: parent.right
                            margins: Kirigami.Units.largeSpacing
                        }
                        visible: getCoverDlg.selectedFile.toString().length > 0
                        icon.name: "folder-open-symbolic"
                        action: selectImageAction

                        FileDialog {
                            id: getCoverDlg
                            title: i18nc("@title:window file dialog used to select the cover image for a new book", "Please Choose Your Cover Image")
                            currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
                            nameFilters: [
                                i18nc("File filter option for displaying only jpeg files", "JPEG images %1", "(*.jpg, *.jpeg)"),
                                i18nc("File filter option for displaying all files", "All files %1", "(*)")
                            ]
                        }
                    }
                }
            }
        }
    }
}
