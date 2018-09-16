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

import org.kde.kirigami 2.1 as Kirigami
/**
 * Page that holds an image to edit the frames on.
 */
Kirigami.Page {
    id: root;
    property string categoryName: "bookPage";
    title: i18nc("title of the page editing sub-page for the book editor", "Page %1", root.pageTitle === "" ? root.index : root.pageTitle);
    property QtObject model;
    property QtObject currentPage;
    property int index: -1;
    property string pageUrl: "";
    property string pageTitle: "";
    signal save();


    onIndexChanged: {
        if (root.index===0) {
            root.currentPage = root.model.acbfData.metaData.bookInfo.coverpage();
        } else if (root.index > 0) {
            root.currentPage = root.model.acbfData.body.page(root.index-1);
        }
        root.pageTitle = root.currentPage.title("");
        // Let's ensure there's always a default text-layer.
        if (root.currentPage.textLayerLanguages.length === 0) {
            root.currentPage.addTextLayer("")
        } else if (root.currentPage.textLayerLanguages.indexOf("") < 0) {
            root.currentPage.duplicateTextLayer(root.currentPage.textLayerLanguages[1], "");
        }
    }

    actions {
        main: saveAndCloseAction;
        right: editPageDataAction;
    }
    Kirigami.Action {
        id: saveAndCloseAction;
        text: i18nc("Saves the remaining unsaved edited fields and closes the page editor", "Save and Close Page");
        iconName: "dialog-ok";
        onTriggered: {
            root.save();
            root.model.setDirty();
            pageStack.pop();
        }
    }
    Kirigami.Action {
        id: editPageDataAction;
        text: i18nc("Saves the remaining unsaved edited fields and closes the page editor", "Save and Close Page");
        iconName: "document-edit"
        onTriggered: pageStack.push(pageInfo)
    }

    Image {
        id: coverImage;
        anchors {
            fill: parent;
            margins: Kirigami.Units.smallSpacing;
        }
        asynchronous: true;
        fillMode: Image.PreserveAspectFit;
        source: pageUrl;
        width: root.width;
        height: root.height;
        property real muliplierWidth: (paintedWidth / sourceSize.width);
        property real muliplierHeight: (paintedHeight / sourceSize.height);
        property int offsetX: (width-paintedWidth)/2
        property int offsetY: (height-paintedHeight)/2

        Repeater {
            model: root.currentPage.frameCount

            Rectangle {
                width: coverImage.muliplierWidth * root.currentPage.frame(index).bounds.width;
                height: coverImage.muliplierHeight * root.currentPage.frame(index).bounds.height;
                x: coverImage.muliplierWidth * root.currentPage.frame(index).bounds.x + coverImage.offsetX
                y: coverImage.muliplierHeight * root.currentPage.frame(index).bounds.y + coverImage.offsetY;
                opacity: 0.2;
                color: "blue";
                border.color: "blue";
                Rectangle {
                    anchors.fill: parent;
                    border.color: "blue";
                    color: "transparent";
                    border.width: Kirigami.Units.smallSpacing;
                }
            }

        }

        Repeater {
            model: root.currentPage.textLayer("").textareaCount

            Rectangle {
                width: coverImage.muliplierWidth * root.currentPage.textLayer("").textarea(index).bounds.width;
                height: coverImage.muliplierHeight * root.currentPage.textLayer("").textarea(index).bounds.height;
                x: coverImage.muliplierWidth * root.currentPage.textLayer("").textarea(index).bounds.x + coverImage.offsetX
                y: coverImage.muliplierHeight * root.currentPage.textLayer("").textarea(index).bounds.y + coverImage.offsetY;
                opacity: {
                    0.2;}
                color: "red";
                border.color: "red";
                border.width: Kirigami.Units.smallSpacing;
                Rectangle {
                    anchors.fill: parent;
                    border.color: "red";
                    color: "transparent";
                    border.width: Kirigami.Units.smallSpacing;
                }
            }
        }

        Repeater {
            model: root.currentPage.jumpCount

            Rectangle {
                width: coverImage.muliplierWidth * root.currentPage.jump(index).bounds.width;
                height: coverImage.muliplierHeight * root.currentPage.jump(index).bounds.height;
                x: coverImage.muliplierWidth * root.currentPage.jump(index).bounds.x + coverImage.offsetX
                y: coverImage.muliplierHeight * root.currentPage.jump(index).bounds.y + coverImage.offsetY;
                opacity: {
                    0.2;}
                color: "green";
                border.color: "green";
                border.width: Kirigami.Units.smallSpacing;
                Rectangle {
                    anchors.fill: parent;
                    border.color: "green";
                    color: "transparent";
                    border.width: Kirigami.Units.smallSpacing;
                }
            }
        }
        MouseArea {
            anchors.fill: parent;
            id: pointCatchArea;
            property point startPoint: Qt.point(0,0);
            property point endPoint: Qt.point(0,0);
            property bool dragging: false;
            hoverEnabled: true;

            onClicked: {
                if (dragging == false) {
                    startPoint = Qt.point(mouse.x, mouse.y);
                    endPoint = startPoint;
                    dragging = true;
                } else {
                    if (Qt.point(mouse.x, mouse.y)!==startPoint) {
                        endPoint = Qt.point(mouse.x, mouse.y)
                        dragging = false;
                        createFrame();
                    }
                }
                mouse.accepted
            }

            onPositionChanged: {
                if (dragging) {
                    endPoint = Qt.point(mouse.x, mouse.y)
                }
            }

            Rectangle {
                x: Math.min(parent.startPoint.x, parent.endPoint.x);
                y: Math.min(parent.startPoint.y, parent.endPoint.y);
                width: Math.max(parent.startPoint.x, parent.endPoint.x) - Math.min(parent.startPoint.x, parent.endPoint.x);
                height: Math.max(parent.startPoint.y, parent.endPoint.y) - Math.min(parent.startPoint.y, parent.endPoint.y);
                opacity: 0.5;
                border.color: "black";
                border.width: 1;
            }

            function createFrame() {
                var x  = ( Math.min(startPoint.x, endPoint.x) - coverImage.offsetX ) / coverImage.muliplierWidth;
                var x2 = ( Math.max(startPoint.x, endPoint.x) - coverImage.offsetX ) / coverImage.muliplierWidth;
                var y  = ( Math.min(startPoint.y, endPoint.y) - coverImage.offsetY ) / coverImage.muliplierHeight;
                var y2 = ( Math.max(startPoint.y, endPoint.y) - coverImage.offsetY ) / coverImage.muliplierHeight;

                addPageArea.topLeft = Qt.point(x,y);
                addPageArea.bottomRight = Qt.point(x2,y2);
                endPoint = startPoint;
                addPageArea.open();
            }

        }




    }
    Component {
        id: pageInfo;
        PageMetaInfo {
            page: root.currentPage;
            onSave: {root.pageTitle = page.title(""); root.model.setDirty();}
        }
    }

    AddPageArea {
        id: addPageArea
        onSave: {
            var index = 0;
            if (type===0) {
                index = root.currentPage.frameCount;
                root.currentPage.addFrame(index);
                root.currentPage.frame(index).setPointsFromRect(topLeft, bottomRight);
            } else if (type===1) {
                index = root.currentPage.textLayer("").textareaCount;
                root.currentPage.textLayer("").addTextarea(index);
                root.currentPage.textLayer("").textarea(index).setPointsFromRect(topLeft, bottomRight);
            } else if (type===2) {
                index = root.currentPage.jumpCount;
                root.currentPage.addJump(index);
                root.currentPage.jump(index).setPointsFromRect(topLeft, bottomRight);
            }
        }
    }

}
