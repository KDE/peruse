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

import QtQuick 2.3

/**
 * @brief Renders a rectangle of a solid color, with a hole somewhere inside it
 */
Item {
    id: component
    /**
     * The height of the top border (distance from the top edge to the beginning of the hole)
     */
    property alias topBorder: topRect.height
    /**
     * The width of the left hand side border (distance from the left edge to the beginning of the hole)
     */
    property alias leftBorder: leftRect.width
    /**
     * The width of the right hand side border (distance from the right edge to the beginning of the hole)
     */
    property alias rightBorder: rightRect.width
    /**
     * The height of the bottom border (distance from the bottom edge to the beginning of the hole)
     */
    property alias bottomBorder: bottomRect.height
    /**
     * The color of the rectangle, around the hole
     */
    property alias color: topRect.color

    function setHole(holeRect) {
        component.topBorder = holeRect.y;
        component.leftBorder = holeRect.x;
        component.rightBorder = component.width - (holeRect.x + holeRect.width);
        component.bottomBorder = component.height - (holeRect.y + holeRect.height);
    }

    Rectangle {
        id: topRect
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
    }
    Rectangle {
        id: leftRect
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }
        color: topRect.color
    }
    Rectangle {
        id: rightRect
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }
        color: topRect.color
    }
    Rectangle {
        id: bottomRect
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        color: topRect.color
    }
}
