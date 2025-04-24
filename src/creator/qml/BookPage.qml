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

import QtQuick 2.12
import QtQuick.Controls 2.12 as QQC2

import org.kde.kirigami 2.7 as Kirigami
/**
 * Page that holds an image to edit the frames on.
 */
Kirigami.Page {
    id: root;
    property string categoryName: "bookPage";
    title: i18nc("title of the page editing sub-page for the book editor", "Page %1", root.pageTitle === "" ? root.index : root.pageTitle);
    property QtObject model;
    property QtObject currentPage;
    property QtObject currentLanguage: null;
    property string currentLanguageCode: currentLanguage ? currentLanguage.language : ""
    property int index: -1;
    property string pageUrl: "";
    property string pageTitle: "";
    property var pageList: [];
    property var textTypes: ["speech", "commentary", "formal", "letter", "code", "heading"];
    signal save();

    enum FieldTypes {
        Frame,
        Textarea,
        Jump
    }

    onIndexChanged: {
        if (root.index===0) {
            root.currentPage = root.model.acbfData.metaData.bookInfo.coverpage();
        } else if (root.index > 0) {
            root.currentPage = root.model.acbfData.body.page(root.index-1);
        }
        root.pageTitle = root.currentPage.title(root.currentLanguageCode);
        // Let's ensure there's always a default text-layer.
        if (root.currentPage.textLayerLanguages.length === 0) {
            root.currentPage.addTextLayer("")
        } else if (root.currentPage.textLayerLanguages.indexOf("") < 0) {
            root.currentPage.duplicateTextLayer(root.currentPage.textLayerLanguages[1], "");
        }

        pageList = [];
        for (var i=0; i<model.acbfData.body.pageCount; i++){
            var t = model.acbfData.body.page(i).title(root.currentLanguageCode)
            if (t !== "") {
                pageList.push(t);
            } else {
                pageList.push(i18n("Page %1", i+1));
            }
        }
    }
    Component.onCompleted: {
        root.updateTranslationActions();
    }

    Kirigami.Action {
        id: translationsAction
        text: i18nc("A submenu which allows the user to chose between translations of the book", "Translations")
        visible: root.model.acbfData.metaData.bookInfo.languages.length > 0
        Kirigami.Action {
            text: i18nc("The option used to show no translation should be used", "No Translation")
            onTriggered: root.currentLanguage = null
            checked: root.currentLanguage === null
            checkable: true
            QQC2.ActionGroup.group: translationSelectionGroup
        }
    }

    actions: [
        saveAndCloseAction,
        editPageDataAction,
        translationsAction,
    ]

    Kirigami.Action {
        id: saveAndCloseAction;
        text: i18nc("Saves the remaining unsaved edited fields and closes the page editor", "Save and Close Page");
        icon.name: "dialog-ok";
        shortcut: "Esc";
        enabled: !addPageArea.sheetOpen
        onTriggered: {
            root.save();
            root.model.setDirty();
            pageStack.pop();
        }
    }
    Kirigami.Action {
        id: editPageDataAction;
        text: i18nc("Edit the page data in detail", "Edit Page Data");
        icon.name: "document-edit"
        onTriggered: pageStack.push(pageInfo)
    }

    function updateTranslationActions() {
        for (var i = 0 ; i < root.model.acbfData.metaData.bookInfo.languages.length ; ++i) {
            var language = root.model.acbfData.metaData.bookInfo.languages[i];
            var action = translationActionEntry.createObject(translationsAction, {language: language});
            translationsAction.children.push(action);
        }
    }
    QQC2.ActionGroup { id: translationSelectionGroup }
    Component {
        id: translationActionEntry
        Kirigami.Action {
            id: control
            text: language.language
            visible: language.show
            property QtObject language
            onTriggered: { root.currentLanguage = control.language; }
            checked: root.currentLanguage && root.currentLanguage === control.language
            checkable: true
            QQC2.ActionGroup.group: translationSelectionGroup
        }
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
            model: root.currentPage.framePointStrings

            Rectangle {
                width: coverImage.muliplierWidth * root.currentPage.frame(index).bounds.width;
                height: coverImage.muliplierHeight * root.currentPage.frame(index).bounds.height;
                x: coverImage.muliplierWidth * root.currentPage.frame(index).bounds.x + coverImage.offsetX
                y: coverImage.muliplierHeight * root.currentPage.frame(index).bounds.y + coverImage.offsetY;
                border.color: "blue";
                color: "transparent";
                opacity: 0.5;
                border.width: Kirigami.Units.smallSpacing;
                Rectangle {
                    anchors.fill: parent;
                    opacity: 0.4;
                    color: "blue";
                }
                Text {
                    text: index+1;
                    anchors {
                        centerIn: parent;
                    }
                    color: "blue";
                    font.weight: Font.Bold;
                }

                MouseArea {
                    anchors.fill: parent;

                    onClicked: {
                        editPageArea.editObject(root.currentPage.frame(index), BookPage.FieldTypes.Frame);
                    }
                }
            }
        }

        Repeater {
            model: root.currentPage.textLayer(root.currentLanguageCode).textareaPointStrings

            Rectangle {
                width: coverImage.muliplierWidth * root.currentPage.textLayer(root.currentLanguageCode).textarea(index).bounds.width;
                height: coverImage.muliplierHeight * root.currentPage.textLayer(root.currentLanguageCode).textarea(index).bounds.height;
                x: coverImage.muliplierWidth * root.currentPage.textLayer(root.currentLanguageCode).textarea(index).bounds.x + coverImage.offsetX
                y: coverImage.muliplierHeight * root.currentPage.textLayer(root.currentLanguageCode).textarea(index).bounds.y + coverImage.offsetY;
                border.color: "red";
                color: "transparent";
                opacity: 0.5;
                border.width: Kirigami.Units.smallSpacing;
                Rectangle {
                    anchors.fill: parent;
                    opacity: 0.4;
                    color: "red";
                }
                Text {
                    text: index+1;
                    anchors {
                        centerIn: parent;
                    }
                    color: "red";
                    font.weight: Font.Bold;
                }

                MouseArea {
                    anchors.fill: parent;

                    onClicked: {
                        editPageArea.editObject(root.currentPage.textLayer(root.currentLanguageCode).textarea(index), BookPage.FieldTypes.Textarea);
                    }
                }
            }
        }

        Repeater {
            model: root.currentPage.jumps

            Rectangle {
                width: coverImage.muliplierWidth * modelData.bounds.width;
                height: coverImage.muliplierHeight * modelData.bounds.height;
                x: coverImage.muliplierWidth * modelData.bounds.x + coverImage.offsetX
                y: coverImage.muliplierHeight * modelData.bounds.y + coverImage.offsetY;
                border.color: "green";
                color: "transparent";
                opacity: 0.5;
                border.width: Kirigami.Units.smallSpacing;
                Rectangle {
                    anchors.fill: parent;
                    opacity: 0.4;
                    color: "green";
                }
                Text {
                    text: index+1;
                    anchors {
                        centerIn: parent;
                    }
                    color: "green";
                    font.weight: Font.Bold;
                }

                MouseArea {
                    anchors.fill: parent;

                    onClicked: {
                        editPageArea.editObject(modelData, BookPage.FieldTypes.Jump)
                    }
                }
            }
        }
        MouseArea {
            anchors.fill: parent;
            id: pointCatchArea;
            property point startPoint: Qt.point(0,0);
            property point endPoint: Qt.point(0,0);
            property bool dragging: false;
            //hoverEnabled: true;
            preventStealing: false;
            propagateComposedEvents: true;
            property bool createNewObject: false;

            onClicked: {
                if(!createNewObject) {
                    mouse.accepted = false;
                } else {
                    createNewObject = false;
                }
            }

            onPressed: {
                if (dragging == false) {
                    startPoint = Qt.point(mouse.x, mouse.y);
                    endPoint = startPoint;
                    dragging = true;
                    mouse.accepted
                }

            }

            onPositionChanged: {
                if (dragging) {
                    endPoint = Qt.point(mouse.x, mouse.y)
                    mouse.accepted
                }
            }

            onReleased: {
                if (dragging) {
                    if (Qt.point(mouse.x, mouse.y)!==startPoint) {
                        endPoint = Qt.point(mouse.x, mouse.y);
                        createNewObject = true;
                        createFrame();
                        mouse.accepted
                    }

                    dragging = false;
                }

            }
            onCanceled: {
                dragging = false;
                endPoint: Qt.point(0,0);
                startPoint: Qt.point(0,0);
                mouse.accepted = false;
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
                var x  = Math.max(( Math.min(startPoint.x, endPoint.x) - coverImage.offsetX ) / coverImage.muliplierWidth, 0);
                var x2 = Math.min(( Math.max(startPoint.x, endPoint.x) - coverImage.offsetX ) / coverImage.muliplierWidth, coverImage.sourceSize.width);
                var y  = Math.max(( Math.min(startPoint.y, endPoint.y) - coverImage.offsetY ) / coverImage.muliplierHeight, 0);
                var y2 = Math.min(( Math.max(startPoint.y, endPoint.y) - coverImage.offsetY ) / coverImage.muliplierHeight, coverImage.sourceSize.height);

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
            colorname: root.model.acbfData.body.bgcolor !== ""? root.model.acbfData.body.bgcolor: "#ffffff";
            page: root.currentPage;
            onSave: {root.pageTitle = page.title(root.currentLanguageCode); root.model.setDirty();}
        }
    }

    AddPageArea {
        id: addPageArea
        imageSource: pageUrl;
        onAccepted: {
            var index = 0;
            var createdObject;
            if (type===BookPage.FieldTypes.Frame) {
                index = root.currentPage.framePointStrings.length;
                root.currentPage.addFrame(index);
                root.currentPage.frame(index).setPointsFromRect(topLeft, bottomRight);

                createdObject = root.currentPage.frame(index);
            } else if (type===BookPage.FieldTypes.Textarea) {
                index = root.currentPage.textLayer(root.currentLanguageCode).textareaPointStrings.length;
                root.currentPage.textLayer(root.currentLanguageCode).addTextarea(index);
                root.currentPage.textLayer(root.currentLanguageCode).textarea(index).setPointsFromRect(topLeft, bottomRight);

                createdObject = root.currentPage.textLayer(root.currentLanguageCode).textarea(index);
            } else if (type===BookPage.FieldTypes.Jump) {
                index = root.currentPage.jumps.length;
                root.currentPage.addJump(0, index);
                root.currentPage.jump(index).setPointsFromRect(topLeft, bottomRight);

                createdObject = root.currentPage.jump(index);
            }

            editPageArea.editObject(createdObject, type);
        }
    }

    EditPageArea {
        id: editPageArea;

        imageSource: pageUrl;
        pages: root.pageList;
        availableTypes: root.textTypes;
        model: root.model;
    }
}
