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

import QtQuick 2.2
import QtQuick.Controls 2.2 as QtControls
import QtQuick.Dialogs 1.2

import org.kde.kirigami 2.1 as Kirigami
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
            root.page.bgcolor = pageBackgroundColor.text;
            root.save();
            pageStack.pop();
        }
    }

    Column {
        id: contentColumn;
        width: root.width - (root.leftPadding + root.rightPadding);
        height: childrenRect.height;
        spacing: Kirigami.Units.smallSpacing;
        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the page title", "Title");
        }

        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextField {
            id: defaultTitle;
            width: parent.width;
            placeholderText: i18nc("placeholder text for default page text-input", "Write to add default title");
            text: root.page.title("");
            onEditingFinished: root.page.setTitle(text, "");
        }
        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the page transition type", "Transition");
        }

        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.ComboBox {
            id: transition;
            width: parent.width;
            model: root.page.availableTransitions();
            currentIndex: root.page.transition!==""?
                              root.page.availableTransitions().indexOf(root.page.transition):
                              root.page.availableTransitions().indexOf("none");
            onActivated: root.page.transition = currentText;
        }
        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the page background color", "Background Color");
        }

        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
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
                color: parent.color;
                onAccepted: root.page.bgcolor = color;
            }
        }

        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the page frames", "Frames");
        }
        ListView {
            width: parent.width;
            height: childrenRect.height;
            model: page.framePointStrings
            delegate: Kirigami.SwipeListItem {
                id: frameItem;
                height: Kirigami.Units.iconSizes.huge + Kirigami.Units.smallSpacing * 2;
                width: parent.width;
                supportsMouseEvents: true;
                actions: [
                    Kirigami.Action {
                        text: i18nc("swap the position of this frame with the previous one", "Move Up");
                        iconName: "go-up"
                        onTriggered: { page.swapFrames(index, index - 1); }
                        enabled: index > 0;
                        visible: enabled;
                    },
                    Kirigami.Action {
                        text: i18nc("swap the position of this frame with the next one", "Move Down");
                        iconName: "go-down"
                        onTriggered: { page.swapFrames(index, index + 1); }
                        enabled: index < page.framePointStrings.length - 1;
                        visible: enabled;
                    },
                    Kirigami.Action {
                        text: i18nc("remove the frame from the page", "Delete Frame");
                        iconName: "list-remove"
                        onTriggered: page.removeFrame(index);
                    }
                ]
                Item {
                    anchors.fill: parent;
                    QtControls.Label {
                        id: frameLabel;
                        text: i18nc("Comic book panel frame name.", "Frame %1", index+1);
                    }
                    Row {
                        anchors {
                            top: frameLabel.bottom;
                            topMargin: Kirigami.Units.smallSpacing;
                        }
                        spacing: Kirigami.Units.smallSpacing;
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
                                title: i18nc("@title color choosing dialog","Choose  background color for this frame");
                                color: parent.color;
                                onAccepted: page.frame(index).bgcolor = color;
                            }
                        }
                    }
                }
            }
        }
        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the page textareas", "Text Areas");
        }
        Row {
            spacing: Kirigami.Units.smallSpacing;
            QtControls.Label {
                id: textLayerBgcolorLabel;
                height: textLayerBgColor.height;
                text: i18nc("Label from textlayer background color.", "Background Color:");
            }
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
                    title: i18nc("@title color choosing dialog","Choose the background color for all textareas on this page");
                    color: parent.color;
                    onAccepted: page.textLayer("").bgcolor = color;
                }
            }
        }
        ListView {
            model: page.textLayer("").textareaPointStrings;
            width: parent.width;
            height: childrenRect.height;
            delegate: Kirigami.SwipeListItem {
                id: textAreaItem;
                height: Kirigami.Units.iconSizes.huge + Kirigami.Units.smallSpacing * 2;
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
                Item {
                    anchors.fill:parent;
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
                        text: page.textLayer("").textarea(index).paragraphs.join("\n\n");
                        onEditingFinished: page.textLayer("").textarea(index).paragraphs = text.split("\n\n");
                    }
                }
            }
        }
        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the page jumps", "Jumps");
        }
        ListView {
            model: page.jumpPointStrings
            width: parent.width;
            height: childrenRect.height;
            delegate: Kirigami.SwipeListItem {
                id: jumpItem;
                height: Kirigami.Units.iconSizes.huge + Kirigami.Units.smallSpacing * 2;
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
                        enabled: index < page.jumpPointStrings.length - 1;
                        visible: enabled;
                    },
                    Kirigami.Action {
                        text: i18nc("remove the jump from the page", "Delete Jump");
                        iconName: "list-remove"
                        onTriggered: page.removeJump(index);
                    }
                ]
                Item {
                    anchors.fill:parent;
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
                        value: page.jump(index).pageIndex;
                        onValueChanged: {
                            if (page.jump(index).pageIndex !== value) {
                                page.jump(index).pageIndex = value;
                            }
                        }
                    }
                }
            }
        }
    }
}
