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
import QtQuick.Controls 1.4 as QtControls

import org.kde.kirigami 2.1 as Kirigami

Item {
    id: root;
    property bool selected: false;
    property alias title: bookTitle.text;
    property string author;
    property string publisher;
    property alias filename: bookFile.text;
    property alias thumbnail: coverImage.source;
    property int categoryEntriesCount;
    property string currentPage;
    property string totalPages;
    signal bookSelected(string filename, int currentPage);
    signal bookDeleteRequested();

    property int neededHeight: bookCover.height;// + bookAuthorLabel.height + bookFile.height + Kirigami.Units.smallSpacing * 4;
    visible: height > 1;
    enabled: visible;
    clip: true;
    Rectangle {
        anchors.fill: parent;
        color: Kirigami.Theme.highlightColor;
        opacity: root.selected ? 1 : 0;
        Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
    }
    Item {
        id: bookCover;
        anchors {
            top: parent.top;
            left: parent.left;
        }
        width: root.width / 3;
        height: width * 1.5;
        Image {
            id: coverImage;
            anchors {
                fill: parent;
                margins: Kirigami.Units.smallSpacing;
            }
            asynchronous: true;
            fillMode: Image.PreserveAspectFit;
        }
        MouseArea {
            anchors.fill: parent;
            onClicked: root.bookSelected(root.filename, root.currentPage);
        }
    }
    Kirigami.Heading {
        id: bookTitle;
        anchors {
            top: parent.top;
            leftMargin: Kirigami.Units.smallSpacing;
            left: bookCover.right;
            right: parent.right;
        }
        maximumLineCount: 1;
        elide: Text.ElideMiddle;
        font.weight: Font.Bold;
        MouseArea {
            anchors.fill: parent;
            onClicked: root.bookSelected(root.filename, root.currentPage);
        }
        Rectangle {
            anchors {
                left: parent.left;
                top: parent.baseline;
                topMargin: 2;
            }
            height: 2;
            width: parent.paintedWidth;
            color: Kirigami.Theme.linkColor;
        }
    }
    Kirigami.Label {
        id: bookAuthorLabel;
        anchors {
            top: bookTitle.bottom;
            left: bookCover.right;
            leftMargin: Kirigami.Units.smallSpacing;
        }
        width: paintedWidth;
        text: "Author";
        font.bold: true;
    }
    Kirigami.Label {
        id: bookAuthor;
        anchors {
            top: bookTitle.bottom;
            left: bookAuthorLabel.right;
            leftMargin: Kirigami.Units.smallSpacing;
            right: parent.right;
        }
        elide: Text.ElideRight;
        text: root.author === "" ? "(unknown)" : root.author;
        opacity: (text === "(unknown)" || text === "") ? 0.3 : 1;
    }
    Kirigami.Label {
        id: bookPublisherLabel;
        anchors {
            top: bookAuthorLabel.bottom;
            left: bookCover.right;
            leftMargin: Kirigami.Units.smallSpacing;
        }
        width: paintedWidth;
        text: "Publisher";
        font.bold: true;
    }
    Kirigami.Label {
        id: bookPublisher;
        anchors {
            top: bookAuthor.bottom;
            left: bookPublisherLabel.right;
            leftMargin: Kirigami.Units.smallSpacing;
            right: parent.right;
        }
        elide: Text.ElideRight;
        text: root.publisher === "" ? "(unknown)" : root.publisher;
        opacity: (text === "(unknown)" || text === "") ? 0.3 : 1;
    }
    Kirigami.Label {
        id: bookFile;
        anchors {
            top: bookPublisherLabel.bottom;
            left: bookCover.right;
            leftMargin: Kirigami.Units.smallSpacing;
            right: parent.right;
        }
        elide: Text.ElideMiddle;
        opacity: 0.3;
        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.8;
        maximumLineCount: 1;
    }
    Item {
        id: descriptionContainer;
        anchors {
            top: bookFile.bottom;
            left: bookCover.right;
            right: parent.right;
            bottom: deleteBase.top;
            margins: Kirigami.Units.smallSpacing;
        }
        Kirigami.Label {
            anchors.fill: parent;
            verticalAlignment: Text.AlignTop;
            text: i18nc("Placeholder text for the book description field when no description is set", "(no description available for this book)");
            opacity: 0.3;
        }
    }
    Item {
        id: deleteBase;
        anchors {
            left: bookCover.right;
            leftMargin: Kirigami.Units.smallSpacing;
            right: parent.right;
            bottom: parent.bottom;
        }
        height: deleteButton.height + Kirigami.Units.smallSpacing * 2;
        Behavior on height { PropertyAnimation { duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
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
            text: i18nc("Spawn inline dialog box to confirm permanent removal of this book", "Delete from device");
            anchors {
                bottom: parent.bottom;
                right: parent.right;
                margins: Kirigami.Units.smallSpacing;
            }
            iconName: "edit-delete";
            onClicked: deleteBase.state = "confirmDelete";
            Behavior on opacity { PropertyAnimation { duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
        }
        Item {
            id: deleteConfirmBase;
            opacity: 0;
            width: parent.width;
            Behavior on opacity { PropertyAnimation { duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; } }
            height: yesDelete.height + confirmDeleteLabel.height + Kirigami.Units.largeSpacing * 2 + Kirigami.Units.smallSpacing;
            Kirigami.Label {
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
                text: i18nc("Confirmation button for book delete dialog", "Yes, really delete");
                iconName: "dialog-ok";
                onClicked: root.bookDeleteRequested();
            }
            QtControls.Button {
                anchors {
                    top: confirmDeleteLabel.bottom;
                    topMargin: Kirigami.Units.smallSpacing;
                    left: parent.horizontalCenter;
                    leftMargin: (parent.width - width) / 4;
                }
                text: i18nc("Cancellation button or book delete dialog", "No, cancel delete");
                iconName: "dialog-cancel";
                onClicked: deleteBase.state = "";
            }
        }
    }
}
