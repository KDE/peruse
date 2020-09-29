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
import QtQuick.Layouts 1.4
import QtQuick.Controls 2.12 as QtControls

import org.kde.kirigami 2.13 as Kirigami

import org.kde.peruse 0.1 as Peruse
/**
 * @brief the page shows basic information about the book
 */
Kirigami.ScrollablePage {
    id: root;
    property string categoryName: "bookBasics";
    property QtObject model;
    signal requestCategoryChange(string categoryName);
    title: i18nc("title of the basic book information page", "Your Book At A Glance");
    actions {
        main: saveBookAction;
    }
    Kirigami.Action {
        id: saveBookAction;
        text: i18nc("Saves the book to a file on disk", "Save Book");
        iconName: "document-save";
        onTriggered: root.model.saveBook();
        enabled: root.model ? root.model.hasUnsavedChanges : false;
    }
    ColumnLayout {
        spacing: Kirigami.Units.smallSpacing;
        Item {
            id: bookCover;
            Layout.alignment: Qt.AlignHCenter;
            width: Kirigami.Units.gridUnit * 10;
            height: width;
            Rectangle {
                id: coverOutline;
                anchors.centerIn: coverImage;
                width: Math.max(coverImage.paintedWidth, Kirigami.Units.iconSizes.large) + Kirigami.Units.smallSpacing * 2;
                height: Math.max(coverImage.paintedHeight, Kirigami.Units.iconSizes.large) + Kirigami.Units.smallSpacing * 2;
                color: Kirigami.Theme.backgroundColor;
                border {
                    width: 2;
                    color: Kirigami.Theme.textColor;
                }
                radius: 2;
            }
            Kirigami.Icon {
                id: coverImage;
                anchors {
                    fill: parent;
                    margins: Kirigami.Units.largeSpacing;
                }
                source: bookModel.filename === "" ? "" : "image://comiccover/" + bookModel.filename;
                placeholder: "application-vnd.oasis.opendocument.text";
                fallback: "paint-unknown";
            }
        }
        Item { width: parent.width; height: Kirigami.Units.gridUnit * 3; }
        Kirigami.FormLayout {
            Layout.fillWidth: true;
            QtControls.Label {
                Kirigami.FormData.label: i18nc("The descriptive label for a label which displays the title of the book", "Book Title:");
                text: root.model ? root.model.title : "";
            }
            Kirigami.LinkButton {
                Kirigami.FormData.label: i18nc("The descriptive label for a link which shows the number of pages in the book", "Pages:");
                text: i18nc("A link which when clicked shows the book pages editor", "%1 total pages", root.model ? root.model.pageCount : 0);
                onClicked: root.requestCategoryChange("book");
            }
        }
    }
}
