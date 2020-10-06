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
 * @brief the page which shows the references contained within the book
 */
Kirigami.ScrollablePage {
    id: root;
    property string categoryName: "bookReferences";
    title: i18nc("title text for the reference editor page", "Editing %1", (reference ? reference.id : ""));
    property QtObject reference;
    onReferenceChanged: {
        textArea.text = reference.paragraphs.join("\n");
    }

    actions {
        contextualActions: textActions;
        main: okClose;
        right: saveReference;
        left: abortChanges;
    }
    property list<QtObject> textActions: [
        Kirigami.Action {
            text: i18nc("Copies the selected text", "Copy");
            icon.name: "edit-copy";
            enabled: textArea.selectedText;
            onTriggered: textArea.copy();
        },
        Kirigami.Action {
            text: i18nc("Cuts the selected text", "Cut");
            icon.name: "edit-cut";
            enabled: textArea.selectedText;
            onTriggered: textArea.cut();
        },
        Kirigami.Action {
            text: i18nc("Pastes the clipboard contents into the current location", "Paste");
            icon.name: "edit-paste";
            enabled: textArea.canPaste;
            onTriggered: textArea.paste();
        },
        Kirigami.Action { separator: true; },
        Kirigami.Action {
            text: currentLink.existingLink ? i18nc("Edit the link the cursor is currently positioned on", "Edit Link") : i18nc("Adds a new link", "Add Link");
            icon.name: "edit-link"
            onTriggered: {
                if (currentLink.existingLink) {
                } else {
                    linkDetails.open();
                }
            }
        }
    ]
    Kirigami.Action {
        id: okClose;
        text: i18nc("Save the reference and close the editor", "Save and Close Editor");
        icon.name: "dialog-ok";
    }
    Kirigami.Action {
        id: saveReference;
        text: i18nc("Save the reference and keep editing", "Save");
        icon.name: "document-save";
    }
    Kirigami.Action {
        id: abortChanges;
        text: i18nc("Abort the changes made in the editor, and close the editor", "Abort Changes");
        icon.name: "dialog-cancel";
    }

    property QtObject currentLink: QtObject {
        property bool existingLink: false;
        property string text: "";
        property string destination: "";
    }
    Kirigami.OverlaySheet {
        id: linkDetails;
        showCloseButton: true
        header: RowLayout {
            Kirigami.Heading {
                text: i18nc("title text for a sheet which lets the user edit the parameters of a link", "Edit Link");
                Layout.fillWidth: true;
                elide: Text.ElideRight;
            }
            QtControls.ToolButton {
                icon.name: "document-save";
                text: i18nc("label for a button which updates the link in the text with the new information from the link editor", "OK");
                onClicked: {
                    // update the link object and change the document
                    linkDetails.close();
                }
            }
        }
        Kirigami.FormLayout {
            QtControls.TextField {
                id: linkText;
                Kirigami.FormData.label: i18nc("Label for the link text input field", "Text");
                placeholderText: i18nc("Placeholder text for the link text input field", "Enter the text of your link here");
            }
            QtControls.TextField {
                id: linkDestination;
                Kirigami.FormData.label: i18nc("Label for the link destination input field", "Destination");
                placeholderText: i18nc("Placeholder text for the link destination input field", "Enter the text of your link here");
            }
            QtControls.Label {
                id: linkDemonstration;
                text: "<a href=\"" + linkDestination.text + "\">" + linkText.text + "</a>";
            }
        }
    }

    QtControls.TextArea {
        id: textArea
        textFormat: Qt.RichText
        wrapMode: QtControls.TextArea.Wrap
        focus: true
        selectByMouse: true
        persistentSelection: true

        MouseArea {
            acceptedButtons: Qt.RightButton
            anchors.fill: parent
            onClicked: {
                mainWindow.contextDrawer.open()
            }
        }

        onLinkActivated: {
            // set the link details on the sheet
            linkDetails.open();
        }
    }
}
