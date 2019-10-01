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
    // Fired when the viewer has completed loading the file.
    // Success is true if this was successful, and false otherwise.
    signal loadingCompleted(bool success);
    property string file;
    property string title;
    property int currentPage;
    property bool rtlMode: false;
    property int zoomMode: 0;
    property int pageCount;
    property var pagesModel;
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
}
