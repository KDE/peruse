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
import QtQuick.Controls 1.0

import org.kde.peruse 0.1 as Peruse
/**
 * @brief a ViewerBase for CBR style books.
 * 
 * It is called from Book when the opened book is one of the following files:
 * - application/x-cbz
 * - application/x-cbr
 * - application/vnd.comicbook+zip
 * - application/vnd.comicbook+rar
 */
ViewerBase {
    id: root;
    property string title: imageBrowser.model.title;
    pagesModel: imageBrowser.model;
    pageCount: imageBrowser.model.pageCount;
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
    hasFrames: true;
    onNextFrame: imageBrowser.goNextFrame();
    onPreviousFrame: imageBrowser.goPreviousFrame();

    onCurrentPageChanged: {
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

    Timer {
        id: initialPageChange;
        interval: applicationWindow().animationDuration;
        running: false;
        repeat: false;
        onTriggered: root.currentPage = imageBrowser.model.currentPage;
    }
    ImageBrowser {
        id: imageBrowser;
        anchors.fill: parent;
        model: Peruse.ArchiveBookModel {
            filename: root.file;
            qmlEngine: globalQmlEngine;
            onLoadingCompleted: {
                root.loadingCompleted(success);
                initialPageChange.start();
            }
        }
        onCurrentIndexChanged: {
            if(root.currentPage !== currentIndex) {
                root.currentPage = currentIndex;
            }
        }
        onGoNextPage: root.goNextPage();
        onGoPreviousPage: root.goPreviousPage();
        imageWidth: root.width;
        imageHeight: root.height;
    }
}
