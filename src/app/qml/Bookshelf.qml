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

import QtQuick
import QtQuick.Controls as QtControls
import QtQuick.Layouts
import Qt.labs.qmlmodels

import org.kde.kirigami as Kirigami

import org.kde.peruse as Peruse
import "listcomponents" as ListComponents

/**
 * @brief This lays out the books and categories.
 * 
 * It uses BookTileTall for the books and CategoryTileTall
 * for showing the categories. Categories can be selected to open
 * a new bookshelf from the right, showing the entries in that
 * subcategory. This is particularly in use with the folder category.
 * 
 * There is also access to the SearchBox, and it is possible to access
 * a BookTile by press+holding the thumbnail.
 * This holds information about the book.
 */
Kirigami.ScrollablePage {
    id: root;

    signal bookSelected(string filename, int currentPage);

    property bool isCurrentContext: isCurrentPage && !applicationWindow().bookOpen
    property alias pageHeader: contentDirectoryView.header;
    property string categoryName: "bookshelf";
    property QtObject model;
    /**
     * Allows you to override the default behaviour of searching inside the shelf (such as
     * for the welcome page where we want to search the global list)
     */
    property alias searchModel: searchFilterProxy.sourceModel;
    property string sectionRole: "title";
    property int sectionCriteria: ViewSection.FirstCharacter;
    property string searchText: ""
    property bool searching: searchText.length > 0
    property bool isSearching: searchFilterProxy.filterString.length > 0;

    function openBook(index: int): void {
        applicationWindow().contextDrawer.close();
        const whatModel = isSearching ? searchFilterProxy.sourceModel : contentDirectoryView.model;
        const whatIndex = isSearching ? searchFilterProxy.sourceIndex(index) : index;
        if (!whatModel) {
            return;
        }
        if (whatModel.indexIsBook(whatIndex)) {
            const book = whatModel.getBookEntry(whatIndex);
            root.bookSelected(book.filename, book.currentPage);
        } else {
            const catEntry = whatModel.getCategoryEntry(whatIndex);
            applicationWindow().pageStack.push(bookshelf, {
                focus: true,
                title: catEntry.name,
                model: catEntry,
            });
        }
    }

    objectName: "bookshelf";

    actions: Kirigami.Action {
        id: mainShelfAction;
        text: i18nc("search in the list of books (not inside the books)", "Search Books");
        icon.name: "system-search";
        onTriggered: searchBox.activate();
        enabled: root.isCurrentContext;
        displayComponent: Kirigami.SearchField {
            id: searchField
            focus: true
            placeholderText: i18nc("placeholder text for the search field", "Tap and type to search");
            onTextChanged: {
                searchText = text
                if(text.length > 0) {
                    searchTimer.start();
                } else {
                    searchTimer.stop();
                    searchFilterProxy.filterString = "";
                }
            }
            Timer {
                id: searchTimer;
                interval: 250;
                repeat: false;
                running: false;
                onTriggered: searchFilterProxy.filterString = searchField.text;
            }
        }
    }

    GridView {
        id: contentDirectoryView

        model: isSearching ? searchFilterProxy : root.model;
        Peruse.FilterProxy {
            id: searchFilterProxy;
            sourceModel: root.model;
        }
        keyNavigationEnabled: true;
        clip: true;

        Kirigami.PlaceholderMessage {
            id: placeholderMessage
            width: parent.width - (Kirigami.Units.gridUnit * 4)
            visible: contentDirectoryView.count === 0
            anchors.centerIn: parent
            text: i18nc("Placeholder Text when there are no comics in the library that match the filter", "No matches")
        }

        cellWidth: {
            const viewWidth = contentDirectoryView.width - Kirigami.Units.smallSpacing * 2;
            let columns = Math.max(Math.floor(viewWidth / 170), 2);
            return Math.floor(viewWidth / columns);
        }
        cellHeight: {
            if (Kirigami.Settings.isMobile) {
                return cellWidth + Kirigami.Units.gridUnit * 2 + Kirigami.Units.largeSpacing;
            } else {
                return 170 + Kirigami.Units.gridUnit * 2 + Kirigami.Units.largeSpacing
            }
        }

        currentIndex: -1

        QtControls.Menu {
            id: contextMenu

            property Peruse.bookEntry currentBook

            QtControls.MenuItem {
                text: i18nc("@action:inmenu", "Open")
                onTriggered: root.bookSelected(
                    contextMenu.currentBook.filename,
                    contextMenu.bookDetails.currentBook.currentPage
                );
            }

            QtControls.MenuItem {
                text: i18nc("@action:inmenu", "Details")
                onTriggered: {
                    applicationWindow().pageStack.pushDialogLayer(Qt.createComponent("org.kde.peruse.app", "BookInfoPage"), {
                        bookEntry: contextMenu.currentBook
                    });
                }
            }
        }

        delegate: DelegateChooser {
            role: "type"
            DelegateChoice {
                roleValue: "category"
                ListComponents.CategoryTileTall {
                    width: Kirigami.Settings.isMobile ? contentDirectoryView.cellWidth : 170
                    height: contentDirectoryView.cellHeight

                    selected: contentDirectoryView.currentIndex === index
                }
            }

            DelegateChoice {
                roleValue: "book"
                ListComponents.BookTileTall {
                    width: Kirigami.Settings.isMobile ? contentDirectoryView.cellWidth : 170
                    height: contentDirectoryView.cellHeight
                    //author: model.author ? model.author : i18nc("used for the author data in book lists if author is empty", "(unknown)");
                    selected: contentDirectoryView.currentIndex === index
                    onBookSelected: root.bookSelected(filename, currentPage);
                    onPressAndHold: {
                        const whatModel = isSearching ? searchFilterProxy.sourceModel : contentDirectoryView.model;
                        const whatIndex = isSearching ? searchFilterProxy.sourceIndex(index) : index;
                        contextMenu.currentBook = whatModel.getBookEntry(whatIndex);
                        contextMenu.popup();

                        // todo
                        //page.connect.onBookDeleteRequested: {
                        //    contentList.removeBook(fileSelected, true);
                        //    close();
                        //}
                    }
                }
            }
        }
    }
}
