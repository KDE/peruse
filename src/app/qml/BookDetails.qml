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

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.kirigami 1.0 as Kirigami
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.peruse 0.1 as Peruse

import "listcomponents" as ListComponents

Kirigami.Page {
    id: root;
    property string file;
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

        dataRepeater.model.clear();
        dataRepeater.model.append({"label": "Author:", value: root.author});
        dataRepeater.model.append({"label": "Publisher:", value: root.publisher});
        dataRepeater.model.append({"label": "Series:", value: root.series});
        dataRepeater.model.append({"label": "Filename:", value: root.filename});
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

    Item {
        width: root.width - (root.leftPadding + root.rightPadding);
        height: root.height - (root.topPadding + root.bottomPadding);
        Column {
            anchors.horizontalCenter: parent.horizontalCenter;
            width: root.width - units.largeSpacing * 2;
            spacing: units.smallSpacing;
            height: childrenRect.height;
            Item {
                id: bookCover;
                anchors {
                    horizontalCenter: parent.horizontalCenter;
                    margins: units.largeSpacing;
                }
                width: Math.min(parent.width - units.largeSpacing * 2, units.iconSizes.enormous + units.largeSpacing * 2);
                height: width;
                Rectangle {
                    anchors.centerIn: coverImage;
                    width: coverImage.paintedWidth + units.smallSpacing * 2;
                    height: coverImage.paintedHeight + units.smallSpacing * 2;
                    color: theme.viewBackgroundColor;
                    border {
                        width: 2;
                        color: theme.viewTextColor;
                    }
                    radius: 2;
                }
                Image {
                    id: coverImage;
                    anchors {
                        fill: parent;
                        margins: units.largeSpacing;
                    }
                    source: (contentList.contentModel.getMimetype(root.filename) === "application/x-cbr") ? "image://comiccover/" + root.filename : "image://preview/" + root.filename
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
                    Kirigami.Label {
                        anchors {
                            top: parent.top;
                            left: parent.left;
                            right: parent.horizontalCenter;
                            bottom: parent.bottom;
                        }
                        verticalAlignment: Text.AlignTop;
                        text: model.label;
                    }
                    Kirigami.Label {
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
                height: deleteButton.height + units.largeSpacing * 2;
                Behavior on height { PropertyAnimation { duration: mainWindow.animationDuration; } }
                states: [
                    State {
                        name: "confirmDelete";
                        PropertyChanges { target: deleteButton; opacity: 0; }
                        PropertyChanges { target: deleteConfirmBase; opacity: 1; }
                        PropertyChanges { target: deleteBase; height: deleteConfirmBase.height; }
                    }
                ]
                PlasmaComponents.Button {
                    id: deleteButton;
                    text: "Delete from device";
                    anchors {
                        top: parent.top;
                        topMargin: units.largeSpacing;
                        horizontalCenter: parent.horizontalCenter;
                    }
                    iconName: "edit-delete";
                    onClicked: deleteBase.state = "confirmDelete";
                    Behavior on opacity { PropertyAnimation { duration: mainWindow.animationDuration; } }
                }
                Item {
                    id: deleteConfirmBase;
                    opacity: 0;
                    width: root.width;
                    Behavior on opacity { PropertyAnimation { duration: mainWindow.animationDuration; } }
                    height: yesDelete.height + confirmDeleteLabel.height + units.largeSpacing * 2 + units.smallSpacing;
                    Kirigami.Label {
                        id: confirmDeleteLabel;
                        anchors {
                            top: parent.top;
                            topMargin: units.largeSpacing;
                            left: parent.left;
                            right: parent.right;
                        }
                        height: paintedHeight;
                        wrapMode: Text.WordWrap;
                        horizontalAlignment: Text.AlignHCenter;
                        text: "Are you sure you want to delete this from your device?";
                    }
                    PlasmaComponents.Button {
                        id: yesDelete;
                        anchors {
                            top: confirmDeleteLabel.bottom;
                            topMargin: units.smallSpacing;
                            right: parent.horizontalCenter;
                            rightMargin: (parent.width - width) / 4;
                        }
                        text: "Yes, really delete";
                        iconName: "dialog-ok";
                        onClicked: {
                            contentList.removeBook(root.file, true);
                            mainWindow.pageStack.pop();
                        }
                    }
                    PlasmaComponents.Button {
                        anchors {
                            top: confirmDeleteLabel.bottom;
                            topMargin: units.smallSpacing;
                            left: parent.horizontalCenter;
                            leftMargin: (parent.width - width) / 4;
                        }
                        text: "No, cancel delete";
                        iconName: "dialog-cancel";
                        onClicked: deleteBase.state = "";
                    }
                }
            }
        }
    }
}
