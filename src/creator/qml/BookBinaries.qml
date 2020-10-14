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

import org.kde.kirigami 2.13 as Kirigami

import org.kde.peruse 0.1 as Peruse
/**
 * @brief the page shows basic information about the book
 */
Kirigami.ScrollablePage {
    id: root;
    property string categoryName: "bookBinaries";
    property QtObject model;
    signal requestCategoryChange(string categoryName);
    title: i18nc("title of the page which lets the user manage chunks of binary data embedded in the book", "Embedded Data");
    actions {
        main: saveBookAction;
        right: addFileAction;
    }
    Kirigami.Action {
        id: saveBookAction;
        text: i18nc("Saves the book to a file on disk", "Save Book");
        iconName: "document-save";
        onTriggered: root.model.saveBook();
        enabled: root.model ? root.model.hasUnsavedChanges : false;
    }
    Kirigami.Action {
        id: addFileAction;
        text: i18nc("Lets the user pick a file to append as a binary item", "Add File...");
        iconName: "document-new";
        onTriggered: {
            var newBinary = root.model.acbfData.data.addBinary(i18nc("The initial identifier used for a newly created binary data element", "Unnamed Binary"));
            editBinarySheet.editBinary(newBinary);
        }
    }

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
                }
            }
        }
    }

    ListView {
        id: binariesList;
        Layout.fillWidth: true;
        model: root.model.acbfData.data.binaries;
        header: ColumnLayout {
            width: binariesList.width - Kirigami.Units.largeSpacing * 4;
            Item { height: Kirigami.Units.largeSpacing; Layout.fillWidth: true; }
            RowLayout {
                Layout.fillWidth: true;
                Item { height: Kirigami.Units.gridUnit; Layout.fillWidth: true; Layout.minimumWidth: Kirigami.Units.largeSpacing * 2; }
                Kirigami.AbstractCard {
                    header: Kirigami.Heading {
                        text: binariesList.count === 0
                            ? i18nc("title text for a card which informs the user there are no binary data entries, and what those are", "No Embedded Binary Data Entries")
                            : i18nc("title text for a card which informs the user what binary data is", "Embedded Binary Data");
                        Layout.fillWidth: true;
                        elide: Text.ElideRight;
                    }
                    contentItem: QtControls.Label {
                        Layout.fillWidth: true;
                        wrapMode: Text.Wrap;
                        text: binariesList.count === 0
                            ? i18nc("Help text for the binaries page, when there are no embedded binary data entries", "There is no embedded data in your book yet. You can add such data by creating a new entry, and then adding data to that entry from some existing file on your system. That data will be imported into your book, and leaves the external file otherwise untouched.")
                            : i18nc("Help text for the binaries page, when there is already data embedded in the book", "You can add new binary data entries by creating a new entry, and then adding data to that entry from some existing file on your system. That data will be imported into your book, and leaves the external file otherwise untouched.");
                    }
                }
            }
            Item { height: Kirigami.Units.largeSpacing; Layout.fillWidth: true; }
        }
        delegate: Kirigami.AbstractListItem {
            id: listItem;
            height: Kirigami.Units.iconSizes.huge + Kirigami.Units.smallSpacing * 2;
            supportsMouseEvents: true;
            onClicked: {
                editBinarySheet.editBinary(modelData);
            }
            RowLayout {
                Layout.fillWidth: true;
                Layout.fillHeight: true;
                Item {
                    Layout.fillHeight: true;
                    Layout.minimumWidth: height;
                    Layout.maximumWidth: height;
                    Kirigami.Icon {
                        id: thumbnail;
                        anchors {
                            fill: parent;
                            margins: Kirigami.Units.smallSpacing;
                        }
                        source: root.model.previewForId("#" + modelData.id);
                        placeholder: "fileview-preview";
                        fallback: "fileview-preview";
                    }
                }
                ColumnLayout {
                    Layout.fillWidth: true;
                    Layout.fillHeight: true;
                    QtControls.Label {
                        text: modelData.id === "" ? i18nc("Title used in the list of binary data when there is no id defined for that entry", "Unnamed piece of data") : modelData.id;
                        Layout.fillWidth: true;
                        Layout.fillHeight: true;
                    }
                    QtControls.Label {
                        text: i18nc("Label which describes which content type this entry is supposed to be", "Content type: %1", modelData.contentType);
                        Layout.fillWidth: true;
                        Layout.fillHeight: true;
                    }
                }
            }
        }
        Rectangle {
            id: processingBackground;
            anchors.fill: parent;
            opacity: root.model && root.model.processing ? 0.5 : 0;
            Behavior on opacity { NumberAnimation { duration: mainWindow.animationDuration; } }
            MouseArea {
                anchors.fill: parent;
                enabled: parent.opacity > 0;
                onClicked: { }
            }
        }
        QtControls.BusyIndicator {
            anchors {
                horizontalCenter: processingBackground.horizontalCenter;
                top: parent.top
                topMargin: x;
            }
            running: processingBackground.opacity > 0;
            visible: running;
        }
    }
}
