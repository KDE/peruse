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
import org.kde.kirigami 2.1 as Kirigami

/**
 * @brief The image viewer used by the CBR and Folder Viewer Base classes.
 * 
 * It handles drawing the image and the different zoom modes.
 */
ListView {
    id: root
    function goNextFrame() { root.currentItem.goNextFrame(); }
    function goPreviousFrame() { root.currentItem.goPreviousFrame(); }
    signal goNextPage();
    signal goPreviousPage();

    property int imageWidth
    property int imageHeight

    orientation: ListView.Horizontal
    snapMode: ListView.SnapOneItem
    cacheBuffer: 3000

    // This ensures that the current index is always up to date, which we need to ensure we can track the current page
    // as required by the thumbnail navigator, and the resume-reading-from functionality
    onMovementEnded: {
        var indexHere = indexAt(contentX + width / 2, contentY + height / 2);
        if(currentIndex !== indexHere) {
            currentIndex = indexHere;
        }
    }
    /**
     * An interactive area with an image.
     * 
     * Clicking once on the image will hide all other controls from view.
     * Clicking twice will instead zoom in.
     * 
     * Pinch will zoom in as well.
     */
    delegate: Flickable {
        id: flick
        width: imageWidth
        height: imageHeight
        contentWidth: imageWidth
        contentHeight: imageHeight
        interactive: contentWidth > width || contentHeight > height
        onInteractiveChanged: root.interactive = !interactive;
        z: interactive ? 1000 : 0
        function goNextFrame() { image.nextFrame(); }
        function goPreviousFrame() { image.previousFrame(); }
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
                property bool shouldCheat: imageWidth * 2 > maxTextureSize || imageHeight * 2 > maxTextureSize;
                property bool isTall: imageHeight < imageWidth;
                property int fixedWidth: isTall ? maxTextureSize * (imageWidth / imageHeight) : maxTextureSize;
                property int fixedHeight: isTall ? maxTextureSize : maxTextureSize * (imageHeight / imageWidth);
                sourceSize.width: shouldCheat ? fixedWidth : imageWidth * 2;
                sourceSize.height: shouldCheat ? fixedHeight : imageHeight * 2;
                MouseArea {
                    anchors.fill: parent
                    onClicked: startToggleControls();
                    onDoubleClicked: {
                        abortToggleControls();
                        if (flick.interactive) {
                            flick.resizeContent(imageWidth, imageHeight, Qt.point(imageWidth/2, imageHeight/2));
                        } else {
                            flick.resizeContent(imageWidth * 2, imageHeight * 2, Qt.point(mouse.x, mouse.y));
                        }
                    }
                }

                // Setup for all the entries.
                property QtObject currentPageObject: {
                    if (root.model.acbfData) {
                        if (model.index===0) {
                            currentPageObject = root.model.acbfData.metaData.bookInfo.coverpage();
                        } else if (model.index > 0) {
                            currentPageObject = root.model.acbfData.body.page(model.index-1);
                        }
                    } else {
                        null;
                    }
                }
                property real muliplier: isTall? (paintedHeight / implicitHeight): (paintedWidth / implicitWidth);
                property int offsetX: (width-paintedWidth)/2;
                property int offsetY: (height-paintedHeight)/2;

                function focusOnFrame(index) {
                    if (index>-1) {
                        var frameBounds = image.currentPageObject.frame(index).bounds;
                        var frameMultiplier = image.implicitWidth/frameBounds.width;
                        //Check if the height of the final frame is higher than the contentHeight
                        //When we're using a *fit to with scheme.
                        if ((frameBounds.height/frameBounds.width)*contentWidth > contentHeight) {
                            frameMultiplier = image.implicitHeight/frameBounds.height;
                            frameMultiplier = frameMultiplier * (contentHeight/image.paintedHeight);
                        } else {
                            frameMultiplier = frameMultiplier * (contentWidth/image.paintedWidth);
                        }
                        flick.resizeContent(imageWidth * frameMultiplier, imageHeight * frameMultiplier, Qt.point(0,0));
                        var frameRect = Qt.rect(image.muliplier * frameBounds.x + image.offsetX,
                                                image.muliplier * frameBounds.y+ image.offsetY,
                                                image.muliplier * frameBounds.width,
                                                image.muliplier * frameBounds.height);
                        flick.contentX = frameRect.x - (flick.width-frameRect.width)/2;
                        flick.contentY = frameRect.y - (flick.height-frameRect.height)/2;
                    }
                }

                function nextFrame() {
                    if (image.totalFrames > 0 && image.currentFrame+1 < image.totalFrames) {
                        image.currentFrame++;
                    } else {
                        flick.resizeContent(imageWidth, imageHeight, Qt.point(imageWidth/2, imageHeight/2));
                        image.currentFrame = -1;
                        flick.returnToBounds();
                        root.goNextPage();
                    }
                }

                function previousFrame() {
                    if (image.totalFrames > 0 && image.currentFrame-1 > -1) {
                        image.currentFrame--;
                    } else {
                        flick.resizeContent(imageWidth, imageHeight, Qt.point(imageWidth/2, imageHeight/2));
                        image.currentFrame = -1;
                        flick.returnToBounds();
                        root.goPreviousPage();
                    }
                }

                property int totalFrames: image.currentPageObject? image.currentPageObject.framePointStrings.length: 0;
                property int currentFrame: -1;
                onCurrentFrameChanged: focusOnFrame(currentFrame);

                Repeater {
                    model: image.currentPageObject? image.currentPageObject.framePointStrings: 0;
                    Rectangle {
                        id: frame;
                        x: image.muliplier * image.currentPageObject.frame(index).bounds.x + image.offsetX;
                        y: image.muliplier * image.currentPageObject.frame(index).bounds.y + image.offsetY;
                        width: {
                            image.muliplier * image.currentPageObject.frame(index).bounds.width;
                        }
                        height: image.muliplier * image.currentPageObject.frame(index).bounds.height;
                        color: "blue";
                        opacity: 0;
                        MouseArea {
                            anchors.fill: parent;
                            onClicked: startToggleControls();
                            preventStealing: true;
                            onDoubleClicked: {
                                abortToggleControls();
                                if (flick.interactive && image.currentFrame == index) {
                                    flick.resizeContent(imageWidth, imageHeight, Qt.point(imageWidth/2, imageHeight/2));
                                    image.currentFrame = -1;
                                } else {
                                    image.currentFrame = index;
                                    mouse.accepted;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    MouseArea {
        anchors {
            top: parent.top;
            left: parent.left;
            bottom: parent.bottom;
        }
        width: parent.width / 6;
        preventStealing: true;
        onClicked: root.layoutDirection === Qt.RightToLeft? root.goNextFrame(): root.goPreviousFrame();
        hoverEnabled: true;

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
    MouseArea {
        anchors {
            top: parent.top;
            right: parent.right;
            bottom: parent.bottom;
        }
        width: parent.width / 6;
        preventStealing: true;
        onClicked: root.layoutDirection === Qt.RightToLeft? root.goPreviousFrame(): root.goNextFrame();
        hoverEnabled: true;
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
