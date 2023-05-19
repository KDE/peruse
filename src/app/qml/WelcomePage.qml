/*
 * SPDX-FileCopyrightText: 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick 2.12
import QtQuick.Controls 2.12 as QtControls
import QtQuick.Layouts 1.12

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
 * It uses BookTileTall to show the selectable books, SearchField to search books
 * and Section to indicate a subsection.
 */
Bookshelf {
    id: root;

    property string categoryName: "welcomePage";
    property bool isCurrentContext: isCurrentPage && !applicationWindow().bookOpen
    property real heightBook: Kirigami.Units.gridUnit * 8
    property bool isLoading: true;

    function updateRecent() {
        root.updateRecentlyRead();
    }

    title: i18nc("title of the welcome page", "Welcome");

    Peruse.CategoryEntriesModel {
        id: recentBooksModel;
    }
    searchModel: contentList.newlyAddedCategoryModel
    model: root.isLoading ? null : (recentBooksModel.count > 0 ? recentBooksModel : contentList.newlyAddedCategoryModel)
    function updateRecentlyRead() {
        root.isLoading = true;
        recentBooksModel.clear();
        for(var i = 0; i < peruseConfig.recentlyOpened.length; ++i) {
            recentBooksModel.appendFakeBook(contentList.bookFromFile(peruseConfig.recentlyOpened[i]), Peruse.CategoryEntriesModel.UnknownRole);
        }
        root.isLoading = false;
    }
    Connections {
        target: peruseConfig;
        function onRecentlyOpenedChanged() { updateRecentlyReadTimer.start(); }
        property Timer updateRecentlyReadTimer: Timer {
            interval: 500
            running: false
            repeat: false
            onTriggered: { root.updateRecentlyRead(); }
        }
    }
    Connections {
        target: applicationWindow();
        function onIsLoadingChanged() {
            if(applicationWindow().isLoading === false) {
                root.updateRecentlyRead();
            }
        }
    }

    pageHeader: Kirigami.Heading {
        leftPadding: Kirigami.Units.gridUnit
        rightPadding: Kirigami.Units.gridUnit
        topPadding: Kirigami.Units.gridUnit

        level: 2

        text: if (recentBooksModel.count > 0) {
            return i18nc("title of list of recently opened books", "Continue Reading");
        } else {
            return i18nc("title of list of recently discovered books", "Recently Added");
        }
    }
}
