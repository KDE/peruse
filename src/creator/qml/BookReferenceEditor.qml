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
    property QtObject model;
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
            text: i18nc("Edit the link the cursor is currently positioned on (or convert the selection to a link, or add a new one if there is no selection)", "Edit Link");
            icon.name: "edit-link"
            onTriggered: linkDetails.edit();
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

    Kirigami.OverlaySheet {
        id: linkDetails;
        showCloseButton: true
        function edit() {
            var linkHref = textDocumentEditor.linkHref(textArea.cursorPosition);
            if (linkHref.length > 0) {
                linkDestination.text = linkHref;
                linkText.text = textDocumentEditor.linkText(textArea.cursorPosition);
                // Select the start and end of the link we're currently sat inside
                var linkStartEnd = textDocumentEditor.linkStartEnd(textArea.cursorPosition);
                textArea.select(linkStartEnd.x, linkStartEnd.y);
            } else {
                linkDestination.text = "";
                // Even if we don't have a selection, just set the text of the link to the selected
                // text (empty text if we don't have one, and we need to clear the field anyway)
                linkText.text = textArea.selectedText;
            }
            // set the start and end of the link to the current selection
            linkDetails.linkStart = textArea.selectionStart;
            linkDetails.linkEnd = textArea.selectionEnd;
            linkDetails.open();
        }
        // The start of the entire link, including anchor markup
        property int linkStart;
        // The end of the entire link, including anchor markup
        property int linkEnd;

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
                    textArea.remove(linkDetails.linkStart, linkDetails.linkEnd);
                    textArea.insert(linkDetails.linkStart, linkDemonstration.text);
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
            ColumnLayout {
                Kirigami.FormData.label: i18nc("Label for the link destination input field", "Destination");
                Layout.fillWidth: true;
                QtControls.TextField {
                    id: linkDestination;
                    Layout.fillWidth: true;
                    placeholderText: i18nc("Placeholder text for the link destination input field", "Enter the destination for your link here");
                }
                ListView {
                    Layout.fillWidth: true;
                    Layout.minimumHeight: Math.min(count, 10) * Kirigami.Units.gridUnit + Kirigami.Units.smallSpacing;
                    Layout.maximumHeight: Layout.minimumHeight;
                    clip: true;
                    model: Peruse.FilterProxy {
                        id: linkDestinationOptionsFilter;
                        sourceModel: Peruse.IdentifiedObjectModel {
                            document: root.model.acbfData;
                        }
                    }
                    delegate: QtControls.Label {
                        width: ListView.view.width;
                        text: {
                            switch(model.type) {
                                case Peruse.IdentifiedObjectModel.ReferenceType:
                                    return i18nc("Entry in a dropdown list which gives the name of a reference, and identifies it as one such", "%1 (Reference)", model.id)
                                    break;
                                case Peruse.IdentifiedObjectModel.BinaryType:
                                    return i18nc("Entry in a dropdown list which gives the name of a reference, and identifies it as one such", "%1 (Binary)", model.id)
                                    break;
                                case Peruse.IdentifiedObjectModel.UnknownType:
                                default:
                                    return i18nc("Entry in a dropdown list which gives the name of an identified object of an unknown type, and marks it as one such", "%1 (Unknown Type)", model.id)
                                    break;
                            }
                        }
                        elide: Text.ElideRight;
                    }
                }
            }
            QtControls.Label {
                id: linkDemonstration;
                Kirigami.FormData.label: i18nc("Label for the link demonstration display field", "Demonstration");
                text: {
                    if (linkDestination.text.length > 0 && linkText.text.length > 0) {
                        return "<a href=\"" + linkDestination.text + "\">" + linkText.text + "</a>";
                    } else if (linkDestination.text.length > 0) {
                        return "<a href=\"" + linkDestination.text + "\">" + linkDestination.text + "</a>";
                    } else if (linkText.text.length > 0) {
                        return linkText.text;
                    }
                    return "";
                }
            }
        }
    }

    TextEdit {
        id: textArea
        textFormat: Qt.RichText
        wrapMode: TextEdit.Wrap
        focus: true
        selectByMouse: true
        persistentSelection: true

        Peruse.TextDocumentEditor {
            id: textDocumentEditor;
            textDocument: textArea.textDocument;
        }
        MouseArea {
            acceptedButtons: Qt.RightButton
            anchors.fill: parent
            onClicked: {
                mainWindow.contextDrawer.open()
            }
        }

        function ensureVisible(rectToMakeVisible)
        {
            if (root.flickable.contentX >= rectToMakeVisible.x) {
                root.flickable.contentX = rectToMakeVisible.x;
            } else if (root.flickable.contentX + root.flickable.width <= rectToMakeVisible.x + rectToMakeVisible.width) {
                root.flickable.contentX = rectToMakeVisible.x + rectToMakeVisible.width - root.flickable.width;
            }
            if (root.flickable.contentY >= rectToMakeVisible.y) {
                root.flickable.contentY = rectToMakeVisible.y;
            } else if (root.flickable.contentY + root.flickable.height <= rectToMakeVisible.y + rectToMakeVisible.height) {
                root.flickable.contentY = rectToMakeVisible.y + rectToMakeVisible.height - root.flickable.height;
            }
        }
        onCursorRectangleChanged: {
            ensureVisible(cursorRectangle);
        }
        onLinkActivated: {
            // This is the nastiest hack... for some reason, clicking a link does not position
            // the cursor where you clicked, but rather /after/ the link you clicked. Not helpful.
            textArea.cursorPosition = textArea.cursorPosition - 1;
            linkDetails.edit();
        }
    }
}
