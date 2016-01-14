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
// import QtQuick.Layouts 1.1
// import QtQuick.Controls 1.0 as QtControls

// import org.kde.plasma.components 2.0 as PlasmaComponents

ListView {
    id: root

    property int imageWidth
    property int imageHeight

    orientation: ListView.Horizontal
    snapMode: ListView.SnapOneItem
    cacheBuffer: 3000

    delegate: Flickable {
        id: flick
        width: imageWidth
        height: imageHeight
        contentWidth: imageWidth
        contentHeight: imageHeight
        interactive: contentWidth > width || contentHeight > height
        onInteractiveChanged: root.interactive = !interactive;
        z: interactive ? 1000 : 0
        PinchArea {
            width: Math.max(flick.contentWidth, flick.width)
            height: Math.max(flick.contentHeight, flick.height)

            property real initialWidth
            property real initialHeight

            onPinchStarted: {
                initialWidth = flick.contentWidth
                initialHeight = flick.contentHeight
            }

            onPinchUpdated: {
                // adjust content pos due to drag
                flick.contentX += pinch.previousCenter.x - pinch.center.x
                flick.contentY += pinch.previousCenter.y - pinch.center.y

                // resize content
                flick.resizeContent(Math.max(imageWidth, initialWidth * pinch.scale), Math.max(imageHeight, initialHeight * pinch.scale), pinch.center)
            }

            onPinchFinished: {
                // Move its content within bounds.
                flick.returnToBounds();
            }


            Image {
                id: image
                width: flick.contentWidth
                height: flick.contentHeight
                source: model.url
                fillMode: Image.PreserveAspectFit
                asynchronous: true
                sourceSize.width: imageWidth * 2
                sourceSize.height: imageHeight * 2
                MouseArea {
                    anchors.fill: parent
                    onClicked: actionButton.toggleVisibility();
                    onDoubleClicked: {
                        if (flick.interactive) {
                            flick.resizeContent(imageWidth, imageHeight, {x: imageWidth/2, y: imageHeight/2});
                        } else {
                            flick.resizeContent(imageWidth * 2, imageHeight * 2, {x: mouse.x, y: mouse.y});
                        }
                    }
                }
            }
        }
    }
}
