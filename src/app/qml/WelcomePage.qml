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
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

import org.kde.peruse 0.1 as Peruse
import "listcomponents" as ListComponents

MobileComponents.Page {
    id: root;
    color: MobileComponents.Theme.viewBackgroundColor;
    signal bookSelected(string filename, int currentPage);
    Item {
        id: titleContainer;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: mainWindow.isLoading ? (parent.height / 2) : (parent.height / 3);
        Behavior on height { PropertyAnimation { duration: mainWindow.animationDuration; easing.type: Easing.InOutQuad; } }
        PlasmaExtras.Heading {
            id: appNameLabel;
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.verticalCenter;
            }
            text: "Peruse";
            horizontalAlignment: Text.AlignHCenter;
        }
        PlasmaComponents.Label {
            id: appDescriptionLabel;
            anchors {
                top: parent.verticalCenter;
                left: parent.left;
                right: parent.right;
            }
            text: "Comic Book Reader";
            horizontalAlignment: Text.AlignHCenter;
        }
        Rectangle {
            anchors.centerIn: parent;
            height: 1;
            color: "black";
            width: appDescriptionLabel.paintedWidth;
        }
    }
    Flickable {
        id: startWithThese;
        property int mostRecentlyRead0: -1;
        property int mostRecentlyRead1: -1;
        property int mostRecentlyRead2: -1;
        property int mostRecentlyAdded0: -1;
        Connections {
            target: mainWindow;
            onIsLoadingChanged: {
                if(mainWindow.isLoading === false) {
                    startWithThese.mostRecentlyRead0 = contentList.indexOfFile(peruseConfig.recentlyOpened[0]);
                    startWithThese.mostRecentlyRead1 = contentList.indexOfFile(peruseConfig.recentlyOpened[1]);
                    startWithThese.mostRecentlyRead2 = contentList.indexOfFile(peruseConfig.recentlyOpened[2]);
                    // the model might be null, if we haven't actually got any entries... so, let's check that
                    // and just leave the whole thing empty in that case :)
                    if(contentList.newlyAddedCategoryModel) {
                        startWithThese.mostRecentlyAdded0 = 0;
                        newItemsRepeater.model = Math.min(10, Math.floor((contentList.newlyAddedCategoryModel.rowCount() - 1)));
                    }
                }
            }
        }
        anchors {
            top: titleContainer.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        opacity: mainWindow.isLoading ? 0 : 1;
        Behavior on opacity { PropertyAnimation { duration: mainWindow.animationDuration; } }
        contentWidth: width;
        contentHeight: recentItemsColumn.height;
        clip: true;
        Column {
            id: recentItemsColumn;
            width: parent.width;
            height: childrenRect.height;
            ListComponents.Section {
                text: "Recently read";
                width: startWithThese.width;
                height: rread0.height > 0 ? paintedHeight : 0;
                visible: height > 0;
            }
            ListComponents.BookTile {
                id: rread0;
                height: startWithThese.mostRecentlyRead0 > -1 ? neededHeight : 0;
                width: startWithThese.width;
                property QtObject book: contentList.get(startWithThese.mostRecentlyRead0);
                author: book.readProperty("author");
                title: book.readProperty("title");
                filename: book.readProperty("filename");
                categoryEntriesCount: 0;
                currentPage: book.readProperty("currentPage");
                onBookSelected: root.bookSelected(filename, currentPage);
            }
            Row {
                width: startWithThese.width;
                height: childrenRect.height;
                ListComponents.BookTile {
                    id: rread1;
                    height: startWithThese.mostRecentlyRead1 > -1 ? neededHeight : 0;
                    width: rread2.visible ? startWithThese.width / 2 : startWithThese.width;
                    property QtObject book: contentList.get(startWithThese.mostRecentlyRead1);
                    author: book.readProperty("author");
                    title: book.readProperty("title");
                    filename: book.readProperty("filename");
                    categoryEntriesCount: 0;
                    currentPage: book.readProperty("currentPage");
                    onBookSelected: root.bookSelected(filename, currentPage);
                }
                ListComponents.BookTile {
                    id: rread2;
                    height: startWithThese.mostRecentlyRead2 > -1 ? neededHeight : 0;
                    width: startWithThese.width / 2;
                    property QtObject book: contentList.get(startWithThese.mostRecentlyRead2);
                    author: book.readProperty("author");
                    title: book.readProperty("title");
                    filename: book.readProperty("filename");
                    categoryEntriesCount: 0;
                    currentPage: book.readProperty("currentPage");
                    onBookSelected: root.bookSelected(filename, currentPage);
                }
            }
            ListComponents.Section {
                text: "Recently added";
                width: startWithThese.width;
                height: paintedHeight;
            }
            PlasmaComponents.Label {
                visible: !firstRecentlyAddedBook.visible;
                height: visible ? paintedHeight : 0;
                width: startWithThese.width;
                text: "You have no comics on your device. Please put some into your Documents or Downloads folder (for example by downloading some) and they will show up here!";
                wrapMode: Text.WordWrap;
                horizontalAlignment: Text.AlignHCenter;
            }
            ListComponents.BookTile {
                id: firstRecentlyAddedBook;
                visible: book !== fakeBook;
                height: visible ? neededHeight : 0;
                width: startWithThese.width;
                property QtObject book: contentList.newlyAddedCategoryModel ? contentList.newlyAddedCategoryModel.get(startWithThese.mostRecentlyAdded0) : fakeBook;
                author: book.readProperty("author");
                title: book.readProperty("title");
                filename: book.readProperty("filename");
                categoryEntriesCount: 0;
                currentPage: book.readProperty("currentPage");
                onBookSelected: root.bookSelected(filename, currentPage);
            }
            Repeater {
                id: newItemsRepeater;
                model: 0;
                Row {
                    width: startWithThese.width;
                    height: childrenRect.height;
                    ListComponents.BookTile {
                        height: neededHeight;
                        width: startWithThese.width / 2;
                        property QtObject book: contentList.newlyAddedCategoryModel ? contentList.newlyAddedCategoryModel.get((index * 2) + 1) : fakeBook;
                        author: book.readProperty("author");
                        title: book.readProperty("title");
                        filename: book.readProperty("filename");
                        categoryEntriesCount: 0;
                        currentPage: book.readProperty("currentPage");
                        onBookSelected: root.bookSelected(filename, currentPage);
                    }
                    ListComponents.BookTile {
                        visible: filename != "";
                        height: neededHeight;
                        width: startWithThese.width / 2;
                        property QtObject book: contentList.newlyAddedCategoryModel ? contentList.newlyAddedCategoryModel.get((index * 2) + 2) : fakeBook;
                        author: book.readProperty("author");
                        title: book.readProperty("title");
                        filename: book.readProperty("filename");
                        categoryEntriesCount: 0;
                        currentPage: book.readProperty("currentPage");
                        onBookSelected: root.bookSelected(filename, currentPage);
                    }
                }
            }
        }
    }
    Peruse.PropertyContainer {
        id: fakeBook;
        property string author: "unnamed";
        property string title: "unnamed";
        property string filename: "none";
        property string currentPage: "0";
    }
    Item {
        id: loadingProgress;
        anchors {
            top: parent.verticalCenter;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        opacity: mainWindow.isLoading ? 1 : 0;
        Behavior on opacity { PropertyAnimation { duration: mainWindow.animationDuration; } }
        PlasmaComponents.Label {
            anchors {
                bottom: parent.verticalCenter;
                left: parent.left;
                right: parent.right;
            }
            horizontalAlignment: Text.AlignHCenter;
            text: "Please wait while we find your comics...";
        }
        PlasmaComponents.BusyIndicator {
            id: loadingSpinner;
            anchors {
                top: parent.verticalCenter;
                left: parent.left;
                right: parent.right;
            }
            running: mainWindow.isLoading;
        }
        PlasmaComponents.Label {
            anchors {
                top: loadingSpinner.bottom;
                left: parent.left;
                right: parent.right;
            }
            horizontalAlignment: Text.AlignHCenter;
            text: contentList.count;
        }
    }
}
