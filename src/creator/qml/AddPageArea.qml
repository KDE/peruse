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
import QtQuick.Controls 2.12 as QtControls
import QtQuick.Dialogs 1.3

import org.kde.kirigami 2.7 as Kirigami
/**
 * @brief a special overlay sheet for adding frames/textareas/jumps
 */
Kirigami.OverlaySheet {
    id: root;
    signal save();
    property alias type: typeComboBox.currentIndex;
    property point topLeft;
    property point bottomRight;
    property string imageSource;
    property string bgColor: "#ffffff";
    property string textBgColor: bgColor;
    property int pageIndex: pageIndexComboBox.currentIndex+1;
    property var pages: [];
    property var availableTypes: [];

    property alias inverted: invertedSwitch.checked;
    property alias transparent: transparentSwitch.checked;
    property alias paragraphs: textAreaInput.text;
    property alias rotation: textRotation.value;
    property alias textTypeIndex: textType.currentIndex;

    onBottomRightChanged: {
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
    }

    Column {
        height: childrenRect.height;
        spacing: Kirigami.Units.smallSpacing;
        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("title text for the add page area sheet", "Add Page Area");
            QtControls.Button {
                id: closeButton;
                anchors {
                    right: saveButton.left;
                    rightMargin: Kirigami.Units.smallSpacing;
                }
                contentItem: Kirigami.Icon {
                    source: "dialog-cancel";
                }
                height: parent.height;
                width: height;
                Keys.onReturnPressed: root.close();
                onClicked: root.close();
            }
            QtControls.Button {
                id: saveButton;
                anchors {
                    right: parent.right;
                    leftMargin: Kirigami.Units.smallSpacing;
                }
                contentItem: Kirigami.Icon {
                    source: "dialog-ok";
                }
                height: parent.height;
                width: height;
                Keys.onReturnPressed: saveAndClose();
                onClicked: saveAndClose();

                function saveAndClose() {
                    root.save();
                    root.close();
                }
            }
        }
        Item {
            width: parent.width;
            height: Kirigami.Units.largeSpacing;
        }

        Rectangle {
            id: clipRectangle;
            clip:true;
            width: Kirigami.Units.iconSizes.huge*3;
            height: Kirigami.Units.iconSizes.huge*3;
            anchors.horizontalCenter: parent.horizontalCenter;
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
                visible: typeComboBox.currentIndex == 1;
            }

        }

        QtControls.Label {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("label for the page area combobox", "Page area type:");
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        }
        QtControls.ComboBox {
            id: typeComboBox;
            model: [i18n("Frame"), i18n("Textarea"), i18n("Jump")]
            width: parent.width - Kirigami.Units.smallSpacing;
        }

        Item {
            width: parent.width;
            height: Kirigami.Units.largeSpacing;
        }

        Item {
            id: frameAreaOptions;
            width: parent.width - Kirigami.Units.smallSpacing;
            height: childrenRect.height;
            visible: typeComboBox.currentIndex == 0;
            Row {
                spacing: Kirigami.Units.smallSpacing;
                QtControls.Label {
                    text: i18nc("Label for background color button", "Background color:")
                    anchors.verticalCenter: parent.verticalCenter;
                }
                Rectangle {
                    id: frameBackgroundColor;
                    height: Kirigami.Units.iconSizes.medium;
                    width: Kirigami.Units.iconSizes.huge;
                    radius: 3;
                    border.color: Kirigami.Theme.disabledTextColor;
                    border.width: 1;
                    color: root.bgColor;
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
                        title: i18nc("@title color choosing dialog","Choose the background color for this frame");
                        color: root.bgColor;
                        onAccepted: root.bgColor = color;
                    }
                }
            }
        }

        Item {
            id: textAreaOptions;
            width: parent.width - Kirigami.Units.smallSpacing;
            height: childrenRect.height;
            visible: typeComboBox.currentIndex == 1;
            Column {
                width: parent.width;
                spacing: Kirigami.Units.smallSpacing;
                Row {
                    spacing: Kirigami.Units.smallSpacing;
                    height: Kirigami.Units.iconSizes.medium;
                    QtControls.Label {
                        text: i18nc("Label for background color button", "Background color:")
                        anchors.verticalCenter: parent.verticalCenter;
                    }
                    Rectangle {
                        id: textAreaBackgroundColor;
                        height: Kirigami.Units.iconSizes.medium;
                        width: Kirigami.Units.iconSizes.huge;
                        radius: 3;
                        border.color: Kirigami.Theme.disabledTextColor;
                        border.width: 1;
                        color: root.textBgColor;
                        anchors.verticalCenter: parent.verticalCenter;
                        MouseArea {
                            anchors.fill: parent;
                            onClicked: {
                                textAreaBackgroundColorDialog.open();

                            }
                            hoverEnabled: true;
                            onEntered: parent.border.color = Kirigami.Theme.buttonHoverColor;
                            onExited: parent.border.color = Kirigami.Theme.disabledTextColor;
                        }
                        ColorDialog {
                            id: textAreaBackgroundColorDialog
                            title: i18nc("@title color choosing dialog","Choose the background color for this frame");
                            color: root.textBgColor;
                            onAccepted: root.textBgColor = color;
                        }
                    }
                    QtControls.Switch {
                        id: transparentSwitch;
                        text: i18nc("Option for making the background of a text area transparent", "Transparent");
                        anchors.verticalCenter: parent.verticalCenter;
                    }
                    QtControls.Switch {
                        id: invertedSwitch;
                        text: i18nc("Option for making the background of a text area cause what is behind it to be inverted", "Inverted");
                        anchors.verticalCenter: parent.verticalCenter;
                    }

                }
                Row {
                    width: parent.width;
                    QtControls.Label {
                        id: rotationLabel;
                        text: i18nc("Label for text rotation slider", "Text rotation:")
                        anchors.verticalCenter: parent.verticalCenter;
                    }
                    spacing: Kirigami.Units.smallSpacing;
                    QtControls.Slider {
                        width: parent.width - rotationLabel.width;
                        id: textRotation;
                        from: 0
                        to: 360;
                    }
                }
                Row {
                    width: parent.width;
                    QtControls.Label {
                        id: typeLabel;
                        text: i18nc("Label for text type combobox", "Type:")
                    }
                    spacing: Kirigami.Units.smallSpacing;
                    QtControls.ComboBox {
                        id: textType;
                        width: parent.width - typeLabel.width;
                        model: root.availableTypes;
                        currentIndex: 0;
                    }
                }

                QtControls.TextArea {
                    id: textAreaInput;
                    width: parent.width;
                    placeholderText: i18nc("Place holder text for text area", "Type to input text for text area here");
                }
            }

        }

        Item {
            id: jumpAreaOptions;
            width: parent.width - Kirigami.Units.smallSpacing;
            height: childrenRect.height;
            visible: typeComboBox.currentIndex == 2;
            Row {
                spacing: Kirigami.Units.smallSpacing;
                QtControls.Label {
                    text: i18nc("Label for the dropdown which will let you pick a page a jump will send the reader to when activated", "Page Index:")
                    anchors.verticalCenter: parent.verticalCenter;
                }
                QtControls.ComboBox {
                    id: pageIndexComboBox;
                    model: root.pages;
                    anchors.verticalCenter: parent.verticalCenter;
                }
            }
        }
    }
}
