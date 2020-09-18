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
 * @brief the page that deals with editing the book.
 * 
 * This is primarily a list of pages that can be moved around. These are inside
 * Kirigami ListSwipeItems.
 * 
 * This also has a button to add pages, which calls up AddPageSheet.
 * And a button to edit the book metadata, which calls up BookMetainfoPage.
 */
Kirigami.ScrollablePage {
    id: root;
    property string categoryName: "book";
    title: i18nc("title of the main book editor page", "Editing %1", bookModel.title == "" ? root.filename : bookModel.title);
    property string filename;

    actions {
        left: addPageSheet.opened ? null : saveBookAction;
        main: addPageSheet.opened ? closeAddPageSheetAction : defaultMainAction;
        right: addPageSheet.opened ? null : addPageAction;
    }
    Kirigami.Action {
        id: saveBookAction;
        text: i18nc("Saves the book to a file on disk", "Save Book");
        iconName: "document-save";
        onTriggered: bookModel.saveBook();
        enabled: bookModel.hasUnsavedChanges;
    }
    Kirigami.Action {
        id: addPageAction;
        text: i18nc("adds a new page at the end of the book", "Add Page");
        iconName: "list-add";
        onTriggered: addPage(bookModel.pageCount);
    }
    Kirigami.Action {
        id: defaultMainAction;
        text: i18nc("causes a dialog to show in which the user can edit the meta information for the entire book", "Edit Metainfo");
        iconName: "document-edit";
        onTriggered: pageStack.push(editMetaInfo);
    }
    Kirigami.Action {
        id: closeAddPageSheetAction;
        text: i18nc("closes the add page sheet", "Do not Add a Page");
        iconName: "dialog-cancel";
        onTriggered: addPageSheet.close();
    }

    function addPage(afterWhatIndex) {
        addPageSheet.addPageAfter = afterWhatIndex;
        addPageSheet.open();
    }

    ListView {
        id: bookList;
        model: Peruse.ArchiveBookModel {
            id: bookModel;
            qmlEngine: globalQmlEngine;
            readWrite: true;
            filename: root.filename;
        }
        Component {
            id: editMetaInfo;
            BookMetainfoPage {
                model: bookModel;
            }
        }
        Component {
            id: editBookPage;
            BookPage {
                model: bookModel;
                onSave: {
                   bookList.updateTitle(index, currentPage.title(""));
                }
            }
        }
        function updateTitle(index, title) {
            //Need to add feature to update data here.
        }

        delegate: Kirigami.SwipeListItem {
            id: listItem;
            height: Kirigami.Units.iconSizes.huge + Kirigami.Units.smallSpacing * 2;
            supportsMouseEvents: true;
            onClicked: ;
            actions: [
                Kirigami.Action {
                    text: i18nc("swap the position of this page with the previous one", "Move Up");
                    iconName: "go-up"
                    onTriggered: { bookModel.swapPages(model.index, model.index - 1); }
                    enabled: model.index > 0;
                    visible: enabled;
                },
                Kirigami.Action {
                    text: i18nc("swap the position of this page with the next one", "Move Down");
                    iconName: "go-down"
                    onTriggered: { bookModel.swapPages(model.index, model.index + 1); }
                    enabled: model.index < bookModel.pageCount - 1;
                    visible: enabled;
                },
                Kirigami.Action {
                    text: i18nc("remove the page from the book", "Delete Page");
                    iconName: "list-remove"
                    onTriggered: bookModel.removePage(model.index);
                },
                Kirigami.Action {
                    text: i18nc("add a page to the book after this one", "Add Page After This");
                    iconName: "list-add"
                    onTriggered: root.addPage(model.index);
                },
                Kirigami.Action {
                    text: i18nc("Edit page data such as title, frames, etc.", "Edit Page");
                    iconName: "document-edit";
                    onTriggered: {
                        pageStack.push(editBookPage, { index: model.index, pageUrl: model.url })
                    }
                }

            ]
            Item {
                Layout.fillWidth: true
                Layout.minimumHeight: Kirigami.Units.iconSizes.enormous
                Item {
                    id: bookCover;
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        bottom: parent.bottom;
                    }
                    width: height;
                    Image {
                        id: coverImage;
                        anchors {
                            fill: parent;
                            margins: Kirigami.Units.smallSpacing;
                        }
                        asynchronous: true;
                        fillMode: Image.PreserveAspectFit;
                        source: model.url;
                    }
                }
                QtControls.Label {
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        left: bookCover.right;
                        leftMargin: Kirigami.Units.largeSpacing;
                    }
                    text: model.title;
                }
            }
        }
        Rectangle {
            id: processingBackground;
            anchors.fill: parent;
            opacity: bookModel.processing ? 0.5 : 0;
            Behavior on opacity { NumberAnimation { duration: mainWindow.animationDuration; } }
            MouseArea {
                anchors.fill: parent;
                enabled: parent.opacity > 0;
                onClicked: { }
            }
        }
        QtControls.BusyIndicator {
            anchors {
                horizontalCenter: processingBackground.horizontalCenter;
                top: parent.top
                topMargin: x;
            }
            running: processingBackground.opacity > 0;
            visible: running;
        }
    }

    AddPageSheet {
        id: addPageSheet;
        model: bookModel;
    }
}
