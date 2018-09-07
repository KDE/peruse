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

import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2 as QtControls

import org.kde.kirigami 2.1 as Kirigami
import org.kde.okular 2.0 as Okular
/**
 * @brief a ViewerBase intended as a fallback for unsupported books.
 * 
 * It is called from Book when the opened book has no other specialised viewers.
 * 
 * It does not use the ImageBrowser because it needs to access
 * Okular Page items for the images.
 */
ViewerBase {
    id: root;
    property string title: documentItem.windowTitleForDocument;
    onFileChanged: documentItem.path = file;
    onCurrentPageChanged: {
        if(documentItem.currentPage !== currentPage) {
            documentItem.currentPage = currentPage;
        }
        if(currentPage !== imageBrowser.currentIndex) {
            pageChangeAnimation.running = false;
            var currentPos = imageBrowser.contentX;
            var newPos;
            imageBrowser.positionViewAtIndex(currentPage, ListView.Center);
            imageBrowser.currentIndex = currentPage;
            newPos = imageBrowser.contentX;
            pageChangeAnimation.from = currentPos;
            pageChangeAnimation.to = newPos;
            pageChangeAnimation.running = true;
        }
    }
    NumberAnimation { id: pageChangeAnimation; target: imageBrowser; property: "contentX"; duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; }
    onRtlModeChanged: {
        if(rtlMode === true) {
            imageBrowser.layoutDirection = Qt.RightToLeft;
        }
        else {
            imageBrowser.layoutDirection = Qt.LeftToRight;
        }
        root.restoreCurrentPage();
    }
    onRestoreCurrentPage: {
        // This is un-pretty, quite obviously. But thanks to the ListView's inability to
        // stay in place when the geometry changes, well, this makes things simple.
        imageBrowser.positionViewAtIndex(imageBrowser.currentIndex, ListView.Center);

    }
    pageCount: documentItem.pageCount;
    thumbnailComponent: thumbnailComponent;
    pagesModel: documentItem.matchingPages;
    Component {
        id: thumbnailComponent;
        Item {
            width: parent.width;
            height: Kirigami.Units.gridUnit * 6;
            MouseArea {
                anchors.fill: parent;
                onClicked: viewLoader.item.currentPage = model.index;
            }
            Rectangle {
                anchors.fill: parent;
                color: Kirigami.Theme.highlightColor;
                opacity: root.currentPage === model.index ? 1 : 0;
                Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
            }
            Okular.ThumbnailItem {
                id: thumbnail
                anchors {
                    top: parent.top;
                    horizontalCenter: parent.horizontalCenter;
                    margins: Kirigami.Units.smallSpacing;
                }
                document: documentItem
                pageNumber: modelData
                height: parent.height - pageTitle.height - Kirigami.Units.smallSpacing * 2;
                function updateWidth() {
                    width = Math.round(height * (implicitWidth / implicitHeight));
                }
                Component.onCompleted: updateWidth();
                onHeightChanged: updateWidth();
                onImplicitHeightChanged: updateWidth();
            }
            QtControls.Label {
                id: pageTitle;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                }
                height: paintedHeight;
                text: modelData + 1;
                elide: Text.ElideMiddle;
                horizontalAlignment: Text.AlignHCenter;
            }
        }
    }
    Okular.DocumentItem {
        id: documentItem
//         onWindowTitleForDocumentChanged: {
//             fileBrowserRoot.title = windowTitleForDocument
//         }
        onOpenedChanged: {
            if(opened === true) {
                root.loadingCompleted(true);
                initialPageChange.start();
            }
        }
        onCurrentPageChanged: {
            if(root.currentPage !== currentPage) {
                root.currentPage = currentPage;
            }
        }
    }

    Timer {
        id: initialPageChange;
        interval: applicationWindow().animationDuration;
        running: false;
        repeat: false;
        onTriggered: root.currentPage = imageBrowser.model.currentIndex;
    }
    ListView {
        id: imageBrowser
        anchors.fill: parent;
        model: documentItem.matchingPages;

        property int imageWidth: root.width + Kirigami.Units.largeSpacing;
        property int imageHeight: root.height;

        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem

        // This ensures that the current index is always up to date, which we need to ensure we can track the current page
        // as required by the thumbnail navigator, and the resume-reading-from functionality
        onMovementEnded: {
            var indexHere = indexAt(contentX + width / 2, contentY + height / 2);
            if(currentIndex !== indexHere) {
                currentIndex = indexHere;
            }
        }

        delegate: Flickable {
            id: flick
            width: imageBrowser.imageWidth
            height: imageBrowser.imageHeight
            contentWidth: imageBrowser.imageWidth
            contentHeight: imageBrowser.imageHeight
            interactive: contentWidth > width || contentHeight > height
            onInteractiveChanged: imageBrowser.interactive = !interactive;
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
                    flick.resizeContent(Math.max(imageBrowser.imageWidth, initialWidth * pinch.scale), Math.max(imageBrowser.imageHeight, initialHeight * pinch.scale), pinch.center)
                }

                onPinchFinished: {
                    // Move its content within bounds.
                    flick.returnToBounds();
                }

                Item {
                    Okular.PageItem {
                        id: page;
                        document: documentItem;
                        pageNumber: index;
                        anchors.centerIn: parent;
                        property real pageRatio: implicitWidth / implicitHeight
                        property bool sameOrientation: root.width / root.height > pageRatio
                        width: sameOrientation ? parent.height * pageRatio : parent.width
                        height: !sameOrientation ? parent.width / pageRatio : parent.height
                    }
                    implicitWidth: page.implicitWidth
                    implicitHeight: page.implicitHeight
                    width: flick.contentWidth
                    height: flick.contentHeight
                    MouseArea {
                        anchors.fill: parent
                        onClicked: startToggleControls();
                        onDoubleClicked: {
                            abortToggleControls();
                            if (flick.interactive) {
                                flick.resizeContent(imageBrowser.imageWidth, imageBrowser.imageHeight, {x: imageBrowser.imageWidth/2, y: imageBrowser.imageHeight/2});
                            } else {
                                flick.resizeContent(imageBrowser.imageWidth * 2, imageBrowser.imageHeight * 2, {x: mouse.x, y: mouse.y});
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
        onClicked: root.goPreviousPage();
    }
    MouseArea {
        anchors {
            top: parent.top;
            right: parent.right;
            bottom: parent.bottom;
        }
        width: parent.width / 6;
        onClicked: root.goNextPage();
    }
}
