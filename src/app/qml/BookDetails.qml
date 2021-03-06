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

import org.kde.kirigami 2.7 as Kirigami

import org.kde.peruse 0.1 as Peruse
import "listcomponents" as ListComponents

Item {
    id: root;
    property string file;
    height: childrenRect.height;
    onFileChanged: {
        var book = contentList.get(contentList.indexOfFile(file));
        filename = book.readProperty("filename");
        filetitle = book.readProperty("filetitle");
        title = book.readProperty("title");
        series = book.readProperty("series");
        author = book.readProperty("author");
        publisher = book.readProperty("publisher");
        created = book.readProperty("created");
        lastOpenedTime = book.readProperty("lastOpenedTime");
        totalPages = book.readProperty("totalPages");
        currentPage = book.readProperty("currentPage");
        thumbnail = book.readProperty("thumbnail");

        dataRepeater.model.clear();
        dataRepeater.model.append({"label": i18nc("Label for the author field", "Author:"), value: root.author});
        dataRepeater.model.append({"label": i18nc("Label for the publisher field", "Publisher:"), value: root.publisher});
        dataRepeater.model.append({"label": i18nc("Label for the series field", "Series:"), value: root.series});
        dataRepeater.model.append({"label": i18nc("Label for the filename field", "Filename:"), value: root.filename});
    }
    property string filename;
    property string filetitle;
//     property string title;
    property string series;
    property string author;
    property string publisher;
    property date created;
    property date lastOpenedTime;
    property int totalPages;
    property int currentPage;
    property string thumbnail;

    Column {
        anchors.horizontalCenter: parent.horizontalCenter;
        spacing: Kirigami.Units.smallSpacing;
        width: parent.width;
        height: childrenRect.height;
        Item {
            id: bookCover;
            anchors {
                horizontalCenter: parent.horizontalCenter;
                margins: Kirigami.Units.largeSpacing;
            }
            width: Math.min(parent.width - Kirigami.Units.largeSpacing * 2, Kirigami.Units.iconSizes.enormous + Kirigami.Units.largeSpacing * 2);
            height: width;
            Rectangle {
                anchors.centerIn: coverImage;
                width: coverImage.paintedWidth + Kirigami.Units.smallSpacing * 2;
                height: coverImage.paintedHeight + Kirigami.Units.smallSpacing * 2;
                color: Kirigami.Theme.backgroundColor;
                border {
                    width: 2;
                    color: Kirigami.Theme.textColor;
                }
                radius: 2;
            }
            Image {
                id: coverImage;
                anchors {
                    fill: parent;
                    margins: Kirigami.Units.largeSpacing;
                }
                source: root.thumbnail;
                asynchronous: true;
                fillMode: Image.PreserveAspectFit;
            }
        }
        Repeater {
            id: dataRepeater;
            model: ListModel {}
            delegate: Item {
                id: base;
                width: root.width;
                height: valueLabel.height;
                QtControls.Label {
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.horizontalCenter;
                        bottom: parent.bottom;
                    }
                    verticalAlignment: Text.AlignTop;
                    text: model.label;
                }
                QtControls.Label {
                    id: valueLabel;
                    anchors {
                        top: parent.top;
                        left: parent.horizontalCenter;
                        right: parent.right;
                    }
                    verticalAlignment: Text.AlignTop;
                    height: paintedHeight;
                    wrapMode: Text.WordWrap;
                    text: model.value;
                }
            }
        }
        Item {
            id: deleteBase;
            width: root.width;
            height: deleteButton.height + Kirigami.Units.largeSpacing * 2;
            Behavior on height { NumberAnimation { duration: applicationWindow().animationDuration; } }
            states: [
                State {
                    name: "confirmDelete";
                    PropertyChanges { target: deleteButton; opacity: 0; }
                    PropertyChanges { target: deleteConfirmBase; opacity: 1; }
                    PropertyChanges { target: deleteBase; height: deleteConfirmBase.height; }
                }
            ]
            QtControls.Button {
                id: deleteButton;
                text: i18nc("Spawn inline dialog box to confirm permanent removal of this book", "Delete from Device");
                anchors {
                    top: parent.top;
                    topMargin: Kirigami.Units.largeSpacing;
                    horizontalCenter: parent.horizontalCenter;
                }
                iconName: "edit-delete";
                onClicked: deleteBase.state = "confirmDelete";
                Behavior on opacity { NumberAnimation { duration: applicationWindow().animationDuration; } }
            }
            Item {
                id: deleteConfirmBase;
                opacity: 0;
                width: root.width;
                Behavior on opacity { NumberAnimation { duration: applicationWindow().animationDuration; } }
                height: yesDelete.height + confirmDeleteLabel.height + Kirigami.Units.largeSpacing * 2 + Kirigami.Units.smallSpacing;
                QtControls.Label {
                    id: confirmDeleteLabel;
                    anchors {
                        top: parent.top;
                        topMargin: Kirigami.Units.largeSpacing;
                        left: parent.left;
                        right: parent.right;
                    }
                    height: paintedHeight;
                    wrapMode: Text.WordWrap;
                    horizontalAlignment: Text.AlignHCenter;
                    text: i18nc("Dialog text for delete book dialog", "Are you sure you want to delete this from your device?");
                }
                QtControls.Button {
                    id: yesDelete;
                    anchors {
                        top: confirmDeleteLabel.bottom;
                        topMargin: Kirigami.Units.smallSpacing;
                        right: parent.horizontalCenter;
                        rightMargin: (parent.width - width) / 4;
                    }
                    text: i18nc("Confirmation button for book delete dialog", "Yes, Really Delete");
                    iconName: "dialog-ok";
                    onClicked: {
                        contentList.removeBook(root.file, true);
                        applicationWindow().pageStack.pop();
                    }
                }
                QtControls.Button {
                    anchors {
                        top: confirmDeleteLabel.bottom;
                        topMargin: Kirigami.Units.smallSpacing;
                        left: parent.horizontalCenter;
                        leftMargin: (parent.width - width) / 4;
                    }
                    text: i18nc("Cancellation button or book delete dialog", "No, Cancel Delete");
                    iconName: "dialog-cancel";
                    onClicked: deleteBase.state = "";
                }
            }
        }
    }
}
