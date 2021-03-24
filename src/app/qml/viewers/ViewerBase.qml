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
import QtQuick.Controls 2.12 as QtControls
import QtQuick.Layouts 1.12 as QtLayouts
import org.kde.kirigami 2.13 as Kirigami

/**
 * @brief a base for holding the image browser.
 * 
 * It keeps track of the comic data and deals with all sorts
 * of side functions like keeping track of the current page,
 * right to left mode and the controls.
 * 
 * This is slightly different between filetypes.
 */
Item {
    id: root;
    signal goNextPage();
    signal goPreviousPage();
    signal goPage(int pageNumber);
    // Fired when the viewer has completed loading the file.
    // Success is true if this was successful, and false otherwise.
    signal loadingCompleted(bool success);
    property string file;
    property string title;
    property int currentPage: -1;
    property bool rtlMode: false;
    property int zoomMode: 0;
    property int pageCount;
    property var pagesModel;
    property list<QtObject> viewerActions;
    property Component thumbnailComponent;
    signal restoreCurrentPage();

    /**
     * Whether or not the viewer supports frame based navigation
     */
    property bool hasFrames: false;
    signal nextFrame();
    signal previousFrame();

    // This all looks a little silly, however, without this, we can't double-click on the
    // page (which is, these days, used for zooming purposes). It also works around some
    // minor small annoyances, like accidental swiping when the finger doesn't leave the
    // surface of the page when switching occurs.
    function startToggleControls() {
        controlsToggler.start();
    }
    function abortToggleControls() {
        controlsToggler.stop();
    }
    Timer {
        id: controlsToggler;
        interval: 500;
        running: false;
        repeat: false;
        onTriggered: {
            if(applicationWindow().controlsVisible === true) {
                applicationWindow().controlsVisible = false;
            }
            else {
                applicationWindow().controlsVisible = true;
            }
        }
    }

    function activateExternalLink(link) {
        linkActivator.showLink(link);
    }

    Kirigami.OverlaySheet {
        id: linkActivator
        function showLink(link) {
            linkActivator.link = link;
            open();
        }
        showCloseButton: true
        header: QtLayouts.RowLayout {
            Kirigami.Heading {
                text: i18nc("Title for a dialog offering the user to open a link to some external resource", "Open External Link?")
                QtLayouts.Layout.fillWidth: true;
                elide: Text.ElideRight;
            }
            QtControls.ToolButton {
                icon.name: "dialog-ok";
                text: i18nc("label for a button which activates an external link", "Open Link");
                onClicked: {
                    Qt.openUrlExternally(linkActivator.link);
                    linkActivator.close();
                }
            }
        }
        property string link
        QtLayouts.ColumnLayout {
            QtControls.Label {
                text: i18n("The link you just clicked points to somewhere outside of the book. Please check the details of the link below, to make sure that you really do want to open it, or just close this sheet.")
                wrapMode: Text.Wrap
            }
            QtControls.Label {
                id: httpOrFileLink
                // For the http(s) case, if we got to here, we can assume we have been given a reasonably laid out link
                visible: linkActivator.link.toLowerCase().startsWith("http") || linkActivator.link.toLowerCase().startsWith("file:")
                textFormat: Text.PlainText
                QtLayouts.Layout.fillWidth: true
                wrapMode: Text.Wrap
                text: linkActivator.link
            }
            QtControls.Label {
                id: mailtoLink
                visible: linkActivator.link.toLowerCase().startsWith("mailto:")
                textFormat: Text.PlainText
                QtLayouts.Layout.fillWidth: true
                wrapMode: Text.Wrap
                text: {
                    if (theThings.length > 2) {
                        // This is a weird one, and likely means there's things like a BCC or whatnot in there,
                        // so let's just show people the entire link
                        return i18n("Compose an email to %1, based on the following link:\n\n%2", email, linkActivator.link);
                    } else if (subject.length > 0 && body.length > 0) {
                        return i18n("Compose an email for %1 with the subject \"%2\" and the following body:\n\n%3", email, subject, body);
                    } else if (subject.length > 0) {
                        return i18n("Compose an email for %1 with the subject \"%2\"", email, subject);
                    } else {
                        return i18n("Compose email for: %1", email)
                    }
                }
                property string email: linkActivator.link.slice(0, linkActivator.link.indexOf("?") - 1)
                property string subject: getThingValue("subject", theThings)
                property string body: getThingValue("body", theThings)

                property var theThings: linkActivator.link.slice(linkActivator.link.indexOf("?")).split("&")
                // Technically we could just refer to theThings directly, but this way we can use this bindingly
                function getThingValue(whatThing, fromWhat) {
                    var theValue = "";
                    for (var i = 0; i < fromWhat.length; ++i) {
                        if (fromWhat[i].toLowerCase().startsWith(whatThing + "=")) {
                            theValue = fromWhat[i].slice(whatThing.length);
                        }
                    }
                    return theValue;
                }
            }
            QtControls.Label {
                visible: mailtoLink.visible === false && httpOrFileLink.visible === false
                textFormat: Text.PlainText
                QtLayouts.Layout.fillWidth: true
                wrapMode: Text.Wrap
                text: linkActivator.link
            }
        }
    }
}
