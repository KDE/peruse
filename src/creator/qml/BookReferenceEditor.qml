// SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.peruse as Peruse

/**
 * @brief the page which shows the references contained within the book
 */
Kirigami.ScrollablePage {
    id: root;
    property string categoryName: "bookReferences";
    property QtObject reference;
    property QtObject model;

    function saveReference(): void {
        reference.id = referenceIdentifier.text;
        reference.language = referenceLanguage.text;
        reference.paragraphs = textDocumentEditor.paragraphs();
    }

    title: referenceLanguage.text === ""
        ? i18nc("title text for the reference editor page, when the reference has no language set", "Editing %1 (default language)", referenceIdentifier.text)
        : i18nc("title text for the reference editor page", "Editing %1 (language: %2)", referenceIdentifier.text, referenceLanguage.text);

    onReferenceChanged: {
        textArea.text = reference.paragraphs.join("\n");
        referenceIdentifier.text = root.reference.id;
        referenceLanguage.text = root.reference.language;
    }

    actions: [
        Kirigami.Action {
            id: okCloseAction;
            text: i18nc("Save the reference and close the editor", "Save and Close Editor");
            icon.name: "dialog-ok";
            onTriggered: {
                root.saveReference();
                pageStack.pop();
            }
        },
        Kirigami.Action {
            id: saveReferenceAction;
            text: i18nc("Save the reference and keep editing", "Save");
            icon.name: "document-save";
            onTriggered: {
                root.saveReference();
            }
        },
        Kirigami.Action {
            id: abortChangesAction;
            text: i18nc("Abort the changes made in the editor, and close the editor", "Abort Changes");
            icon.name: "dialog-cancel";
            onTriggered: {
                pageStack.pop();
            }
        },
        Kirigami.Action {
            text: i18nc("Opens a sheet which lets the user change the name and language of the reference", "Edit Details...");
            icon.name: "documentinfo";
            onTriggered: referenceDetails.open();
        },
        Kirigami.Action { separator: true; },
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

    FormCard.FormCardDialog {
        id: referenceDetails

        title: i18nc("@title:dialog", "Edit Reference Details")

        standardButtons: Controls.Dialog.Save | Controls.Dialog.Close

        FormCard.FormTextFieldDelegate {
            id: referenceIdentifier

            label: i18nc("@label:textfield", "Unique identifier");
        }

        FormCard.FormTextFieldDelegate {
            id: referenceLanguage
            label: i18nc("@label:textfield", "Language");
        }
    }

    LinkEditorSheet {
        id: linkDetails;
        textField: textArea;
        editorHelper: textDocumentEditor;
        model: root.model;
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

        function ensureVisible(rectToMakeVisible: var): void {
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
