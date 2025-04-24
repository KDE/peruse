/*
 * Copyright (C) 2018 Wolthera van Hövell tot Westerflier<griffinvalley@gmail.com>
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
import QtQuick.Controls as QtControls
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

/**
 * Page that holds an image to edit the frames on.
 */
FormCard.FormCardPage {
    id: root

    property QtObject page;
    property string colorname: "#ffffff";
    property color pageBackgroundColor: root.page.bgcolor !== "" ? root.page.bgcolor: root.colorname

    signal save();

    title: i18nc("title text for the page meta information editor sheet", "Edit Page Information")

    actions: QtControls.Action {
        id: saveAndCloseAction
        text: i18nc("Saves the remaining unsaved edited fields and closes the metainfo editor", "Close Editor")
        icon.name: "dialog-ok-symbolic"
        shortcut: "Esc"
        onTriggered: {
            root.page.setTitle(defaultTitle.text, "");
            root.save();
            pageStack.pop();
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "General")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            id: defaultTitle;
            label: i18nc("@label:textfield", "Title:")
            text: root.page.title("");
            onEditingFinished: root.page.setTitle(text, "");
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: pageId;
            label: i18nc("@label:textfield", "ID:")
            text: root.page.id
            onEditingFinished: root.page.id = text
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: transition

            text: i18nc("@label:listbox", "Transition:")
            model: root.page.availableTransitions()
            currentIndex: root.page.transition !== "" ?
                              root.page.availableTransitions().indexOf(root.page.transition):
                              root.page.availableTransitions().indexOf("none");
            onActivated: root.page.transition = currentText;
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormColorDelegate {
            id: backgroundColorDelegate

            text: i18nc("@label:chooser", "Background Color:")
            color: root.page.bgcolor !== "" ? root.page.bgcolor: root.colorname;
            onColorChanged: root.page.bgcolor = color;
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Frames");
    }

    FormCard.FormCard {
        FormCard.FormPlaceholderMessageDelegate {
            visible: frameRepeater.count === 0
            text: i18nc("@info:placeholder", "No frames present")
        }

        Repeater {
            id: frameRepeater

            model: page.framePointStrings
            delegate: RowLayout {
                Layout.fillWidth: true;

                QtControls.Label {
                    text: i18nc("Comic book panel frame name.", "Frame %1", index+1);
                }

                ColumnLayout {
                    QtControls.Switch {
                        text: i18nc("A switch which lets the user change the background colour of the page when this frame is focused", "Change page background color");
                    }
                    RowLayout {
                        QtControls.Label {
                            height: Kirigami.Units.iconSizes.medium;
                            text: i18nc("Label from frame background color.", "Background Color:");
                        }
                        Rectangle {
                            height: Kirigami.Units.iconSizes.medium;
                            width: Kirigami.Units.iconSizes.huge;
                            radius: 3;
                            border.color: Kirigami.Theme.disabledTextColor;
                            border.width: 1;
                            color: page.frame(index).bgcolor !== ""? page.frame(index).bgcolor : root.pageBackgroundColor;
                            MouseArea {
                                anchors.fill: parent;
                                onClicked: {
                                    frameBackgroundColorDialog.open();
                                }
                                hoverEnabled: true;
                                onEntered: parent.border.color = Kirigami.Theme.buttonHoverColor;
                                onExited: parent.border.color = Kirigami.Theme.disabledTextColor;
                            }
                            ColorDialog {
                                id: frameBackgroundColorDialog
                                title: i18nc("@title color choosing dialog","Choose background color for this frame");
                                selectedColor: page.frame(index).bgcolor !== ""? page.frame(index).bgcolor : root.pageBackgroundColor;
                                onAccepted: page.frame(index).bgcolor = selectedColor;
                            }
                        }
                    }
                }
                Item { height: Kirigami.Units.iconSizes.medium; Layout.fillWidth: true; }
                QtControls.ToolButton {
                    QtControls.ToolTip.delay: Kirigami.Units.toolTipDelay; QtControls.ToolTip.timeout: 5000; QtControls.ToolTip.visible: parent.visible && (Kirigami.Settings.tabletMode ? pressed : hovered) && QtControls.ToolTip.text.length > 0
                    QtControls.ToolTip.text: i18nc("swap the position of this frame with the previous one", "Move Up");
                    icon.name: "go-up"
                    display: QtControls.AbstractButton.IconOnly
                    onClicked: { page.swapFrames(index, index - 1); }
                    enabled: index > 0;
                    visible: enabled;
                }
                QtControls.ToolButton {
                    QtControls.ToolTip.delay: Kirigami.Units.toolTipDelay; QtControls.ToolTip.timeout: 5000; QtControls.ToolTip.visible: parent.visible && (Kirigami.Settings.tabletMode ? pressed : hovered) && QtControls.ToolTip.text.length > 0
                    QtControls.ToolTip.text: i18nc("swap the position of this frame with the next one", "Move Down");
                    icon.name: "go-down"
                    display: QtControls.AbstractButton.IconOnly
                    onClicked: { page.swapFrames(index, index + 1); }
                    enabled: index < page.framePointStrings.length - 1;
                    visible: enabled;
                }
                QtControls.ToolButton {
                    QtControls.ToolTip.delay: Kirigami.Units.toolTipDelay; QtControls.ToolTip.timeout: 5000; QtControls.ToolTip.visible: parent.visible && (Kirigami.Settings.tabletMode ? pressed : hovered) && QtControls.ToolTip.text.length > 0
                    QtControls.ToolTip.text: i18nc("remove the frame from the page", "Delete Frame");
                    icon.name: "list-remove"
                    display: QtControls.AbstractButton.IconOnly
                    onClicked: page.removeFrame(index);
                }
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Text Areas");
    }

    FormCard.FormCard {
        FormCard.FormColorDelegate {
            color: page.textLayer("").bgcolor !== ""? page.textLayer("").bgcolor : pageBackgroundColor
            onColorChanged: page.textLayer("").bgcolor = color
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormPlaceholderMessageDelegate {
            visible: textAreasRepeater.count === 0
            text: i18nc("@info:placeholder", "No text areas present")
        }

        Repeater {
            id: textAreasRepeater

            model: page.textLayer("").textareaPointStrings;
            delegate: Kirigami.SwipeListItem {
                Layout.fillWidth: true
                height: childrenRect.height
                supportsMouseEvents: true;
                actions: [
                    Kirigami.Action {
                        text: i18nc("swap the position of this text area with the previous one", "Move Up");
                        icon.name: "go-up"
                        onTriggered: { page.textLayer("").swapTextareas(index, index - 1); }
                        enabled: index > 0;
                        visible: enabled;
                    },
                    Kirigami.Action {
                        text: i18nc("swap the position of this text area with the next one", "Move Down");
                        icon.name: "go-down"
                        onTriggered: { page.textLayer("").swapTextareas(index, index + 1); }
                        enabled: index < page.textLayer("").textareaPointStrings.length - 1;
                        visible: enabled;
                    },
                    Kirigami.Action {
                        text: i18nc("remove the text area from the page", "Delete Text Area");
                        icon.name: "list-remove"
                        onTriggered: page.textLayer("").removeTextarea(index);
                    }
                ]
                contentItem: Item {
                    Layout.fillWidth: true;
                    Layout.fillHeight: true;
                    QtControls.Label {
                        id: textareaLabel;
                        text: i18nc("Comic book panel textarea name.", "Text Area %1", index+1);
                    }
                    QtControls.TextArea {
                        anchors {
                            top: textareaLabel.bottom;
                            topMargin: Kirigami.Units.smallSpacing;
                        }
                        width:parent.width-Kirigami.Units.iconSizes.huge;
                        wrapMode: TextEdit.Wrap
                        text: page.textLayer("").textarea(index).paragraphs.join("\n\n");
                        onEditingFinished: page.textLayer("").textarea(index).paragraphs = text.split("\n\n");
                    }
                }
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Jumps");
    }

    FormCard.FormCard {
        FormCard.FormPlaceholderMessageDelegate {
            visible: jumpsRepeater.count === 0
            text: i18nc("@info:placeholder", "No jumps present")
        }

        Repeater {
            id: jumpsRepeater
            model: page.jumps
            delegate: Kirigami.SwipeListItem {
                Layout.fillWidth: true
                height: childrenRect.height
                supportsMouseEvents: true;
                actions: [
                    Kirigami.Action {
                        text: i18nc("swap the position of this jump with the previous one", "Move Up");
                        icon.name: "go-up"
                        onTriggered: { page.swapJumps(index, index - 1); }
                        enabled: index > 0;
                        visible: enabled;
                    },
                    Kirigami.Action {
                        text: i18nc("swap the position of this jump with the next one", "Move Down");
                        icon.name: "go-down"
                        onTriggered: { page.swapJumps(index, index + 1); }
                        enabled: index < jumpsRepeater.count - 1;
                        visible: enabled;
                    },
                    Kirigami.Action {
                        text: i18nc("remove the jump from the page", "Delete Jump");
                        icon.name: "list-remove"
                        onTriggered: page.removeJump(index);
                    }
                ]
                contentItem: Item {
                    Layout.fillWidth: true;
                    Layout.fillHeight: true;
                    QtControls.Label {
                        id: jumpLabel;
                        text: i18nc("Comic book panel jump name.", "Jump %1", index+1);
                    }
                    QtControls.Label {
                        id: pageIndexLabel;
                        anchors {
                            top: jumpLabel.bottom;
                            topMargin: Kirigami.Units.smallSpacing;
                        }
                        height: jumpIndexSpin.height;
                        text: i18nc("Label from jump page index.", "Page Index:");
                    }

                    QtControls.SpinBox {
                        anchors {
                            top: jumpLabel.bottom;
                            topMargin: Kirigami.Units.smallSpacing;
                            left: pageIndexLabel.right;
                            leftMargin: Kirigami.Units.smallSpacing;
                        }
                        from: 0;
                        to: 99;
                        id: jumpIndexSpin;
                        value: modelData.pageIndex;
                        onValueChanged: {
                            if (modelData.pageIndex !== value) {
                                modelData.pageIndex = value;
                            }
                        }
                    }
                }
            }
        }
    }
}
