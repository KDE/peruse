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

/**
 * @brief a special overlay sheet for adding frames/textareas/jumps
 */
Kirigami.OverlaySheet {
    id: root;
    signal accepted(int type);

    property string imageSource;

    property point topLeft;
    property point bottomRight;

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

    showCloseButton: true
    header: Kirigami.Heading {
        text: i18nc("title text for the add page area sheet", "Add Page Area");
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
        }

        RowLayout {
            QtControls.Button {
                id: createFrameButton;
                Layout.alignment: Qt.AlignLeft;
                Layout.fillWidth: true;
                text: i18nc("Button which creates a new frame, and closes the dialog", "Create Frame");
                Keys.onReturnPressed: createAndClose();
                onClicked: createAndClose();

                function createAndClose() {
                    root.accepted(BookPage.FieldTypes.Frame);
                    root.close();
                }
            }

            QtControls.Button {
                id: createTextareaButton;
                Layout.alignment: Qt.AlignCenter;
                Layout.fillWidth: true;
                text: i18nc("Button which creates new textarea, and closes the dialog", "Create Textarea");
                Keys.onReturnPressed: createAndClose();
                onClicked: createAndClose();

                function createAndClose() {
                    root.accepted(BookPage.FieldTypes.Textarea);
                    root.close();
                }
            }

            QtControls.Button {
                id: createJumpButton;
                Layout.alignment: Qt.AlignRight;
                Layout.fillWidth: true;
                text: i18nc("Button which creates a new jump, and closes the dialog", "Create Jump");
                Keys.onReturnPressed: createAndClose();
                onClicked: createAndClose();

                function createAndClose() {
                    root.accepted(BookPage.FieldTypes.Jump);
                    root.close();
                }
            }
        }
    }
}
