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
import QtQuick.Controls 2.12 as QtControls
import QtQuick.Window 2.12

import org.kde.kirigami 2.16 as Kirigami

import org.kde.peruse as Peruse
import "listcomponents" as ListComponents
/**
 * @brief Page that handles reading the book.
 * 
 * 
 */
Kirigami.Page {
    id: root;
    objectName: "bookViewer";
    clip: true;
    property bool isCurrentContext: isCurrentPage && applicationWindow().bookOpen

    // Remove all the padding when we've hidden controls. Content is king!
    topPadding: applicationWindow().controlsVisible ? (applicationWindow() && applicationWindow().header ? applicationWindow().header.height : 0) : 0;
    leftPadding: applicationWindow().controlsVisible ? Kirigami.Units.gridUnit : 0;
    rightPadding: applicationWindow().controlsVisible ? Kirigami.Units.gridUnit : 0;
    bottomPadding: applicationWindow().controlsVisible ? Kirigami.Units.gridUnit * 2 : 0;

    background: Rectangle {
        anchors.fill: parent;
        opacity: applicationWindow().controlsVisible ? 0 : 1;
        Behavior on opacity { NumberAnimation { duration: applicationWindow().animationDuration; } }
        color: "black";
    }

    // Perhaps we should store and restore this?
    property bool showControls: true;
    property Item pageStackItem: applicationWindow().pageStack.layers.currentItem;
    onPageStackItemChanged: {
        if(root.isCurrentContext) {
            applicationWindow().controlsVisible = root.showControls;
        }
        else {
            root.showControls = applicationWindow().controlsVisible;
            applicationWindow().controlsVisible = true;
        }
    }

    property bool rtlMode: false;
    /**
     * zoomMode: Peruse.Config.ZoomMode
     */
    property int zoomMode: Peruse.Config.ZoomFull;

    property string file;
    property int currentPage;
    property int totalPages;
    onCurrentPageChanged: {
        // set off a timer to slightly postpone saving the current page, so it doesn't happen during animations etc
        updateCurrent.start();
    }

    function nextFrame() {
        // If there is a next frame to go to, or whether it is supported at all
        if(viewLoader.item.hasFrames === true) {
            viewLoader.item.nextFrame();
        }
        else {
            nextPage();
        }
    }
    function previousFrame(): void {
        // If there is a next frame to go to, or whether it is supported at all
        if(viewLoader.item.hasFrames === true) {
            viewLoader.item.previousFrame();
        }
        else {
            previousPage();
        }
    }
    function nextPage(): void {
        if(viewLoader.item.currentPage < viewLoader.item.pageCount - 1) {
            viewLoader.item.currentPage++;
        }
    }
    function previousPage(): void {
        if(viewLoader.item.currentPage > 0) {
            viewLoader.item.currentPage--;
        }
    }
    function setCurrentPage(pageNumber) {
        viewLoader.item.currentPage = pageNumber;
    }
    function closeBook() {
        applicationWindow().contextDrawer.close();
        // also for storing current page (otherwise postponed a bit after page change, done here as well to ensure it really happens)
        applicationWindow().controlsVisible = true;
        applicationWindow().pageStack.layers.pop();
        applicationWindow().globalDrawer.open();
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
            width: parent !== null ? parent.width : height;
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
            Image {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    margins: Kirigami.Units.smallSpacing;
                }
                height: parent.height - pageTitle.height - Kirigami.Units.smallSpacing * 2;
                asynchronous: true;
                fillMode: Image.PreserveAspectFit;
                source: model.url;
            }
            QtControls.Label {
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

    function toggleFullscreen(): void {
        applicationWindow().contextDrawer.close();
        if(applicationWindow().visibility !== Window.FullScreen) {
            applicationWindow().visibility = Window.FullScreen;
            applicationWindow().controlsVisible = false;
        }
        else {
            applicationWindow().visibility = Window.AutomaticVisibility;
            applicationWindow().controlsVisible = true;
        }
    }

    property list<QtObject> mobileActions: [
        Kirigami.Action {
            text: applicationWindow().visibility !== Window.FullScreen ? i18nc("Enter full screen mode on a touch-based device", "Go Full Screen") : i18nc("Exit full sceen mode on a touch based device", "Exit Full Screen");
            icon.name: "view-fullscreen";
            onTriggered: toggleFullscreen();
            enabled: root.isCurrentContext && Kirigami.Settings.isMobile
        },
        Kirigami.Action {
            text: i18nc("Action used on touch devices to close the currently open book and return to whatever page was most recently shown", "Close Book");
            shortcut: "Esc"
            icon.name: "dialog-close";
            onTriggered: closeBook();
            enabled: root.isCurrentContext && Kirigami.Settings.isMobile
        }
    ]
    property list<QtObject> desktopActions: [
        Kirigami.Action {
            text: i18nc("Top level entry leading to a submenu with options for the book display", "View Options");
            icon.name: "configure";
            Kirigami.Action {
                text: i18nc("Header title for the section in which the direction the book will be navigated can be picked", "Reading Direction")
            }
            Kirigami.Action {
                text: i18nc("Title for the option which will make the book navigate from left to right", "Left to Right")
                icon.name: "format-text-direction-ltr";
                shortcut: rtlMode ? "r" : "";
                enabled: root.isCurrentContext && !Kirigami.Settings.isMobile && root.rtlMode;
                onTriggered: { root.rtlMode = false; }
            }
            Kirigami.Action {
                text: i18nc("Title for the option which will make the book navigate from right to left", "Right to Left")
                icon.name: "format-text-direction-rtl";
                shortcut: rtlMode ? "" : "r";
                enabled: root.isCurrentContext && !Kirigami.Settings.isMobile && !root.rtlMode;
                onTriggered: { root.rtlMode = true; }
            }
//             QtObject {
//                 property string text: "Zoom"
//             }
//             Kirigami.Action {
//                 text: "Fit full page"
//                 icon.name: "zoom-fit-best";
//                 enabled: root.isCurrentContext && !Kirigami.Settings.isMobile && root.zoomMode !== Peruse.Config.ZoomFull;
//                 onTriggered: { root.zoomMode = Peruse.Config.ZoomFull; }
//             }
//             Kirigami.Action {
//                 text: "Fit width"
//                 icon.name: "zoom-fit-width";
//                 enabled: root.isCurrentContext && !Kirigami.Settings.isMobile && root.zoomMode !== Peruse.Config.ZoomFitWidth;
//                 onTriggered: { root.zoomMode = Peruse.Config.ZoomFitWidth; }
//             }
//             Kirigami.Action {
//                 text: "Fit height"
//                 icon.name: "zoom-fit-height";
//                 enabled: root.isCurrentContext && !Kirigami.Settings.isMobile && root.zoomMode !== Peruse.Config.ZoomFitHeight;
//                 onTriggered: { root.zoomMode = Peruse.Config.ZoomFitHeight; }
//             }
//             QtObject {}
        },
        Kirigami.Action {
            text: i18nc("Go to the previous frame on the current page", "Previous Frame");
            shortcut: root.isCurrentContext && StandardKey.MoveToPreviousChar;
            icon.name: "go-previous";
            onTriggered: previousFrame();
            enabled: root.isCurrentContext && !Kirigami.Settings.isMobile
        },
        Kirigami.Action {
            text: i18nc("Go to the next frame on the current page", "Next Frame");
            shortcut: root.isCurrentContext && StandardKey.MoveToNextChar;
            icon.name: "go-next";
            onTriggered: nextFrame();
            enabled: root.isCurrentContext && !Kirigami.Settings.isMobile
        },
        Kirigami.Action {
            text: i18nc("Go to the previous page in the book", "Previous Page");
            shortcut: root.isCurrentContext && StandardKey.MoveToNextPage;
            icon.name: "go-previous";
            onTriggered: previousPage();
            enabled: root.isCurrentContext && !Kirigami.Settings.isMobile;
        },
        Kirigami.Action {
            text: i18nc("Go to the next page in the book", "Next Page");
            shortcut: StandardKey.MoveToNextPage;
            icon.name: "go-next";
            onTriggered: nextPage();
            enabled: root.isCurrentContext && !Kirigami.Settings.isMobile;
        },
        Kirigami.Action {
            text: applicationWindow().visibility !== Window.FullScreen ? i18nc("Enter full screen mode on a non-touch-based device", "Go Full Screen") : i18nc("Exit full sceen mode on a non-touch based device", "Exit Full Screen");
            shortcut: (applicationWindow().visibility === Window.FullScreen) ? "Esc" : "f";
            icon.name: "view-fullscreen";
            onTriggered: toggleFullscreen();
            enabled: root.isCurrentContext && !Kirigami.Settings.isMobile;
        },
        Kirigami.Action {
            text: i18nc("Action used on non-touch devices to close the currently open book and return to whatever page was most recently shown", "Close Book");
            shortcut: (applicationWindow().visibility === Window.FullScreen) ? "" : "Esc";
            icon.name: "dialog-close";
            onTriggered: closeBook();
            enabled: root.isCurrentContext && !Kirigami.Settings.isMobile;
        }
    ]
    actions: Kirigami.Settings.isMobile ? mobileActions : desktopActions

    function updateContextualActions() {
        actions.contextualActions.length = 0;
        var newList = Kirigami.Settings.isMobile ? mobileActions : desktopActions;
        for(var i = 0; i < viewLoader.item.viewerActions.length; ++i) {
            var action = viewLoader.item.viewerActions[i];
            newList.push(action);
        }
        actions.contextualActions = newList;
    }

    /**
     * This holds an instance of ViewerBase, which can either be the
     * Okular viewer(the fallback one), or one of the type specific
     * ones(ImageBrowser based).
     */
    Item {
        width: root.width - (root.leftPadding + root.rightPadding);
        height: root.height - (root.topPadding + root.bottomPadding);
        Timer {
            id: updateCurrent;
            interval: applicationWindow().animationDuration;
            running: false;
            repeat: false;
            onTriggered: {
                if(viewLoader.item && viewLoader.item.pagesModel && viewLoader.item.pagesModel.currentPage !== undefined) {
                    viewLoader.item.pagesModel.currentPage = root.currentPage;
                }
            }
        }
        NumberAnimation { id: thumbnailMovementAnimation; target: thumbnailNavigator; property: "contentY"; duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; }
        Loader {
            id: viewLoader;
            anchors.fill: parent;
            property bool loadingCompleted: false;
            onStatusChanged: {
                if (status === Loader.Error) {
                    console.debug("Error loading up the reader...");
                }
            }
            onLoaded: item.file = root.file;
            Binding {
                target: viewLoader.item;
                property: "rtlMode";
                value: root.rtlMode;
            }
            Binding {
                target: viewLoader.item;
                property: "zoomMode";
                value: root.zoomMode;
            }
            Connections {
                target: viewLoader.item;
                function onLoadingCompleted(success) {
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
                        root.updateContextualActions();
                        applicationWindow().globalDrawer.close();
                    }
                }
                function onTitleChanged() { root.title = viewLoader.item.title; }
                function onCurrentPageChanged() {
                    if(root.currentPage !== viewLoader.item.currentPage && viewLoader.loadingCompleted) {
                        root.currentPage = viewLoader.item.currentPage;
                    }
                    thumbnailMovementAnimation.running = false;
                    var currentPos = thumbnailNavigator.contentY;
                    var newPos;
                    thumbnailNavigator.positionViewAtIndex(viewLoader.item.currentPage, ListView.Center);
                    newPos = thumbnailNavigator.contentY;
                    thumbnailMovementAnimation.from = currentPos;
                    thumbnailMovementAnimation.to = newPos;
                    thumbnailMovementAnimation.running = true;
                }
                function onViewerActionsChanged() { root.updateContextualActions(); }
                function onGoNextPage() { root.nextPage(); }
                function onGoPreviousPage() { root.previousPage(); }
                function onGoPage() { root.setCurrentPage(pageNumber); }
            }
        }
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            width: parent.width - (Kirigami.Units.largeSpacing * 8)

            visible: viewLoader.status === Loader.Error;

            icon.name: "emblem-error"
            text: i18nc("Message shown on the book reader view when there is an issue loading any reader at all (usually when Okular's qml components are not installed for some reason)", "Failed to load the reader component")
            explanation: i18nc("Message shown on the book reader view when there is an issue loading any reader at all (usually when Okular's qml components are not installed for some reason)", "This is generally caused by broken packaging. Contact whomever you got this package from and inform them of this error.");
        }
    }

    onFileChanged: {
        // Let's set the page title to something useful
        var book = contentList.bookFromFile(file);
        root.title = book.readProperty("title");

        // The idea is to have a number of specialised options as relevant to various
        // types of comic books, and then finally fall back to Okular as a catch-all
        // but generic viewer component.
        var attemptFallback = true;

        var mimetype = contentList.contentModel.getMimetype(file);
        console.debug("Mimetype is " + mimetype);
        if(mimetype == "application/x-cbz" || mimetype == "application/x-cbr" || mimetype == "application/vnd.comicbook+zip" || mimetype == "application/vnd.comicbook+rar") {
            viewLoader.source = "viewers/cbr.qml";
            attemptFallback = false;
        }
        if(mimetype == "inode/directory" || mimetype == "image/jpeg" || mimetype == "image/png") {
            viewLoader.source = "viewers/folderofimages.qml";
            attemptFallback = false;
        }

        if(attemptFallback) {
            viewLoader.source = "viewers/okular.qml";
        }
    }
}
