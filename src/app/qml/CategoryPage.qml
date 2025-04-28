
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only or LGPL-3.0-only or LicenseRef-KDE-Accepted-LGPL

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls

import org.kde.kitemmodels as KItemModels
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.peruse as Peruse

Kirigami.ScrollablePage {
    id: root

    required property string categoryName
    required property string emptyPlaceholder
    property alias model: sortFilterModel.sourceModel

    signal bookSelected(string filename, int currentPage)

    actions: Kirigami.Action {
        displayComponent: Kirigami.SearchField {
            onTextChanged: sortFilterModel.filterString = text;
        }
    }

    ListView {
        id: list

        currentIndex: -1

        model: KItemModels.KSortFilterProxyModel {
            id: sortFilterModel

            filterRoleName: 'title'
            filterCaseSensitivity: Qt.CaseInsensitive
        }

        section {
            property: 'title'
            criteria: ViewSection.FirstCharacter
            delegate: Kirigami.ListSectionHeader {
                required property string section
                width: ListView.view.width
                text: section
            }
        }

        delegate: Delegates.RoundedItemDelegate {
            id: categoryDelegate

            required property int index
            required property string title
            required property Peruse.CategoryEntriesModel categoryEntriesModel
            required property int categoryEntriesCount

            text: title

            contentItem: Delegates.SubtitleContentItem {
                itemDelegate: categoryDelegate
                subtitle: i18ncp("@info", "%1 book", "%1 books", categoryDelegate.categoryEntriesCount)
            }

            onClicked: {
                const bookshelf = root.Controls.ApplicationWindow.window.pageStack.push(Qt.createComponent("org.kde.peruse.app", "Bookshelf"), {
                    title: categoryDelegate.title,
                    model: categoryDelegate.categoryEntriesModel
                });
                bookshelf.onBookSelected.connect((filename, currentPage) => root.bookSelected(filename, currentPage))
            }
        }

        Kirigami.PlaceholderMessage {
            id: placeholderMessage
            width: parent.width - Kirigami.Units.gridUnit * 4
            visible: list.count === 0
            anchors.centerIn: parent
            text: sortFilterModel.filterString.length > 0 ? i18nc("Placeholder Text when there are no comics in the library that match the filter", "No matches") : root.emptyPlaceholder
        }
    }
}
