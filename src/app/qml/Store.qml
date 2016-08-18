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
import QtQuick.Dialogs 1.0

import org.kde.kirigami 1.0 as Kirigami
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

import org.kde.newstuff 1.0 as NewStuff

import org.kde.peruse 0.1 as Peruse

import "listcomponents" as ListComponents

Kirigami.Page {
    id: root;
    property string categoryName: "storePage";
    title: i18nc("title of the book store page", "Book Store");

    Item {
        width: root.width - (root.leftPadding + root.rightPadding);
        height: root.height - (root.topPadding + root.bottomPadding);

        NewStuff.NewStuffList {
            anchors.fill: parent;
            configFile: peruseConfig.newstuffLocation;
            onMessage: console.log("KNS Message: " + message);
            onIdleMessage: console.log("KNS Idle: " + message);
            onBusyMessage: console.log("KNS Busy: " + message);
            onErrorMessage: console.log("KNS Error: " + message);
        }
    }
}
