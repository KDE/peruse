/*
 * Copyright (C) 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
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
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4 as QtControls

import org.kde.kirigami 1.0 as Kirigami
import org.kde.peruse 0.1 as Peruse

Kirigami.Page {
    id: root;
    property string categoryName: "createNewBook";
    title: i18nc("title of the new book creation page", "Create New Book");

    actions {
        main: Kirigami.Action {
            text: i18nc("Accept button which will create a new book", "Create Book");
            iconName: "dialog-ok";
            property int splitPos: osIsWindows ? 8 : 7;
            onTriggered: {
                var filename = newBookModel.createBook(getFolderDlg.folder.toString().substring(splitPos), titleEdit.text, getCoverDlg.fileUrl.toString().substring(splitPos));
                if(filename.length > 0)
                {
                    mainWindow.openBook(filename);
                }
            }
        }
    }
    Peruse.ArchiveBookModel {
        id: newBookModel;
        qmlEngine: globalQmlEngine;
    }

    Column {
        id: contentColumn;
        width: root.width - (root.leftPadding + root.rightPadding);
        height: childrenRect.height;
        spacing: Kirigami.Units.smallSpacing;
        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the book title", "Title");
        }
        QtControls.TextField {
            id: titleEdit;
            width: parent.width;
            text: i18nc("Default name for new books", "Untitled");
        }

        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the file system location for the book", "Folder");
        }
        Kirigami.Label {
            width: parent.width - getFolderButton.width;
            text: getFolderDlg.folder;
            QtControls.Button {
                id: getFolderButton;
                anchors.left: parent.right;
                height: parent.height;
                width: height;
                iconName: "folder-open"
                onClicked: getFolderDlg.open();
            }
            FileDialog {
                id: getFolderDlg;
                title: i18nc("Title of a folder dialog used to select the location of a new book", "Please choose the location for the book");
                folder: mainWindow.homeDir();
                selectFolder: true;
            }
        }

        Kirigami.Heading {
            width: parent.width - getCoverButton.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the cover image for the book", "Cover Image");
            QtControls.Button {
                id: getCoverButton;
                anchors.left: parent.right;
                height: getFolderButton.height;
                width: height;
                iconName: "folder-open"
                onClicked: getCoverDlg.open();
            }
            FileDialog {
                id: getCoverDlg;
                title: i18nc("Title of a file dialog used to select the cover image for a new book", "Please choose your cover image");
                folder: mainWindow.homeDir();
                nameFilters: [ "JPEG images (*.jpg, *.jpeg)", "All files (*)" ];
            }
        }
        Item {
            width: parent.width;
            height: Kirigami.Units.iconSizes.enormous + Kirigami.Units.smallSpacing;
            Image {
                anchors.centerIn: parent;
                height: Kirigami.Units.iconSizes.enormous;
                width: Kirigami.Units.iconSizes.enormous;
                asynchronous: true;
                fillMode: Image.PreserveAspectFit;
                source: getCoverDlg.fileUrl;
            }
        }
    }
}
