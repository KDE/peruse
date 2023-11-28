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

import QtQuick 2.12
import QtQuick.Layouts 1.4
import QtQuick.Controls 2.12 as QtControls
import QtQuick.Dialogs 1.3

import org.kde.kirigami 2.13 as Kirigami
/**
 * Page that holds an image to edit the frames on.
 */
import QtQuick 2.0

Kirigami.ScrollablePage {
    id: root;
    title: i18nc("title text for the page meta information editor sheet", "Edit Page Information");
    property QtObject page;
    property string colorname: "#ffffff";
    signal save();

    actions {
        main: saveAndCloseAction;
    }
    Kirigami.Action {
        id: saveAndCloseAction;
        text: i18nc("Saves the remaining unsaved edited fields and closes the metainfo editor", "Close Editor");
        iconName: "dialog-ok";
        shortcut: "Esc";
        onTriggered: {
            root.page.setTitle(defaultTitle.text, "")
            root.save();
            pageStack.pop();
        }
    }

    Kirigami.FormLayout {
        Layout.fillWidth: true
        QtControls.TextField {
            id: defaultTitle;
            Kirigami.FormData.label: i18nc("label text for the edit field for the page title", "Title");
            placeholderText: i18nc("placeholder text for the page title text-input", "Write to add default title");
            text: root.page.title("");
            onEditingFinished: root.page.setTitle(text, "");
        }
        QtControls.TextField {
            id: pageId;
            Kirigami.FormData.label: i18nc("label text for the edit field for the page id", "ID");
            placeholderText: i18nc("placeholder text for page ID text-input", "Write to add an ID");
            text: root.page.id
            onEditingFinished: root.page.id = text
        }
        QtControls.ComboBox {
            id: transition;
            Kirigami.FormData.label: i18nc("label text for the edit field for the page transition type", "Transition");
            model: root.page.availableTransitions();
            currentIndex: root.page.transition!==""?
                              root.page.availableTransitions().indexOf(root.page.transition):
                              root.page.availableTransitions().indexOf("none");
            onActivated: root.page.transition = currentText;
        }
        Row {
            Kirigami.FormData.label: i18nc("label text for the edit field for the page background color", "Background Color");
            height: Kirigami.Units.iconSizes.medium;
            Rectangle {
                id: pageBackgroundColor;
                height: Kirigami.Units.iconSizes.medium;
                width: Kirigami.Units.iconSizes.huge;
                radius: 3;
                border.color: Kirigami.Theme.disabledTextColor;
                border.width: 1;
                color: root.page.bgcolor !== ""? root.page.bgcolor: root.colorname;
                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        backgroundColorDialog.open();

                    }
                    hoverEnabled: true;
                    onEntered: parent.border.color = Kirigami.Theme.buttonHoverColor;
                    onExited: parent.border.color = Kirigami.Theme.disabledTextColor;
                }
                ColorDialog {
                    id: backgroundColorDialog
                    title: i18nc("@title color choosing dialog","Choose the background color for page");
                    color: root.page.bgcolor !== ""? root.page.bgcolor: root.colorname;
                    onAccepted: root.page.bgcolor = color;
                }
            }
        }

        Kirigami.Separator {
            Kirigami.FormData.label: i18nc("label text for the edit field for the page frames", "Frames");
            Kirigami.FormData.isSection: true
        }

        Repeater {
            model: page.framePointStrings
            delegate: RowLayout {
                Kirigami.FormData.label: i18nc("Comic book panel frame name.", "Frame %1", index+1);
                Layout.fillWidth: true;
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
                            color: page.frame(index).bgcolor !== ""? page.frame(index).bgcolor: pageBackgroundColor.color;
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
                                color: page.frame(index).bgcolor !== ""? page.frame(index).bgcolor: pageBackgroundColor.color;
                                onAccepted: page.frame(index).bgcolor = color;
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
        Kirigami.Separator {
            Kirigami.FormData.label: i18nc("label text for the edit field for the page textareas", "Text Areas");
            Kirigami.FormData.isSection: true;
        }
        Row {
            spacing: Kirigami.Units.smallSpacing;
            Kirigami.FormData.label: i18nc("Label from textlayer background color.", "Background Color:");
            Rectangle {
                height: Kirigami.Units.iconSizes.medium;
                width: Kirigami.Units.iconSizes.huge;
                id: textLayerBgColor;
                radius: 3;
                border.color: Kirigami.Theme.disabledTextColor;
                border.width: 1;
                color: page.textLayer("").bgcolor !== ""? page.textLayer("").bgcolor: pageBackgroundColor.color;
                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        textLayerBackgroundColorDialog.open();

                    }
                    hoverEnabled: true;
                    onEntered: parent.border.color = Kirigami.Theme.buttonHoverColor;
                    onExited: parent.border.color = Kirigami.Theme.disabledTextColor;
                }
                ColorDialog {
                    id: textLayerBackgroundColorDialog
                    title: i18nc("@title color choosing dialog","Choose the background color for all text areas on this page");
                    color: page.textLayer("").bgcolor !== ""? page.textLayer("").bgcolor: pageBackgroundColor.color;
                    onAccepted: page.textLayer("").bgcolor = color;
                }
            }
        }
        Repeater {
            model: page.textLayer("").textareaPointStrings;
            delegate: Kirigami.SwipeListItem {
                Layout.fillWidth: true
                height: childrenRect.height
                supportsMouseEvents: true;
                actions: [
                    Kirigami.Action {
                        text: i18nc("swap the position of this text area with the previous one", "Move Up");
                        iconName: "go-up"
                        onTriggered: { page.textLayer("").swapTextareas(index, index - 1); }
                        enabled: index > 0;
                        visible: enabled;
                    },
                    Kirigami.Action {
                        text: i18nc("swap the position of this text area with the next one", "Move Down");
                        iconName: "go-down"
                        onTriggered: { page.textLayer("").swapTextareas(index, index + 1); }
                        enabled: index < page.textLayer("").textareaPointStrings.length - 1;
                        visible: enabled;
                    },
                    Kirigami.Action {
                        text: i18nc("remove the text area from the page", "Delete Text Area");
                        iconName: "list-remove"
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
        Kirigami.Separator {
            Kirigami.FormData.label: i18nc("label text for the edit field for the page jumps", "Jumps");
            Kirigami.FormData.isSection: true;
        }
        Repeater {
            id: jumpsRepeater;
            model: page.jumps
            delegate: Kirigami.SwipeListItem {
                Layout.fillWidth: true
                height: childrenRect.height
                supportsMouseEvents: true;
                actions: [
                    Kirigami.Action {
                        text: i18nc("swap the position of this jump with the previous one", "Move Up");
                        iconName: "go-up"
                        onTriggered: { page.swapJumps(index, index - 1); }
                        enabled: index > 0;
                        visible: enabled;
                    },
                    Kirigami.Action {
                        text: i18nc("swap the position of this jump with the next one", "Move Down");
                        iconName: "go-down"
                        onTriggered: { page.swapJumps(index, index + 1); }
                        enabled: index < jumpsRepeater.count - 1;
                        visible: enabled;
                    },
                    Kirigami.Action {
                        text: i18nc("remove the jump from the page", "Delete Jump");
                        iconName: "list-remove"
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
