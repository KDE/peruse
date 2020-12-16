/*
 * SPDX-FileCopyrightText: 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick 2.12
import QtQuick.Controls 2.12 as QtControls
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15

import org.kde.kirigami 2.12 as Kirigami

import org.kde.peruse 0.1 as Peruse
import "listcomponents" as ListComponents

/**
 * @brief The page that Peruse opens up on.
 * 
 * The WelcomePage shares some resemblance to the
 * BookShelf pages in that it allows the user to select a comic,
 * but where BookShelf pages are really for discovery and searching
 * through categories and subcategories, the WelcomePage is primarily
 * for selecting the recently opened and new books, which the user is most likely
 * to look at when they want to read.
 * 
 * It uses BookTileTall to show the selectable books, SearchFiedl to search books
 * and Section to indicate a subsection.
 */
Bookshelf {
    id: root;

    property string categoryName: "welcomePage";
    property bool isCurrentContext: isCurrentPage && !applicationWindow().bookOpen
    property real heightBook: Kirigami.Units.gridUnit * 8
    property string searchText: ""
    property bool searching: searchText.length > 0

//     signal bookSelected(string filename, int currentPage);

    function updateRecent() {
        startWithThese.updateRecentlyRead();
    }

    title: i18nc("title of the welcome page", "Welcome");

    actions.main: Kirigami.Action {
        text: i18nc("search in the list of books (not inside the books)", "Search Books");
        iconName: "system-search";
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
                }
            }
        }
    }

    Peruse.CategoryEntriesModel {
        id: recentBooksModel;
    }
    model: peruseConfig.recentlyOpened.length > 0 ? recentBooksModel : contentList.newlyAddedCategoryModel
    function updateRecentlyRead() {
        recentBooksModel.clear();
        for(var i = 0; i < peruseConfig.recentlyOpened.length; ++i) {
            recentBooksModel.appendFakeBook(contentList.bookFromFile(peruseConfig.recentlyOpened[i]), Peruse.CategoryEntriesModel.UnknownRole);
        }
    }
    Connections {
        target: peruseConfig;
        onRecentlyOpenedChanged: root.updateRecentlyRead();
    }
    Connections {
        target: applicationWindow();
        onIsLoadingChanged: {
            if(applicationWindow().isLoading === false) {
                root.updateRecentlyRead();
            }
        }
    }

    pageHeader: Item {
        width: root.width
        height: mainWindow.isLoading ? root.height : Kirigami.Units.gridUnit * 5
        Behavior on height { NumberAnimation { duration: Kirigami.Units.longDuration; } }
        visible: !searching && Window.window.width > Kirigami.Units.gridUnit * 30
        Kirigami.Heading {
            id: appNameLabel;
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.verticalCenter;
            }
            text: "Peruse";
            horizontalAlignment: Text.AlignHCenter;
        }
        QtControls.Label {
            id: appDescriptionLabel;
            anchors {
                top: parent.verticalCenter;
                left: parent.left;
                right: parent.right;
            }
            text: i18nc("application subtitle", "Comic Book Reader");
            horizontalAlignment: Text.AlignHCenter;
        }
        Rectangle {
            anchors.centerIn: parent;
            height: 1;
            color: Kirigami.Theme.textColor;
            width: appDescriptionLabel.paintedWidth;
        }
        Kirigami.Heading {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                margins: Kirigami.Units.largeSpacing
            }
            text: peruseConfig.recentlyOpened.length > 0 ? i18nc("title of list of recently opened books", "Continue Reading") : i18nc("title of list of recently discovered books", "Recently Added")
            level: 2
        }
    }
}
