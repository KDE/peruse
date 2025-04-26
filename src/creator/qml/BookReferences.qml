// SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.peruse as Peruse
/**
 * @brief the page which shows the references contained within the book
 */
Kirigami.ScrollablePage {
    id: root;
    property string categoryName: "bookReferences";
    title: i18nc("title text for the references page", "References");
    property QtObject model;

    actions: Kirigami.Action {
        id: newReferenceAction
        text: i18nc("Action which when triggered adds a new reference to the book and opens the reference editor", "New Reference")
        icon.name: "list-add-symbolic"
        onTriggered: {
            var newReference = model.acbfData.references.addReference(
                i18nc("The default ID for new references", "Unnamed Reference"),
                i18nc("The default text for new references", "<p>Enter the body of your reference document here.</p>\n<p>This is what links look like: <a href=\"destination\">Some Link</a></p>"));
            pageStack.push(referenceEditor, { reference: newReference, model: root.model });
        }
    }
    Component {
        id: referenceEditor;
        BookReferenceEditor { }
    }

    ListView {
        id: view

        model: Peruse.FilterProxy {
            filterRole: 259; // TypeRole
            filterInt: 0; // ReferenceType
            sortRole: 258; // OriginalIndexRole
            sourceModel: Peruse.IdentifiedObjectModel { document: root.model.acbfData; }
        }

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property QtObject object
            required property string objectId
            required property int originalIndex

            text: objectId

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Delegates.SubtitleContentItem {
                    itemDelegate: delegate
                    subtitle: delegate.object.paragraphs.length > 0 ? delegate.object.paragraphs.join("\n") : ""
                }

                Controls.ToolButton {
                    text: i18nc("swap the position of this page with the previous one", "Move Up");
                    icon.name: "go-up"
                    enabled: delegate.originalIndex > 0;
                    visible: enabled;
                    onClicked: { root.model.acbfData.references.swapReferencesByIndex(delegate.originalIndex, delegate.originalIndex - 1); }
                }

                Controls.ToolButton {
                    text: i18nc("swap the position of this page with the next one", "Move Down");
                    icon.name: "go-down"
                    enabled: delegate.originalIndex < view.count - 1;
                    visible: enabled;
                    onClicked: root.model.acbfData.references.swapReferencesByIndex(delegate.originalIndex, delegate.originalIndex + 1);
                }

                Controls.ToolButton {
                    icon.name: "document-edit";
                    onClicked: root.Controls.ApplicationWindow.window.pageStack.push(referenceEditor, {
                        reference: delegate.object,
                        model: root.model,
                    });
                }
            }
        }

        Kirigami.PlaceholderMessage {
            text: i18n("There is no reference information in your book yet")
            explanation: i18n("References are collections of paragraphs of text, which you can fill with simple formatting (strong, emphasis, and other simple layouting like that), as well as links to both internal things (other references or one of the pieces of binary data), or to an external resource, such as a website.")
            visible: view.count === 0
            helpfulAction: newReferenceAction
            anchors.centerIn: parent
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
