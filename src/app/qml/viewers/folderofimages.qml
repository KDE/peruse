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

ViewerBase {
    id: root;
    pagesModel: imageBrowser.model;
    pageCount: imageBrowser.model.pageCount;

    onCurrentPageChanged: {
        if(currentPage !== imageBrowser.currentIndex) {
            pageChangeAnimation.false;
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
    NumberAnimation { id: pageChangeAnimation; target: imageBrowser; property: "contentX"; duration: mainWindow.animationDuration; easing.type: Easing.InOutQuad; }

    ImageBrowser {
        id: imageBrowser;
        anchors.fill: parent;
        model: Peruse.FolderBookModel {
            filename: root.file;
            onLoadingCompleted: root.loadingCompleted(success);
        }
        onCurrentIndexChanged: {
            if(root.currentPage !== currentIndex) {
                root.currentPage = currentIndex;
            }
        }
        imageWidth: root.width;
        imageHeight: root.height;
    }
}
