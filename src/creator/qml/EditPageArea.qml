/*
 * Copyright (C) 2021 Mahmoud Ahmed Khalil
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

import org.kde.kirigami 2.12 as Kirigami

import org.kde.peruse 0.1 as Peruse

/**
 * @brief a special overlay sheet for editing frames/textareas/jumps
 */
Kirigami.OverlaySheet {
    id: root;

    property QtObject model;
    property QtObject objectToBeEdited;
    property int objectToBeEditedType;

    property string imageSource;
    property var pages: [];
    property var availableTypes: [];

    /**
     * editObject should be the proper way to modify particular objects
     * on a page, calling the sheet's open() won't behave as expected.
     */
    function editObject(object, type) {
        if(object && type !== -1) {
            objectToBeEdited = object;
            objectToBeEditedType = type;

            // Explicitly setting the controls
            switch(objectToBeEditedType) {
                case BookPage.FieldTypes.Frame:
                    areaId.text = objectToBeEdited.id;
                    frameBackgroundColor.color = objectToBeEdited.bgcolor;
                    break;
                case BookPage.FieldTypes.Textarea:
                    areaId.text = objectToBeEdited.id;
                    textAreaBackgroundColor.color = objectToBeEdited.bgcolor;
                    transparentSwitch.checked = objectToBeEdited.transparent;
                    invertedSwitch.checked = objectToBeEdited.inverted;
                    textRotation.value = objectToBeEdited.textRotation;
                    textType.currentIndex = availableTypes.indexOf(objectToBeEdited.type);
                    textAreaInput.text = objectToBeEdited.paragraphs.join("\n");
                    break;
                case BookPage.FieldTypes.Jump:
                    pageIndexComboBox.currentIndex = objectToBeEdited.pageIndex;
                    break;

                default:
                    return;
            }

            var topLeft = Qt.point(objectToBeEdited.bounds.x, objectToBeEdited.bounds.y);
            var bottomRight = Qt.point(objectToBeEdited.bounds.x + objectToBeEdited.bounds.width, objectToBeEdited.bounds.y + objectToBeEdited.bounds.height);
            var widthFull = Math.max(bottomRight.x, topLeft.x) - Math.min(bottomRight.x, topLeft.x);
            var heightFull = Math.max(bottomRight.y, topLeft.y) - Math.min(bottomRight.y, topLeft.y);
            var baseSize = Math.min(mainWindow.height, mainWindow.width)*0.3;

            clipRectangle.height = baseSize;
            clipRectangle.width = baseSize*(widthFull/heightFull);
            var multiplier = baseSize/heightFull;
            if (widthFull > heightFull) {
                multiplier = baseSize/widthFull;
                clipRectangle.width = baseSize;
                clipRectangle.height = baseSize*(heightFull/widthFull);
            }
            preview.width = preview.sourceSize.width*multiplier;
            preview.height = preview.sourceSize.height*multiplier;
            preview.x = 0-(topLeft.x*multiplier);
            preview.y = 0-(topLeft.y*multiplier);

            open();
        }
    }

    onSheetOpenChanged: {
        if(sheetOpen) {
            if(!objectToBeEdited || objectToBeEditedType === -1) {
                console.log("EditPageArea should be only opened via its editObject function");
                close();
            }
        } else {
            objectToBeEdited = null;
            objectToBeEditedType = -1;
        }
    }

    showCloseButton: true
    header: Kirigami.Heading {
        text: i18nc("title text for the edit page area sheet", "Edit Page Area");
        Layout.fillWidth: true
        elide: Text.ElideRight
    }

    ColumnLayout {
        Rectangle {
            id: clipRectangle;
            clip:true;
            width: Kirigami.Units.iconSizes.huge*3;
            height: Kirigami.Units.iconSizes.huge*3;
            Layout.alignment: Qt.AlignHCenter;
            Image {
                id: preview
                source: root.imageSource;
            }
            Text {
                id: textRotationGuide;
                anchors.centerIn: parent;
                height: 1;
                color: "red";
                text: i18n("Text Rotation Guide");
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                rotation: 360-textRotation.value;
                visible: objectToBeEditedType == BookPage.FieldTypes.Textarea;
            }
        }

        Kirigami.FormLayout {
            Layout.fillWidth: true;

            QtControls.TextField {
                id: areaId
                Kirigami.FormData.label: i18nc("Label for the ID input field", "ID:")
                placeholderText: i18nc("Placeholder text for the page area ID text-field", "Write to add an ID");
                visible: objectToBeEditedType == BookPage.FieldTypes.Frame || objectToBeEditedType == BookPage.FieldTypes.Textarea
            }
            Row {
                Kirigami.FormData.label: i18nc("Label for background color button", "Background color:")
                visible: objectToBeEditedType == BookPage.FieldTypes.Frame;
                spacing: Kirigami.Units.smallSpacing;
                Rectangle {
                    id: frameBackgroundColor;
                    height: Kirigami.Units.iconSizes.medium;
                    width: Kirigami.Units.iconSizes.huge;
                    radius: 3;
                    border.color: Kirigami.Theme.disabledTextColor;
                    border.width: 1;
                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            backgroundColorDialog.open();

                        }
                        hoverEnabled: true;
                        onEntered: parent.border.color = Kirigami.Theme.hoverColor;
                        onExited: parent.border.color = Kirigami.Theme.disabledTextColor;
                    }
                    ColorDialog {
                        id: backgroundColorDialog
                        title: i18nc("@title color choosing dialog","Choose the background color for this frame");
                        color: frameBackgroundColor.color;
                        onAccepted: frameBackgroundColor.color = color;
                    }
                }
            }

            Row {
                visible: objectToBeEditedType == BookPage.FieldTypes.Textarea;
                Kirigami.FormData.label: i18nc("Label for background color button", "Background color:")
                spacing: Kirigami.Units.smallSpacing;
                height: Kirigami.Units.iconSizes.medium;
                Rectangle {
                    id: textAreaBackgroundColor;
                    height: Kirigami.Units.iconSizes.medium;
                    width: Kirigami.Units.iconSizes.huge;
                    radius: 3;
                    border.color: Kirigami.Theme.disabledTextColor;
                    border.width: 1;
                    color: objectToBeEdited && objectToBeEdited.hasOwnProperty("bgcolor")? objectToBeEdited.bgcolor : "#ffffff";
                    anchors.verticalCenter: parent.verticalCenter;
                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            textAreaBackgroundColorDialog.open();

                        }
                        hoverEnabled: true;
                        onEntered: parent.border.color = Kirigami.Theme.hoverColor;
                        onExited: parent.border.color = Kirigami.Theme.disabledTextColor;
                    }
                    ColorDialog {
                        id: textAreaBackgroundColorDialog
                        title: i18nc("@title color choosing dialog","Choose the background color for this frame");
                        color: textAreaBackgroundColor.color;
                        onAccepted: textAreaBackgroundColor.color = color;
                    }
                }
            }

            QtControls.Switch {
                id: transparentSwitch;
                visible: objectToBeEditedType == BookPage.FieldTypes.Textarea;
                Kirigami.FormData.label: i18nc("Option for making the background of a text area transparent", "Transparent");
            }
            QtControls.Switch {
                id: invertedSwitch;
                visible: objectToBeEditedType == BookPage.FieldTypes.Textarea;
                Kirigami.FormData.label: i18nc("Option for making the background of a text area cause what is behind it to be inverted", "Inverted");
            }
            QtControls.Slider {
                id: textRotation;
                Kirigami.FormData.label: i18nc("Label for text rotation slider", "Text rotation:")
                visible: objectToBeEditedType == BookPage.FieldTypes.Textarea;
                from: 0
                to: 360;
            }

            QtControls.ComboBox {
                id: textType;
                Kirigami.FormData.label: i18nc("Label for text type combobox", "Type:")
                visible: objectToBeEditedType == BookPage.FieldTypes.Textarea;
                model: root.availableTypes;
                popup.z: 999; // HACK This is an absolute hack, but combos inside OverlaySheets have their popups show up underneath, because of fun z ordering stuff
            }

            Kirigami.ActionToolBar {
                visible: textAreaInput.visible
                actions: [
                    Kirigami.Action {
                        text: i18nc("Edit the link the cursor is currently positioned on (or convert the selection to a link, or add a new one if there is no selection)", "Edit Link");
                        icon.name: "edit-link"
                        onTriggered: linkDetails.edit();
                    }
                ]
            }
            QtControls.TextArea {
                id: textAreaInput;
                Layout.fillWidth: true;
                Kirigami.FormData.label: i18nc("Label for the text area body field", "Textarea Body:");
                visible: objectToBeEditedType === BookPage.FieldTypes.Textarea;
                textFormat: Qt.RichText
                wrapMode: TextEdit.Wrap
                focus: true
                selectByMouse: true
                persistentSelection: true
                Peruse.TextDocumentEditor {
                    id: textDocumentEditor;
                    textDocument: textAreaInput.textDocument;
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
                    //if (root.flickable.contentX >= rectToMakeVisible.x) {
                        //root.flickable.contentX = rectToMakeVisible.x;
                    //} else if (root.flickable.contentX + root.flickable.width <= rectToMakeVisible.x + rectToMakeVisible.width) {
                        //root.flickable.contentX = rectToMakeVisible.x + rectToMakeVisible.width - root.flickable.width;
                    //}
                    //if (root.flickable.contentY >= rectToMakeVisible.y) {
                        //root.flickable.contentY = rectToMakeVisible.y;
                    //} else if (root.flickable.contentY + root.flickable.height <= rectToMakeVisible.y + rectToMakeVisible.height) {
                        //root.flickable.contentY = rectToMakeVisible.y + rectToMakeVisible.height - root.flickable.height;
                    //}
                }
                onCursorRectangleChanged: {
                    ensureVisible(cursorRectangle);
                }
                onLinkActivated: {
                    // This is the nastiest hack... for some reason, clicking a link does not position
                    // the cursor where you clicked, but rather /after/ the link you clicked. Not helpful.
                    textAreaInput.cursorPosition = textAreaInput.cursorPosition - 1;
                    linkDetails.edit();
                }
            }

            QtControls.ComboBox {
                id: pageIndexComboBox;
                Kirigami.FormData.label: i18nc("Label for the dropdown which will let you pick a page a jump will send the reader to when activated", "Page Index:")
                visible: objectToBeEditedType == BookPage.FieldTypes.Jump;
                model: root.pages;
                popup.z: 999; // HACK This is an absolute hack, but combos inside OverlaySheets have their popups show up underneath, because of fun z ordering stuff
            }
        }

        QtControls.Button {
            id: saveButton;
            Layout.alignment: Qt.AlignRight
            text: i18nc("Button which saves the page area, and closes the dialog", "Save");
            Keys.onReturnPressed: saveAndClose();
            onClicked: saveAndClose();

            function saveAndClose() {
                if(root.objectToBeEdited) {
                    switch(root.objectToBeEditedType) {
                        case BookPage.FieldTypes.Frame:
                            root.objectToBeEdited.id = areaId.text;
                            root.objectToBeEdited.bgcolor = frameBackgroundColor.color;
                            break;

                        case BookPage.FieldTypes.Textarea:
                            root.objectToBeEdited.id = areaId.text;
                            root.objectToBeEdited.bgcolor = textAreaBackgroundColor.color;
                            root.objectToBeEdited.transparent = transparentSwitch.checked;
                            root.objectToBeEdited.inverted = invertedSwitch.checked;
                            root.objectToBeEdited.paragraphs = textDocumentEditor.paragraphs();
                            root.objectToBeEdited.type = availableTypes[textType.currentIndex];
                            root.objectToBeEdited.textRotation = textRotation.value;
                            break;

                        case BookPage.FieldTypes.Jump:
                            root.objectToBeEdited.pageIndex = pageIndexComboBox.currentIndex;
                            break;

                        default:
                            break;
                    }
                }

                root.close();
            }
        }

        LinkEditorSheet {
            id: linkDetails;
            textField: textAreaInput;
            editorHelper: textDocumentEditor;
            model: root.model;
            rootItem.z: 200
        }
    }
}
