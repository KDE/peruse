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
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.kirigami 1.0 as Kirigami
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.peruse 0.1 as Peruse

Kirigami.Page {
    id: root;
    clip: true;
    implicitWidth: applicationWindow().width;

    // Perhaps we should store and restore this?
    property bool showControls: true;
    property Item pageStackItem: applicationWindow().pageStack.currentItem;
    onPageStackItemChanged: {
        if(mainWindow.pageStack.currentItem == root) {
            applicationWindow().controlsVisible = root.showControls;
        }
        else {
            root.showControls = applicationWindow().controlsVisible;
            applicationWindow().controlsVisible = true;
        }
    }

    property string file;
    property int currentPage;
    property int totalPages;
    onCurrentPageChanged: {
        // set off a timer to slightly postpone saving the current page, so it doesn't happen during animations etc
        updateCurrent.start();
    }

    function nextPage() {
        if(viewLoader.item.currentPage < viewLoader.item.pageCount - 1) {
            viewLoader.item.currentPage++;
        }
    }
    function previousPage() {
        if(viewLoader.item.currentPage > 0) {
            viewLoader.item.currentPage--;
        }
    }
    function closeBook() {
        // also for storing current page (otherwise postponed a bit after page change, done here as well to ensure it really happens)
        applicationWindow().controlsVisible = true;
        mainWindow.pageStack.pop();
    }

    property Item contextualTopItems: ListView {
        id: thumbnailNavigator;
        anchors.fill: parent;
        clip: true;
        delegate: thumbnailComponent;
    }
    Component {
        id: thumbnailComponent;
        Item {
            width: parent.width;
            height: units.gridUnit * 6;
            MouseArea {
                anchors.fill: parent;
                onClicked: viewLoader.item.currentPage = model.index;
            }
            Rectangle {
                anchors.fill: parent;
                color: theme.highlightColor;
                opacity: root.currentPage === model.index ? 1 : 0;
                Behavior on opacity { NumberAnimation { duration: units.shortDuration; } }
            }
            Image {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    margins: units.smallSpacing;
                }
                height: parent.height - pageTitle.height - units.smallSpacing * 2;
                asynchronous: true;
                fillMode: Image.PreserveAspectFit;
                source: model.url;
            }
            PlasmaComponents.Label {
                id: pageTitle;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                }
                height: paintedHeight;
                text: model.title;
                elide: Text.ElideMiddle;
                horizontalAlignment: Text.AlignHCenter;
            }
        }
    }

    property list<QtObject> mobileActions: [
        Kirigami.Action {
            text: "Close book";
            shortcut: "Esc";
            iconName: "action-close";
            onTriggered: closeBook();
            enabled: mainWindow.pageStack.currentItem == root;
        }
    ]
    property list<QtObject> desktopActions: [
        Kirigami.Action {
            text: "Close book";
            shortcut: "Esc";
            iconName: "action-close";
            onTriggered: closeBook();
            enabled: mainWindow.pageStack.currentItem == root;
        },
        Kirigami.Action {
            text: "Previous page";
            shortcut: StandardKey.MoveToPreviousChar
            iconName: "action-previous";
            onTriggered: previousPage();
            enabled: mainWindow.pageStack.currentItem == root;
        },
        Kirigami.Action {
            text: "Next page";
            shortcut: StandardKey.MoveToNextChar;
            iconName: "action-next";
            onTriggered: nextPage();
            enabled: mainWindow.pageStack.currentItem == root;
        }
    ]
    contextualActions: PLASMA_PLATFORM.substring(0, 5) === "phone" ? mobileActions : desktopActions;

    Item {
        width: root.width - (root.leftPadding + root.rightPadding);
        height: root.height - (root.topPadding + root.bottomPadding);
        Timer {
            id: updateCurrent;
            interval: mainWindow.animationDuration;
            running: false;
            repeat: false;
            onTriggered: {
                if(viewLoader.item && viewLoader.item.pagesModel && viewLoader.item.pagesModel.currentPage !== undefined) {
                    viewLoader.item.pagesModel.currentPage = root.currentPage;
                }
            }
        }
        NumberAnimation { id: thumbnailMovementAnimation; target: thumbnailNavigator; property: "contentY"; duration: mainWindow.animationDuration; easing.type: Easing.InOutQuad; }
        Loader {
            id: viewLoader;
            anchors.fill: parent;
            property bool loadingCompleted: false;
            onLoaded: {
                if(status === Loader.Error) {
                    // huh! problem...
                }
                else {
                    item.file = root.file;
                }
            }
            Connections {
                target: viewLoader.item;
                onLoadingCompleted: {
                    if(success) {
                        thumbnailNavigator.model = viewLoader.item.pagesModel;
                        if(viewLoader.item.thumbnailComponent) {
                            thumbnailNavigator.delegate = viewLoader.item.thumbnailComponent;
                        }
                        else {
                            thumbnailNavigator.delegate = thumbnailComponent;
                        }
                        peruseConfig.setFilesystemProperty(root.file, "totalPages", viewLoader.item.pageCount);
                        if(root.totalPages !== viewLoader.item.pageCount) {
                            root.totalPages = viewLoader.item.pageCount;
                        }
                        viewLoader.item.currentPage = root.currentPage;
                        viewLoader.loadingCompleted = true;
                    }
                }
                onCurrentPageChanged: {
                    if(root.currentPage !== viewLoader.item.currentPage && viewLoader.loadingCompleted) {
                        root.currentPage = viewLoader.item.currentPage;
                    }
                    thumbnailMovementAnimation.false;
                    var currentPos = thumbnailNavigator.contentY;
                    var newPos;
                    thumbnailNavigator.positionViewAtIndex(viewLoader.item.currentPage, ListView.Center);
                    newPos = thumbnailNavigator.contentY;
                    thumbnailMovementAnimation.from = currentPos;
                    thumbnailMovementAnimation.to = newPos;
                    thumbnailMovementAnimation.running = true;
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
            onClicked: previousPage();
        }
        MouseArea {
            anchors {
                top: parent.top;
                right: parent.right;
                bottom: parent.bottom;
            }
            width: parent.width / 6;
            onClicked: nextPage();
        }
    }

    onFileChanged: {
        // Let's set the page title to something useful
        var book = contentList.get(contentList.indexOfFile(file));
        title = book.readProperty("title");

        // The idea is to have a number of specialised options as relevant to various
        // types of comic books, and then finally fall back to Okular as a catch-all
        // but generic viewer component.
        var attemptFallback = true;

        var mimetype = contentList.contentModel.getMimetype(file);
        console.debug("Mimetype is " + mimetype);
        if(mimetype == "application/x-cbz" || mimetype == "application/x-cbr") {
            viewLoader.source = "viewers/cbr.qml";
            attemptFallback = false;
        }
        if(mimetype == "inode/directory") {
            viewLoader.source = "viewers/folderofimages.qml";
            attemptFallback = false;
        }

        if(attemptFallback) {
            viewLoader.source = "viewers/okular.qml";
        }
    }
}
