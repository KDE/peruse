/*
 * Copyright (C) 2015 Vishesh Handa <vhanda@kde.org>
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

import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Window 2.1

ApplicationWindow {
    id: root
    visible: true

    property Item sidebar
    property Item mainItem

    onSidebarChanged: {
        if (sidebar) {
            sidebar.parent = sidebarArea
            sidebar.anchors.fill = sidebarArea
        }
    }

    onMainItemChanged: {
        if (mainItem) {
            mainItem.parent = mainItemArea
            mainItem.anchors.fill = mainItemArea
        }
    }

    SystemPalette { id: sysPal; }

    Item {
        id: mainItemArea
        anchors.fill: parent

        implicitHeight: root.height
        implicitWidth: root.width

        Rectangle {
            color: sysPal.dark
            anchors.fill: parent
            z: -1
        }
    }

    Item {
        id: sidebarArea
        anchors.left: parent.left
        anchors.top: parent.top

        implicitHeight: root.height
        implicitWidth: mainItemArea.width * 0.6
        width: 0

        visible: width
        Behavior on width {
            NumberAnimation { duration: 150 }
        }

        Rectangle {
            color: sysPal.window
            anchors.fill: parent
            z: -1
        }
    }

    function toggleSidebar() {
        if (sidebarArea.width)
            sidebarArea.width = 0;
        else
            sidebarArea.width = sidebarArea.implicitWidth
    }

    // The MouseArea used to hide the sidebar when clicking on the blank area
    MouseArea {
        anchors.left: sidebarArea.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        enabled: sidebarArea.width
        onClicked: toggleSidebar()
    }
}
