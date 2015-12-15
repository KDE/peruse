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

import org.kde.plasma.mobilecomponents 0.2 as MobileComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.peruse 0.1 as Peruse

MobileComponents.Page {
    id: root;
    color: MobileComponents.Theme.viewBackgroundColor;
    property string file;
    property int currentPage;

    contextualActions: [
        Action {
            text: "Close book";
            iconName: "action-close";
            onTriggered: mainWindow.pageStack.pop();
        },
        Action {
            text: "Previous page";
            iconName: "action-previous";
        },
        Action {
            text: "Next page";
            iconName: "action-next";
        }
    ]

    Loader {
        id: viewLoader;
        anchors.fill: parent;
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
                    item.currentIndex = root.currentPage;
                }
            }
        }
    }
    onFileChanged: {
        // The idea is to have a number of specialised options as relevant to various
        // types of comic books, and then finally fall back to Okular as a catch-all
        // but generic viewer component.
        var attemptFallback = false;

        var mimetype = contentList.contentModel.getMimetype(file);
        console.debug("Mimetype is " + mimetype);
        if(mimetype == "application/x-cbz") {
            viewLoader.source = "viewers/cbr.qml";
        }
        if(mimetype == "inode/directory") {
            viewLoader.source = "viewers/folderofimages.qml";
        }

        if(attemptFallback) {
            viewLoader.source = "viewers/okular.qml";
        }
    }
}
