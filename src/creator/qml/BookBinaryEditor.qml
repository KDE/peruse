/*
 * Copyright (C) 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
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
import QtQuick.Layouts 1.4
import QtQuick.Controls 2.12 as QtControls
import QtQuick.Dialogs 1.3

import org.kde.kirigami 2.13 as Kirigami

import org.kde.peruse 0.1 as Peruse

/**
 * @brief A sheet which lets you edit the details of a single embedded data binary
 */
Kirigami.OverlaySheet {
    id: editBinarySheet;
    property QtObject binary: null;
    function editBinary(binaryObject) {
        editBinarySheet.binary = binaryObject;
        binaryId.text = binaryObject.id;
        binaryContentType.text = binaryObject.contentType;
        open();
    }
    showCloseButton: true
    header: RowLayout {
        Kirigami.Heading {
            text: i18nc("title text for a sheet which lets the user edit a binary entry", "Edit Binary");
            Layout.fillWidth: true;
            elide: Text.ElideRight;
        }
        QtControls.ToolButton {
            icon.name: "document-save";
            text: i18nc("label for a button which updates the binary with the new details", "OK");
            onClicked: {
                editBinarySheet.binary.id = binaryId.text;
                editBinarySheet.binary.contentType = binaryContentType.text;
                editBinarySheet.close();
            }
        }
    }
    Kirigami.FormLayout {
        QtControls.TextField {
            id: binaryId;
            Layout.fillWidth: true;
            Kirigami.FormData.label: i18nc("Label for the binary ID input field", "ID");
            placeholderText: i18nc("Placeholder text for the binary ID input field", "Enter the name of your binary here (often a filename)");
        }
        QtControls.TextField {
            id: binaryContentType;
            Kirigami.FormData.label: i18nc("Label for the binary content type input field", "Content Type");
            Layout.fillWidth: true;
            placeholderText: i18nc("Placeholder text for the binary content type input field", "Enter the content type of your binary here (aka mimetype)");
        }
        RowLayout {
            Layout.fillWidth: true;
            Kirigami.FormData.label: i18nc("Label for the field which describes the data currently held by the current instance", "Current Data");
            QtControls.Label {
                id: binaryDataAmount;
                Layout.fillWidth: true;
                text: {
                    if (editBinarySheet.binary !== null && editBinarySheet.binary.size > 0) {
                        return i18nc("Label used in the binary editor sheet to describe the size of the binary data contained in the current instance", "%1 bytes", editBinarySheet.binary.size);
                    } else {
                        return i18nc("Label used in the binary editor sheet when there is no data in the current instance", "No data set");
                    }
                }
            }
            QtControls.Button {
                id: binaryDataImport;
                text: i18nc("Label for the button in the binary editor sheet which lets the user replace the data contained in the current instance", "Import Data...");
                icon.name: "document-open-data";
                property string fileName;
                function addFile() {
                    editBinarySheet.binary.contentType = peruseConfig.getFilesystemProperty(fileName, "mimetype");
                    editBinarySheet.binary.setDataFromFile(fileName);
                    binaryContentType.text = editBinarySheet.binary.contentType;
                    // Reset the ID if and only if the text field is empty, or the ID is the default identifier in the user's language
                    if (binaryId.text === "" || binaryId.text === i18nc("The initial identifier used for a newly created binary data element", "Unnamed Binary")) {
                        editBinarySheet.binary.id = fileName.split("/").pop();
                        binaryId.text = editBinarySheet.binary.id;
                    }
                    fileName = "";
                }
                onClicked: { openDlg.open(); }
                FileDialog {
                    id: openDlg;
                    title: i18nc("@title:window standard file open dialog used to add file data into the book", "Pick A File To Add");
                    folder: mainWindow.homeDir();
                    nameFilters: [
                        i18nc("The file type filter for showing all files", "All files %1", "(*)")
                    ]
                    property int splitPos: osIsWindows ? 8 : 7;
                    onAccepted: {
                        if (openDlg.fileUrl.toString().substring(0, 7) === "file://") {
                            var aOk = false;
                            binaryDataImport.fileName = openDlg.fileUrl.toString().substring(splitPos);
                            // Make sure we're not just loading ginormous files, warn after 10MiB of size
                            var byteSize = peruseConfig.getFilesystemProperty(binaryDataImport.fileName, "bytes");
                            if (byteSize > 0) {
                                aOk = true;
                            }
                            if (aOk && byteSize > 10485760) {
                                aOk = false; // let the user make the choice...
                                fileSizeOkSheet.open();
                            }
                            // Only keep going if we're all good
                            if (aOk) {
                                binaryDataImport.addFile();
                            }
                        }
                    }
                    onRejected: {
                        // Just do nothing, we don't really care...
                    }
                }
                MessageBoxSheet {
                    id: fileSizeOkSheet;
                    title: i18nc("@title:window a message box used to ask the user if they really want to add a very large file to their book", "Very Large File");
                    text: i18nc("The main query text for a message box used to ask the user if they really want to add a very large file to their book", "The file you are attempting to add, %1, is more than 10MiB. Are you sure you want to add it to the book?", binaryDataImport.fileName);
                    actions: [
                        QtControls.Action {
                            text: i18nc("The option used to let the user agree to the proposed action", "Yes, Add Large File");
                            onTriggered: { binaryDataImport.addFile(); }
                        },
                        QtControls.Action {
                            text: i18nc("The option used to let the user abort the proposed action", "Don't Add");
                            onTriggered: { binaryDataImport.fileName = ""; }
                        }
                    ]
                }
            }
        }
    }
}
