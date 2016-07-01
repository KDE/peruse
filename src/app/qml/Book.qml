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
import QtQuick.Window 2.2

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.kirigami 1.0 as Kirigami
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.peruse 0.1 as Peruse

import "listcomponents" as ListComponents

Kirigami.Page {
    id: root;
    objectName: "bookViewer";
    clip: true;
    implicitWidth: applicationWindow().width;

    // Remove all the padding when we've hidden controls. Content is king!
    topPadding: applicationWindow().controlsVisible ? (applicationWindow() && applicationWindow().header ? applicationWindow().header.height : 0) : 0;
    leftPadding: applicationWindow().controlsVisible ? Kirigami.Units.gridUnit : 0;
    rightPadding: applicationWindow().controlsVisible ? Kirigami.Units.gridUnit : 0;
    bottomPadding: applicationWindow().controlsVisible ? Kirigami.Units.gridUnit : 0;

    background: Rectangle {
        anchors.fill: parent;
        opacity: applicationWindow().controlsVisible ? 0 : 1;
        Behavior on opacity { NumberAnimation { duration: mainWindow.animationDuration; } }
        color: "black";
    }

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

    function nextPage() {
        if(viewLoader.item.currentPage < viewLoader.item.pageCount - 1) {
            viewLoader.item.currentPage++;
        } else {
            bookInfo.showBookInfo(file);
        }
    }
    function previousPage() {
        if(viewLoader.item.currentPage > 0) {
            viewLoader.item.currentPage--;
        } else {
            bookInfo.showBookInfo(file);
        }
    }
    function closeBook() {
        mainWindow.contextDrawer.close();
        // also for storing current page (otherwise postponed a bit after page change, done here as well to ensure it really happens)
        applicationWindow().controlsVisible = true;
        mainWindow.pageStack.pop();
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

    function toggleFullscreen() {
        mainWindow.contextDrawer.close();
        if(applicationWindow().visibility !== Window.FullScreen) {
            applicationWindow().visibility = Window.FullScreen;
        }
        else {
            applicationWindow().visibility = Window.AutomaticVisibility;
        }
        if(viewLoader.item.restoreCurrentPage !== undefined) {
            restoreViewLayoutStuff.start();
        }
    }
    Timer {
        id: restoreViewLayoutStuff;
        interval: mainWindow.animationDuration * 2;
        running: false;
        repeat: false;
        onTriggered: {
            viewLoader.item.restoreCurrentPage();
            if(applicationWindow().visibility === Window.FullScreen) {
                applicationWindow().pageStack.currentIndex = 0;
                applicationWindow().pageStack.currentIndex = applicationWindow().pageStack.depth - 1;
            }
        }
    }

    property list<QtObject> mobileActions: [
        Kirigami.Action {
            text: applicationWindow().visibility !== Window.FullScreen ? i18nc("Enter full screen mode on a touch-based device", "Go full screen") : i18nc("Exit full sceen mode on a touch based device", "Exit full screen");
            iconName: "view-fullscreen";
            onTriggered: toggleFullscreen();
            enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypePhone;
        },
        Kirigami.Action {
            text: i18nc("Action used on touch devices to close the currently open book and return to whatever page was most recently shown", "Close book");
            shortcut: (bookInfo.opened ? "" : "Esc");
            iconName: "dialog-close";
            onTriggered: closeBook();
            enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypePhone;
        }
    ]
    property list<QtObject> desktopActions: [
        Kirigami.Action {
            text: i18nc("Top level entry leading to a submenu with options for the book display", "View options");
            iconName: "configure";
            QtObject {
                property string text: "Reading Direction"
            }
            Kirigami.Action {
                text: "Left to Right"
                iconName: "format-text-direction-ltr";
                shortcut: rtlMode ? "r" : "";
                enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypeDesktop && root.rtlMode === true;
                onTriggered: { root.rtlMode = false; }
            }
            Kirigami.Action {
                text: "Right to Left"
                iconName: "format-text-direction-rtl";
                shortcut: rtlMode ? "" : "r";
                enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypeDesktop && root.rtlMode === false;
                onTriggered: { root.rtlMode = true; }
            }
            QtObject {}
            QtObject {
                property string text: "Zoom"
            }
            Kirigami.Action {
                text: "Fit full page"
                iconName: "zoom-fit-best";
                enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypeDesktop && root.zoomMode !== Peruse.Config.ZoomFull;
                onTriggered: { root.zoomMode = Peruse.Config.ZoomFull; }
            }
            Kirigami.Action {
                text: "Fit width"
                iconName: "zoom-fit-width";
                enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypeDesktop && root.zoomMode !== Peruse.Config.ZoomFitWidth;
                onTriggered: { root.zoomMode = Peruse.Config.ZoomFitWidth; }
            }
            Kirigami.Action {
                text: "Fit height"
                iconName: "zoom-fit-height";
                enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypeDesktop && root.zoomMode !== Peruse.Config.ZoomFitHeight;
                onTriggered: { root.zoomMode = Peruse.Config.ZoomFitHeight; }
            }
            QtObject {}
        },
        Kirigami.Action {
            text: i18nc("Go to the previous page in the book", "Previous page");
            shortcut: bookInfo.opened ? "" : StandardKey.MoveToPreviousChar;
            iconName: "go-previous";
            onTriggered: previousPage();
            enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypeDesktop;
        },
        Kirigami.Action {
            text: i18nc("Go to the next page in the book", "Next page");
            shortcut: bookInfo.opened ? "" : StandardKey.MoveToNextChar;
            iconName: "go-next";
            onTriggered: nextPage();
            enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypeDesktop;
        },
        Kirigami.Action {
            text: applicationWindow().visibility !== Window.FullScreen ? i18nc("Enter full screen mode on a non-touch-based device", "Go full screen") : i18nc("Exit full sceen mode on a non-touch based device", "Exit full screen");
            shortcut: (applicationWindow().visibility === Window.FullScreen) ? (bookInfo.opened ? "" : "Esc") : "f";
            iconName: "view-fullscreen";
            onTriggered: toggleFullscreen();
            enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypeDesktop;
        },
        Kirigami.Action {
            text: i18nc("Action used on non-touch devices to close the currently open book and return to whatever page was most recently shown", "Close book");
            shortcut: (applicationWindow().visibility === Window.FullScreen) ? "" : (bookInfo.opened ? "" : "Esc");
            iconName: "dialog-close";
            onTriggered: closeBook();
            enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypeDesktop;
        },

        // Invisible actions, for use in bookInfo
        Kirigami.Action {
            visible: false;
            shortcut: bookInfo.opened ? StandardKey.MoveToPreviousChar : "";
            onTriggered: bookInfo.previousBook();
            enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypeDesktop;
        },
        Kirigami.Action {
            visible: false;
            shortcut: bookInfo.opened ? StandardKey.MoveToNextChar : "";
            onTriggered: bookInfo.nextBook();
            enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypeDesktop;
        },
        Kirigami.Action {
            visible: false;
            shortcut: bookInfo.opened ? "Return" : "";
            onTriggered: bookInfo.openSelected();
            enabled: mainWindow.pageStack.currentItem == root && mainWindow.deviceType === mainWindow.deviceTypeDesktop;
        }
    ]
    actions {
        contextualActions: mainWindow.deviceType === mainWindow.deviceTypePhone ? mobileActions : desktopActions;
        main: bookInfo.opened ? bookInfoAction : mainBookAction;
    }
    Kirigami.Action {
        id: mainBookAction;
        text: applicationWindow().visibility !== Window.FullScreen ? i18n("Enter full screen mode on any device type", "Go full screen") : i18nc("Exit full screen mode on any device type", "Exit full screen");
        iconName: "view-fullscreen";
        onTriggered: toggleFullscreen();
        enabled: mainWindow.pageStack.currentItem == root;
    }
    Kirigami.Action {
        id: bookInfoAction;
        text: i18n("Closes the book information drawer", "Close");
        shortcut: bookInfo.opened ? "Esc" : "";
        iconName: "dialog-cancel";
        onTriggered: bookInfo.close();
        enabled: mainWindow.pageStack.currentItem == root;
    }

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
            Timer {
                id: drawerTimer;
                interval: mainWindow.animationDuration * 3;
                running: false;
                repeat: false;
                onTriggered: applicationWindow().globalDrawer.close();
            }
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
                        drawerTimer.start();
                        root.restoreViewLayoutStuff.start();
                    }
                }
                onCurrentPageChanged: {
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

    Kirigami.OverlaySheet {
        id: bookInfo;
        function setNewCurrentIndex(newIndex) {
            seriesListAnimation.running = false;
            var currentPos = seriesListView.contentX;
            var newPos;
            seriesListView.positionViewAtIndex(newIndex, ListView.Center);
            newPos = seriesListView.contentX;
            seriesListAnimation.from = currentPos;
            seriesListAnimation.to = newPos;
            seriesListAnimation.running = true;
            seriesListView.currentIndex = newIndex;
        }
        function nextBook() {
            if(seriesListView.currentIndex < seriesListView.model.rowCount()) {
                setNewCurrentIndex(seriesListView.currentIndex + 1);
            }
        }
        function previousBook() {
            if(seriesListView.currentIndex > 0) {
                setNewCurrentIndex(seriesListView.currentIndex - 1);
            }
        }
        function openSelected() {
            applicationWindow().showBook(detailsTile.filename, detailsTile.currentPage);
        }
        function showBookInfo(filename) {
            if(opened) {
                return;
            }
            seriesListView.model = contentList.seriesModelForEntry(filename);
            setNewCurrentIndex(seriesListView.model.indexOfFile(filename));
            open();
        }
        onOpenedChanged: {
            if(opened === false) {
                applicationWindow().controlsVisible = controlsShown;
            }
            else {
                controlsShown = applicationWindow().controlsVisible;
                applicationWindow().controlsVisible = true;
            }
        }
        property bool controlsShown;
        property QtObject currentBook: fakeBook;
        property QtObject fakeBook: Peruse.PropertyContainer {
            property string author: "";
            property string title: "";
            property string filename: "";
            property string publisher: "";
            property string thumbnail: "";
            property string currentPage: "0";
            property string totalPages: "0";
        }
        Column {
            clip: true;
            width: root.width - units.largeSpacing * 2;
            height: childrenRect.height + units.largeSpacing * 2;
            spacing: units.largeSpacing;
            ListComponents.BookTile {
                id: detailsTile;
                height: neededHeight;
                width: parent.width;
                author: bookInfo.currentBook.readProperty("author");
                publisher: bookInfo.currentBook.readProperty("publisher");
                title: bookInfo.currentBook.readProperty("title");
                filename: bookInfo.currentBook.readProperty("filename");
                thumbnail: bookInfo.currentBook.readProperty("thumbnail");
                categoryEntriesCount: 0;
                currentPage: bookInfo.currentBook.readProperty("currentPage");
                totalPages: bookInfo.currentBook.readProperty("totalPages");
                onBookSelected: {
                    if(root.file !== filename) {
                        applicationWindow().showBook(filename, currentPage);
                    }
                }
                onBookDeleteRequested: {
                    // Not strictly needed for the listview itself, but it's kind of
                    // nice for making sure the details tile is right
                    var oldIndex = seriesListView.currentIndex;
                    seriesListView.currentIndex = -1;
                    contentList.removeBook(detailsTile.filename, true);
                    seriesListView.currentIndex = oldIndex;
                }
            }
            // tags and ratings, comment by self
            // store hook for known series with more content
            ListView {
                id: seriesListView;
                width: parent.width;
                height: units.gridUnit * 12;
                orientation: ListView.Horizontal;
                NumberAnimation { id: seriesListAnimation; target: seriesListView; property: "contentX"; duration: mainWindow.animationDuration; easing.type: Easing.InOutQuad; }
                delegate: ListComponents.BookTileTall {
                    height: model.filename != "" ? neededHeight : 1;
                    width: seriesListView.width / 3;
                    author: model.author;
                    title: model.title;
                    filename: model.filename;
                    thumbnail: model.thumbnail;
                    categoryEntriesCount: 0;
                    currentPage: model.currentPage;
                    totalPages: model.totalPages;
                    onBookSelected: bookInfo.setNewCurrentIndex(model.index);
                    selected: seriesListView.currentIndex === model.index;
                }
                onCurrentIndexChanged: {
                    bookInfo.currentBook = model.get(currentIndex);
                }
            }
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
