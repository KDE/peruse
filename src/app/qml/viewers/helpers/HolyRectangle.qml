/*
 * Copyright (C) 2019 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

/**
 * @brief Renders a rectangle of a solid color, with a hole somewhere inside it
 *
 * The rectangle will extend past the item's borders, yielding the result of obscuring everything
 * behind it, except for the hole. The extent of the obscured area is the width and height of the
 * component instance (thus ensuring we definitely obscure the image it's put over the top of,
 * even when the area is moved into the viewport quite a way, such as is done when moving
 * through the frames of a page). To stop this behavior, set the instance's clip property to true.
 */
Item {
    id: component
    /**
     * The height of the top border (distance from the top edge to the beginning of the hole)
     */
    property int topBorder: 0
    /**
     * The width of the left hand side border (distance from the left edge to the beginning of the hole)
     */
    property int leftBorder: 0
    /**
     * The width of the right hand side border (distance from the right edge to the beginning of the hole)
     */
    property int rightBorder: 0
    /**
     * The height of the bottom border (distance from the bottom edge to the beginning of the hole)
     */
    property int bottomBorder: 0
    /**
     * The color of the rectangle, around the hole
     */
    property alias color: topRect.color

    /**
     * Whether animations are enabled for repositioning of the rectangle's hole
     */
    property bool animatePosition: true

    /**
     * Set all the values of the hole in one go, by using an inscribed rectangle.
     * It will conceptually punch a hole in HolyRect in the location and of the
     * size described by the rectangle passed to the function.
     * @param holeRect A rectangle which must fit inside HolyRect instance
     */
    function setHole(holeRect) {
        if (!isNaN(holeRect.x)) {
//             console.debug("New hole: " + holeRect);
            component.topBorder = holeRect.y;
            component.leftBorder = holeRect.x;
            component.rightBorder = component.width - (holeRect.x + holeRect.width);
            component.bottomBorder = component.height - (holeRect.y + holeRect.height);
        } else {
            component.topBorder = 0;
            component.leftBorder = 0;
            component.rightBorder = 0;
            component.bottomBorder = 0;
        }
    }

    Behavior on topBorder { enabled: animatePosition; NumberAnimation { duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
    Behavior on leftBorder { enabled: animatePosition; NumberAnimation { duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
    Behavior on rightBorder { enabled: animatePosition; NumberAnimation { duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
    Behavior on bottomBorder { enabled: animatePosition; NumberAnimation { duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
    Behavior on color { ColorAnimation { duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
    Behavior on opacity { NumberAnimation { duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }

    Rectangle {
        id: topRect
        anchors {
            top: parent.top
            topMargin: -component.height
            left: parent.left
            leftMargin: -component.leftBorder
            right: parent.right
            rightMargin: -component.rightBorder
        }
        height: component.height + component.topBorder
    }
    Rectangle {
        id: leftRect
        anchors {
            top: parent.top
            left: parent.left
            leftMargin: -component.width
            bottom: parent.bottom
        }
        width: component.width + component.leftBorder
        color: topRect.color
    }
    Rectangle {
        id: rightRect
        anchors {
            top: parent.top
            right: parent.right
            rightMargin: -component.width
            bottom: parent.bottom
        }
        width: component.width + component.rightBorder
        color: topRect.color
    }
    Rectangle {
        id: bottomRect
        anchors {
            left: parent.left
            leftMargin: -component.leftBorder
            right: parent.right
            rightMargin: -component.rightBorder
            bottom: parent.bottom
            bottomMargin: -component.height
        }
        height: component.height + component.bottomBorder
        color: topRect.color
    }
}
