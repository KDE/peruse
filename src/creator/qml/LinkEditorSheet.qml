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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.peruse as Peruse

/**
 * \brief A sheet used to perform inline edits on \<a href... type links in a TextArea
 * 
 * The sheet makes use of a TextDocumentEditor helper, which should be attached to the TextArea's textdocument instance
 */
FormCard.FormCardDialog {
    id: root

    property Item textField
    property QtObject editorHelper
    property QtObject model
    // The start of the entire link, including anchor markup
    property int linkStart
    // The end of the entire link, including anchor markup
    property int linkEnd

    function edit(): void {
        var linkHref = editorHelper.linkHref(textField.cursorPosition);
        if (linkHref.length > 0) {
            linkDestination.text = linkHref;
            linkText.text = editorHelper.linkText(textField.cursorPosition);
            // Select the start and end of the link we're currently sat inside
            var linkStartEnd = editorHelper.linkStartEnd(textField.cursorPosition);
            textField.select(linkStartEnd.x, linkStartEnd.y);
        } else {
            linkDestination.text = "";
            // Even if we don't have a selection, just set the text of the link to the selected
            // text (empty text if we don't have one, and we need to clear the field anyway)
            linkText.text = textField.selectedText;
        }
        // set the start and end of the link to the current selection
        root.linkStart = textField.selectionStart;
        root.linkEnd = textField.selectionEnd;
        root.open();
    }


    standardButtons: Controls.Dialog.Save | Controls.Dialog.Cancel

    title: i18nc("@title:dialog", "Edit Link");

    onAccepted: {
        var theLink = "";
        if (linkDestination.text.length > 0 && linkText.text.length > 0) {
            theLink = "<a href=\"" + linkDestination.text + "\">" + linkText.text + "</a>";
        } else if (linkDestination.text.length > 0) {
            theLink = "<a href=\"" + linkDestination.text + "\">" + linkDestination.text + "</a>";
        } else if (linkText.text.length > 0) {
            theLink = linkText.text;
        }
        textField.remove(root.linkStart, root.linkEnd);
        textField.insert(root.linkStart, theLink);
        root.close();
    }

    onRejected: root.close()

    FormCard.FormTextFieldDelegate {
        id: linkText;
        label: i18nc("@label:textfield", "Text")
    }

    FormCard.FormTextFieldDelegate {
        id: linkDestination;
        label: i18nc("@label:textfield", "Destination")
        onTextChanged: {
            // TODO reimplemt auto completion
            //if (linkDestination.text.startsWith("#")) {
            //    linkDestinationOptionsFilter.setFilterFixedString(linkDestination.text.slice(1));
            //} else {
            //    linkDestinationOptionsFilter.setFilterFixedString(linkDestination.text);
            //}
        }
    }

    /*
        ColumnLayout {
            Layout.fillWidth: true;
            Controls.TextField {
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
                    filterRole: 257 // the ID role in IdentifiedObjectModel
                    sourceModel: Peruse.IdentifiedObjectModel {
                        document: root.model.acbfData;
                    }
                }
                delegate: Controls.Label {
                    width: ListView.view.width;
                    text: {
                        switch(model.type) {
                            case Peruse.IdentifiedObjectModel.ReferenceType:
                                return i18nc("Entry in a dropdown list which gives the name of a reference object, and identifies it as one such", "%1 (Reference)", (model.id.length > 0 ? model.id : "no ID"))
                                break;
                            case Peruse.IdentifiedObjectModel.BinaryType:
                                return i18nc("Entry in a dropdown list which gives the name of a binary object, and identifies it as one such", "%1 (Binary)", (model.id.length > 0 ? model.id : "no ID"))
                                break;
                            case Peruse.IdentifiedObjectModel.TextareaType:
                                return i18nc("Entry in a dropdown list which gives the base details of a Textarea object, and identifies it as one such", "Textarea (ID: %1, text: %2)", (model.id.length > 0 ? model.id : "no ID"), model.object.paragraphs[0]);
                                break;
                            case Peruse.IdentifiedObjectModel.FrameType:
                                return i18nc("Entry in a dropdown list which gives the name of a frame object, and identifies it as one such", "%1 (Frame)", (model.id.length > 0 ? model.id : "no ID"))
                                break;
                            case Peruse.IdentifiedObjectModel.PageType:
                                return i18nc("Entry in a dropdown list which gives the title and ID of a page", "Page \"%1\" (ID: %2)", model.object.title(), (model.id.length > 0 ? model.id : "no ID"));
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
    }*/

    FormCard.FormTextDelegate {
        text: i18n("Preview")
        description: {
            if (linkDestination.text.length > 0 && linkText.text.length > 0) {
                return "<a href=\"" + linkDestination.text + "\">" + linkText.text + "</a>";
            } else if (linkDestination.text.length > 0) {
                return "<a href=\"" + linkDestination.text + "\">" + linkDestination.text + "</a>";
            } else if (linkText.text.length > 0) {
                return linkText.text;
            }
            return "";
        }
        descriptionItem.textFormat: Text.StyledText
    }
}
