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

import org.kde.kirigami 2.1 as Kirigami
import QtQuick.Controls 2.2 as QtControls
/**
 * @brief a special overlay sheet for adding frames/textareas/jumps
 */
Kirigami.OverlaySheet {
    id: root;
    signal save();
    property alias type: typeComboBox.currentIndex;
    property point topLeft;
    property point bottomRight;

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

        QtControls.Label {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("label for the activity field", "Activity:");
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        }
        QtControls.ComboBox {
            id: typeComboBox;
            model: [i18n("Frame"), i18n("Textarea"), i18n("Jump")]
            width: parent.width - Kirigami.Units.smallSpacing;
        }
    }
}
