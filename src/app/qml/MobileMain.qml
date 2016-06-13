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
import org.kde.kirigami 1.0 as Kirigami
import org.kde.peruse 0.1 as Peruse

PeruseMain {
    id: root;
    width: 750;
    height: 1100;

    function openOther() {
        pageStack.push(openDlg, { folder: root.homeDir() } );
    }

    Component {
        id: openDlg;
        Kirigami.Page {
            id: root;
            property string folder;
            title: i18nc("Title of a page which lets you open comics not in your collection by using a standard touch-friendly dig-down style filesystem browser", "Open comics not in your collection");
            FileFinder {
                width: root.width - (root.leftPadding + root.rightPadding);
                height: root.height - (root.topPadding + root.bottomPadding);
                folder: root.folder;
                showFiles: true;
                property int splitPos: osIsWindows ? 8 : 7;
                onAccepted: {
                    if(selectedItem().substring(0, 7) === "file://") {
                        showBook(selectedItem().substring(splitPos), 0);
                    }
                }
                onAborted: pageStack.pop();
            }
        }
    }
}
