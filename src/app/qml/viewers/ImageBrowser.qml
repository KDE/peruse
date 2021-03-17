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

import QtQuick 2.12
import QtQuick.Window 2.12

import org.kde.kirigami 2.13 as Kirigami

import "helpers" as Helpers

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
    signal goPage(int pageNumber);

    onWidthChanged: restorationTimer.start()
    onHeightChanged: restorationTimer.start()
    Timer {
        id: restorationTimer
        interval: 300
        running: false
        repeat: false
        onTriggered: {
            if (currentItem) {
                imageBrowser.positionViewAtIndex(imageBrowser.currentIndex, ListView.Center);
                currentItem.refocusFrame();
            }
            else {
                restorationTimer.start();
            }
        }
    }

    function navigateTo(pageNo, frameNo = -1) {
        goPage(pageNo);
        root.currentItem.currentFrame = frameNo;
    }
    
    function switchToNextJump() {
        root.currentItem.nextJump();
    }

    function activateCurrentJump() {
        root.currentItem.activateCurrentJump();
    }

    interactive: false // No interactive flicky stuff here, we'll handle that with the navigator instance
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
        opacity: ListView.isCurrentItem ? 1 : 0
        Behavior on opacity { NumberAnimation { duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
        width: imageWidth
        height: imageHeight
        contentWidth: imageWidth
        contentHeight: imageHeight
        interactive: (contentWidth > width || contentHeight > height) && (totalFrames === 0)
        z: interactive ? 1000 : 0
        property bool hasInteractiveObjects: image.frameJumps.length > 0;
        function goNextFrame() { image.nextFrame(); }
        function goPreviousFrame() { image.previousFrame(); }
        function setColouredHole(holeRect,holeColor) {
            pageHole.setHole(holeRect);
            pageHole.color = holeColor;
        }
        Timer {
            id: refocusTimer
            interval: 200
            running: false
            repeat: false
            onTriggered: {
                flick.resetHole();
                if (totalFrames > 0 && currentFrame > -1) {
                    image.focusOnFrame();
                }
            }
        }
        function refocusFrame() {
            refocusTimer.start();
        }
        function resetHole() {
            if(image.status == Image.Ready) {
                var holeColor = "transparent";
                if (image.currentPageObject !== null) {
                    holeColor = image.currentPageObject.bgcolor;
                }
                setColouredHole(image.paintedRect, holeColor);
            }
        }
        function nextJump() {
            image.nextJump();
        }
        function activateCurrentJump() {
            image.activateCurrentJump();
        }
        ListView.onIsCurrentItemChanged: resetHole();
        Connections {
            target: image
            onStatusChanged: refocusFrame();
        }
        property alias totalFrames: image.totalFrames;
        property alias currentFrame: image.currentFrame;
        pixelAligned: true
        property bool actuallyMoving: moving || xMover.running || yMover.running || widthMover.running || heightMover.running
        Behavior on contentX { NumberAnimation { id: xMover; duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
        Behavior on contentY { NumberAnimation { id: yMover; duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
        Behavior on contentWidth { NumberAnimation { id: widthMover; duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
        Behavior on contentHeight { NumberAnimation { id: heightMover; duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
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
                Helpers.HolyRectangle {
                    id: pageHole
                    anchors.fill: parent
                    color: image.currentFrameObj.bgcolor
                    visible: opacity > 0
                    opacity: image.currentFrame === -1 ? 1 : 0
                    animatePosition: false
                }
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
                property real muliplier: isTall? (paintedHeight / pixHeight): (paintedWidth / pixWidth);
                property int offsetX: (width-paintedWidth)/2;
                property int offsetY: (height-paintedHeight)/2;
                property rect paintedRect: Qt.rect(offsetX, offsetY, paintedWidth, paintedHeight);

                // This is some magic that QML Image does for us, to be helpful. It isn't very helpful to us.
                property int pixWidth: image.implicitWidth * Screen.devicePixelRatio;
                property int pixHeight: image.implicitHeight * Screen.devicePixelRatio;

                function focusOnFrame() {
                    flick.resizeContent(imageWidth, imageHeight, Qt.point(flick.contentX, flick.contentY));
                    var frameObj = image.currentFrameObj;
                    var frameBounds = frameObj.bounds;
                    var frameMultiplier = image.pixWidth/frameBounds.width * (root.imageWidth/image.paintedWidth);
                    // If it's now too large to fit inside the viewport, scale it by height instead
                    if ((frameBounds.height/frameBounds.width)*root.imageWidth > root.imageHeight) {
                        frameMultiplier = image.pixHeight/frameBounds.height * (root.imageHeight/image.paintedHeight);
                    }
//                     console.debug("Frame bounds for frame " + index + " are " + frameBounds + " with multiplier " + frameMultiplier);
//                     console.debug("Actual pixel size of image with implicit size " + image.implicitWidth + " by " + image.implicitHeight + " is " + pixWidth + " by " + pixHeight);
                    flick.resizeContent(imageWidth * frameMultiplier, imageHeight * frameMultiplier, Qt.point(flick.contentX,flick.contentY));
                    var frameRect = Qt.rect((image.muliplier * frameBounds.x) + image.offsetX
                                        , (image.muliplier * frameBounds.y) + image.offsetY
                                        , (image.muliplier * frameBounds.width)
                                        , (image.muliplier * frameBounds.height));
                    flick.contentX = frameRect.x - (flick.width-frameRect.width)/2;
                    flick.contentY = frameRect.y - (flick.height-frameRect.height)/2;
                }

                function nextFrame() {
                    if (image.totalFrames > 0 && image.currentFrame+1 < image.totalFrames) {
                        image.currentFrame++;
                    } else {
                        image.currentFrame = -1;
                        flick.returnToBounds();
                        root.goNextPage();
                        if(root.currentItem.totalFrames > 0) {
                            root.currentItem.currentFrame = 0;
                        }
                    }
                }

                function previousFrame() {
                    if (image.totalFrames > 0 && image.currentFrame-1 > -1) {
                        image.currentFrame--;
                    } else {
                        image.currentFrame = -1;
                        flick.returnToBounds();
                        root.goPreviousPage();
                        if(root.currentItem.totalFrames > 0) {
                            root.currentItem.currentFrame = root.currentItem.totalFrames - 1;
                        }
                    }
                }

                property int totalFrames: image.currentPageObject? image.currentPageObject.framePointStrings.length: 0;
                property int currentFrame: -1;
                property QtObject currentFrameObj: image.currentPageObject && image.totalFrames > 0 && image.currentFrame > -1 ? image.currentPageObject.frame(currentFrame) : noFrame;
                onCurrentFrameObjChanged: {
                    initFrame();
                    focusOnFrame(image.currentFrame);
                }
                property QtObject noFrame: QtObject {
                    property rect bounds: image.paintedRect
                    property color bgcolor: image.currentPageObject? image.currentPageObject.bgcolor: "transparent";
                }
                
                // if we're on touch screen, we set the currentJumpIndex to -1 by default
                // otherwise we set it to the first available jump on the frame
                property int currentJumpIndex: Kirigami.Settings.isMobile? -1 : 0;
                property var frameJumps: [];
                
                function initFrame() {
                    currentJumpIndex = Kirigami.Settings.isMobile? -1 : 0;
                    var newFrameJumps = [];
                    
                    if(currentFrameObj === noFrame) {
                        newFrameJumps = image.currentPageObject.jumps;
                    } else {
                        for(var i = 0; i < image.currentPageObject.jumps.length; i++) {
                            var jumpObj = image.currentPageObject.jump(i);
                            if(frameContainsJump(jumpObj)) {
                                newFrameJumps.push(jumpObj);
                            }
                        }
                    }
                    
                    frameJumps = newFrameJumps;
                }

                Repeater {
                    property QtObject textLayer: image.currentPageObject.textLayer("en")
                    model: textLayer ? textLayer.textareas : 0;
                    Helpers.TextAreaHandler {
                        model: root.model
                        multiplier: image.muliplier
                        offsetX: image.offsetX
                        offsetY: image.offsetY
                        textArea: modelData
                        enabled: image.frameContainsJump(modelData) && !flick.actuallyMoving
                        function onLinkActivated(link) {
                            // work through the various options for what a link might actually be here...
                        }
                        // hover on bin link, show name in tooltip, if image show thumbnail in tooltop, on click open in popup if we know how, offer external if we don't (maybe in that popup?)
                        // hover on ref link, show small snippet in tooltip, on click open first in popup, if already in popup, open in full display reader
                        // hover on page(/frame), tooltip shows destination in some pleasant format (if page has non-numeric title, show the title)
                        // hover on external link, show something about opening external link
                    }
                }

                /**
                 * \brief returns true if the sent jump bounds are within the image's current frame
                 * @param jumpObj - the given jump object
                 */
                function frameContainsJump(jumpObj) {
                    if(flick.ListView.isCurrentItem) {
                        if(image.currentFrameObj === noFrame) {
                            return true;
                        } 
                        
                        return jumpObj.bounds.x >= image.currentFrameObj.bounds.x && 
                                jumpObj.bounds.y >= image.currentFrameObj.bounds.y &&
                                (jumpObj.bounds.x + jumpObj.bounds.width) <= (image.currentFrameObj.bounds.x + image.currentFrameObj.bounds.width) &&
                                (jumpObj.bounds.y + jumpObj.bounds.height) <= (image.currentFrameObj.bounds.y + image.currentFrameObj.bounds.height);
                    }
                    
                    return false;
                }
                
                function nextJump() {
                    if(image.currentJumpIndex === -1 || !jumpsRepeater.itemAt(image.currentJumpIndex).hovered) {
                        image.currentJumpIndex = (image.currentJumpIndex + 1) % image.frameJumps.length;
                    }
                    //image.currentJumpIndex = image.currentJumpIndex === image.frameJumps.length - 1? 0 : ++image.currentJumpIndex;
                }

                function activateCurrentJump() {
                    if(currentJumpIndex !== -1 && jumpsRepeater.itemAt(currentJumpIndex)) {
                        jumpsRepeater.itemAt(currentJumpIndex).activated();
                    }
                }
                
                Repeater {
                    id: jumpsRepeater;
                    model: image.frameJumps;
                    
                    Helpers.JumpHandler {                        
                        jumpObject: modelData;
                        
                        offsetX: image.offsetX;
                        offsetY: image.offsetY;
                        
                        widthMultiplier: image.muliplier;
                        heightMultiplier: image.muliplier;
                        
                        focused: image.currentJumpIndex === index;
                        
                        Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; easing.type: Easing.InOutQuad; } }
                        
                        onActivated: {
                            root.navigateTo(jumpObject.pageIndex);
                        }
                        
                        onHoveredChanged: image.currentJumpIndex = (hovered ? index : -1);
                    }
                }

                Repeater {
                    model: image.currentPageObject? image.currentPageObject.framePointStrings: 0;
//                     Rectangle {
//                         id: frame;
//                         x: (image.muliplier * image.currentPageObject.frame(index).bounds.x) + image.offsetX;
//                         y: (image.muliplier * image.currentPageObject.frame(index).bounds.y) + image.offsetY;
//                         width: image.muliplier * image.currentPageObject.frame(index).bounds.width;
//                         height: image.muliplier * image.currentPageObject.frame(index).bounds.height;
//                         color: "blue";
//                         opacity: 0;
//                     }
                    Helpers.HolyRectangle {
                        anchors.fill: parent;
                        property QtObject frameObj: image.currentPageObject ? image.currentPageObject.frame(index) : noFrame;
                        property rect frameRect: Qt.rect((image.muliplier * frameObj.bounds.x) + image.offsetX,
                                            (image.muliplier * frameObj.bounds.y) + image.offsetY,
                                            (image.muliplier * frameObj.bounds.width),
                                            (image.muliplier * frameObj.bounds.height))
                        color: frameObj.bgcolor;
                        opacity: image.currentFrame === index ? 1 : 0;
                        visible: opacity > 0;
                        topBorder: frameRect.y;
                        leftBorder: frameRect.x;
                        rightBorder: width - (frameRect.x + frameRect.width);
                        bottomBorder: height - (frameRect.y + frameRect.height);
                    }
                }
                
                MouseArea {
                    anchors.fill: parent;
                    enabled: flick.interactive;
                    onClicked: startToggleControls();
                    onDoubleClicked: {
                        abortToggleControls();
                        flick.resizeContent(imageWidth, imageHeight, Qt.point(imageWidth/2, imageHeight/2));
                        flick.returnToBounds();
                    }
                }
                Kirigami.PlaceholderMessage {
                    anchors.centerIn: parent
                    width: parent.width - (Kirigami.Units.largeSpacing * 4)
                    visible: image.status === Image.Error;
                    text: i18nc("Message shown on the book reader view when there is an issue loading the image for a specific page", "Could not load the image for this page.\nThis is most commonly due to a missing image decoder (specifically, the Qt Imageformats package, which Peruse depends on for loading images), and likely a packaging error. Contact whoever you got this package from and inform them of this error.\n\nSpecifically, the image we attempted to load is called %1, and the image formats Qt is aware of are %2. If there is a mismatch there, that will be the problem.\n\nIf not, please report this bug to us, and give as much information as you can to assist us in working out what's wrong.", image.source, peruseConfig.supportedImageFormats().join(", "));
                }
            }
        }
    }

    Helpers.Navigator {
        enabled: root.currentItem ? !root.currentItem.interactive : false;
        acceptTaps: !root.currentItem.hasInteractiveObjects;
        anchors.fill: parent;
        onLeftRequested: root.layoutDirection === Qt.RightToLeft? root.goNextFrame(): root.goPreviousFrame();
        onRightRequested: root.layoutDirection === Qt.RightToLeft? root.goPreviousFrame(): root.goNextFrame();
        onTapped: startToggleControls();
        onDoubleTapped: {
            abortToggleControls();
            if (root.currentItem.totalFrames === 0) {
                root.currentItem.resizeContent(imageWidth * 2, imageHeight * 2, Qt.point(eventPoint.x, eventPoint.y));
                root.currentItem.returnToBounds();
            }
        }
    }
}
