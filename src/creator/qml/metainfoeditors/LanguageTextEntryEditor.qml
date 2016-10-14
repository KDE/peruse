/*
 * Copyright (C) 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

import org.kde.kirigami 1.0 as Kirigami
import org.kde.plasma.components 2.0 as PlasmaComponents

Item {
    id: root;
    height: childrenRect.height;

    property string title;
    property string text;
    property bool removePossible: true;
    signal saveRequested();
    signal removeRequested();

    onTextChanged: {
        if(text !== editor.text) {
            editor.text = text;
        }
    }

    Kirigami.Label {
        id: titleLabel;
        width: parent.width;
        height: paintedHeight + units.smallSpacing * 2;
        text: root.title;
        PlasmaComponents.Button {
            id: removeButton;
            anchors {
                top: parent.top;
                right: parent.right;
            }
            height: parent.height;
            width: height;
            iconName: "list-remove";
            visible: root.removePossible;
            onClicked: root.removeRequested();
        }
        PlasmaComponents.Button {
            anchors {
                top: parent.top;
                right: removeButton.visible ? removeButton.left : parent.right;
                rightMargin: removeButton.visible ? units.smallSpacing : 0;
            }
            height: parent.height;
            width: height;
            iconName: "document-save";
            opacity: editor.text !== root.text;
            Behavior on opacity { PropertyAnimation { duration: mainWindow.animationDuration; } }
            enabled: opacity > 0;
            onClicked: {
                root.text = editor.text;
                root.saveRequested();
            }
        }
    }
    PlasmaComponents.TextField {
        id: editor;
        width: parent.width;
        anchors.top: titleLabel.bottom;
    }
}
