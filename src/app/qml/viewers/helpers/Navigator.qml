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

import QtQuick 2.12
import org.kde.kirigami 2.7 as Kirigami

/**
 * @brief Navigation overlay for all books
 *
 * This will take taps/clicks on the left and right hand side of the viewport,
 * and detect swipes across the full width of the viewport, as well as forward
 * taps (and double taps) made in between the sides
 */

Item {
    id: component
    signal leftRequested()
    signal rightRequested()
    signal tapped(point eventPoint);
    signal doubleTapped(point eventPoint);

    Timer {
        id: swipeTimer
        interval: 300
        running: false
        repeat: false
        onTriggered: {
            swipeHandler.swipeActive = false;
//             console.debug("No swipe for you, too slow!");
        }
    }
    PointHandler {
        id: swipeHandler
        property bool swipeActive: false;
        property point pressPosition;
        property point position;
        // This is something that will definitely really want to go somewhere else... like, say, in Kirigami
        property int minimumSwipeDistance: Math.max(leftHandNav.width, component.height / 6);
        onActiveChanged: {
            if (active === true) {
                swipeActive = true;
                swipeTimer.start();
                pressPosition = point.pressPosition;
            } else if (swipeActive === true) {
                swipeTimer.stop();
                swipeActive = false;

                var swipeVector = Qt.vector2d(position.x - pressPosition.x, position.y - pressPosition.y);
                if (swipeVector.length() > minimumSwipeDistance) {
                    // determinant along the x unit vector just becomes minus swipeVector.y
                    // var determinant = swipeVector.x * xUnit.y - swipeVector.y * xUnit.x;
                    var angle = Math.atan2(-swipeVector.y, swipeVector.dotProduct(Qt.vector2d(1, 0))) * 180 / Math.PI + 180;

                    if (angle >= 315 || angle < 45) {
//                         console.debug("leftward swipe");
                        component.rightRequested();
                    } else if (angle >= 45 && angle < 135) {
//                         console.debug("downward swipe");
                        component.leftRequested();
                    } else if (angle >= 135 && angle < 225) {
//                         console.debug("rightward swipe");
                        component.leftRequested();
                    } else {
//                         console.debug("upward swipe");
                        component.rightRequested();
                    }
                }/* else {
                    console.debug("Swipe was too weak to determine anything...");
                }*/
            }
        }
        onPointChanged: {
            if (swipeActive === true) {
                position = point.position;
            }
        }
    }

    Item {
        id: leftHandNav
        anchors {
            top: parent.top;
            left: parent.left;
            bottom: parent.bottom;
        }
        width: parent.width / 6;
        TapHandler {
            onTapped: component.leftRequested();
        }
        MouseArea {
            anchors.fill: parent
            enabled: !Kirigami.Settings.tabletMode
            hoverEnabled: true;
            acceptedButtons: Qt.NoButton
            onPositionChanged: {
                var hWidth = width/2;
                var hHeight = height/2;
                var opacityX = mouse.x>hWidth? hWidth-(mouse.x-hWidth) : mouse.x;
                opacityX = opacityX/(hWidth - (Kirigami.Units.iconSizes.huge/2));
                var opacityY = mouse.y>hHeight? hHeight-(mouse.y-hHeight) : mouse.y;
                opacityY = opacityY/(hHeight - (Kirigami.Units.iconSizes.huge/2));
                leftPageIcon.opacity = opacityX*opacityY;
            }
            onExited: {
                leftPageIcon.opacity = 0;
            }
        }

        Rectangle {
            id: leftPageIcon;
            anchors.centerIn: parent;
            width: Kirigami.Units.iconSizes.huge;
            height: width;
            radius:width/2;
            color: Kirigami.Theme.highlightColor;
            opacity: 0;
            Kirigami.Icon {
                anchors.centerIn: parent;
                source: "go-previous"
                width: parent.width*(2/3);
                height: width;
            }
        }
    }
    Item {
        anchors {
            top: parent.top
            left: leftHandNav.right
            right: rightHandNav.left
            bottom: parent.bottom
        }
        TapHandler {
            onTapped: component.tapped(eventPoint.position);
            onDoubleTapped: component.doubleTapped(eventPoint.position);
        }
    }
    Item {
        id: rightHandNav
        anchors {
            top: parent.top;
            right: parent.right;
            bottom: parent.bottom;
        }
        width: parent.width / 6;
        TapHandler {
            onTapped: component.rightRequested();
        }
        MouseArea {
            anchors.fill: parent
            enabled: !Kirigami.Settings.tabletMode
            hoverEnabled: true;
            acceptedButtons: Qt.NoButton
            onPositionChanged: {
                var hWidth = width/2;
                var hHeight = height/2;
                var opacityX = mouse.x>hWidth? hWidth-(mouse.x-hWidth) : mouse.x;
                opacityX = opacityX/(hWidth - (Kirigami.Units.iconSizes.huge/2));
                var opacityY = mouse.y>hHeight? hHeight-(mouse.y-hHeight) : mouse.y;
                opacityY = opacityY/(hHeight - (Kirigami.Units.iconSizes.huge/2));
                rightPageIcon.opacity = opacityX*opacityY;
            }
            onExited: {
                rightPageIcon.opacity = 0;
            }
        }

        Rectangle {
            id: rightPageIcon;
            anchors.centerIn: parent;
            width: Kirigami.Units.iconSizes.huge;
            height: width;
            radius:width/2;
            color: Kirigami.Theme.highlightColor;
            opacity: 0;
            Kirigami.Icon {
                anchors.centerIn: parent;
                source: "go-next"
                width: parent.width*(2/3);
                height: width;
            }
        }
    }
}
