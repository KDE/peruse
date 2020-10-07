/*
 * Copyright (C) 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
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
 * @brief the page which shows the references contained within the book
 */
Kirigami.ScrollablePage {
    id: root;
    property string categoryName: "bookReferences";
    title: i18nc("title text for the references page", "References");
    property QtObject model;

    actions {
        main: newReferenceAction;
    }
    Kirigami.Action {
        id: newReferenceAction;
        text: i18nc("Action which when triggered adds a new reference to the book and opens the reference editor", "New Reference");
        icon.name: "list-add";
        onTriggered: {
            var newReference = model.acbfData.references.addReference(
                i18nc("The default ID for new references", "Unnamed Reference"),
                i18nc("The default text for new references", "<p>Enter the body of your reference document here.</p>\n<p>This is what links look like: <a href=\"destination\">Some Link</a></p>"));
            pageStack.push(referenceEditor, { reference: newReference });
        }
    }
    Component {
        id: referenceEditor;
        BookReferenceEditor { }
    }

    Kirigami.CardsListView {
        id: referencesList;
        Layout.fillWidth: true;
        model: root.model.acbfData.references.references;
        header: ColumnLayout {
            width: referencesList.width - Kirigami.Units.largeSpacing * 4;
            visible: referencesList.count === 0;
            Item { height: Kirigami.Units.largeSpacing; Layout.fillWidth: true; }
            RowLayout {
                Layout.fillWidth: true;
                Item { height: Kirigami.Units.gridUnit; Layout.fillWidth: true; }
                Kirigami.AbstractCard {
                    header: Kirigami.Heading {
                        text: i18nc("title text for a card which informs the user there are no references, and what references are", "No References");
                        Layout.fillWidth: true;
                        elide: Text.ElideRight;
                    }
                    contentItem: QtControls.Label {
                        Layout.fillWidth: true;
                        wrapMode: Text.Wrap;
                        text: i18nc("Help text for the references page, when there are no references", "There is no reference information in your book yet. You can create a new reference piece by clicking on the New Reference control. References are collections of paragraphs of text, which you can fill with simple formatting (strong, emphasis, and other simple layouting like that), as well as links to both internal things (other references or one of the pieces of binary data), or to an external resource, such as a website.");
                    }
                }
                Item { height: Kirigami.Units.gridUnit; Layout.fillWidth: true; }
            }
        }
        delegate: Kirigami.AbstractCard {
            header: RowLayout {
            Layout.fillWidth: true;
                Kirigami.Heading {
                    Layout.fillWidth: true;
                    text: modelData.id;
                    elide: Text.ElideRight;
                }
                QtControls.ToolButton {
                    icon.name: "document-edit";
                    onClicked: pageStack.push(referenceEditor, { reference: modelData });
                }
            }
            contentItem: QtControls.Label {
                Layout.fillWidth: true;
                text: modelData.paragraphs;
            }
        }
    }
}
