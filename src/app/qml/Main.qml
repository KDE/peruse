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

import QtCore
import QtQuick
import QtQuick.Window
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami

PeruseMain {
    id: root;
    width: Math.min(Screen.desktopAvailableWidth * 0.6, Kirigami.Units.gridUnit * 80);
    height: Math.min(Screen.desktopAvailableHeight * 0.7, Kirigami.Units.gridUnit * 60);

    function openOther() {
        openDlg.open();
    }

    FileDialog {
        id: openDlg;
        title: i18nc("@title:window standard file open dialog used to open a book not in the collection", "Please Choose a Comic to Open");
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        property int splitPos: Qt.platform.os === "windows" ? 8 : 7;
        onAccepted: {
            if(openDlg.selectedFile.toString().substring(0, 7) === "file://") {
                root.showBook(openDlg.selectedFile.toString().substring(splitPos), 0);
            }
        }
        onRejected: {
            // Just do nothing, we don't really care...
        }
    }
}
