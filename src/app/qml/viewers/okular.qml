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

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.okular 2.0 as Okular

ViewerBase {
    id: root;
    onFileChanged: documentItem.path = file;
    onCurrentPageChanged: {
        if(documentItem.currentPage !== currentPage) {
            documentItem.currentPage = currentPage;
        }
    }
    pageCount: documentItem.pageCount;
    thumbnailComponent: thumbnailComponent;
    pagesModel: documentItem.matchingPages;
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
            Okular.ThumbnailItem {
                id: thumbnail
                anchors {
                    top: parent.top;
                    horizontalCenter: parent.horizontalCenter;
                    margins: units.smallSpacing;
                }
                document: documentItem
                pageNumber: modelData
                height: parent.height - pageTitle.height - units.smallSpacing * 2;
                function updateWidth() {
                    width = Math.round(height * (implicitWidth / implicitHeight));
                }
                Component.onCompleted: updateWidth();
                onHeightChanged: updateWidth();
                onImplicitHeightChanged: updateWidth();
            }
            PlasmaComponents.Label {
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
            }
        }
        onCurrentPageChanged: {
            if(root.currentPage !== currentPage) {
                root.currentPage = currentPage;
            }
        }
    }

    Okular.DocumentView {
        id: pageArea
        document: documentItem
        anchors.fill: parent

        onPageChanged: {
//             bookmarkConnection.target = page
//             actionButton.checked = page.bookmarked
        }
        onClicked: {
            if(applicationWindow().controlsVisible === true) {
                applicationWindow().controlsVisible = false;
            }
            else {
                applicationWindow().controlsVisible = true;
            }
        }
    }
}
