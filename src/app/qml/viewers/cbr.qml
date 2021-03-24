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
import QtQuick.Controls 2.12 as QQC2
import org.kde.kirigami 2.13 as Kirigami

import org.kde.peruse 0.1 as Peruse
/**
 * @brief a ViewerBase for CBR style books.
 * 
 * It is called from Book when the opened book is one of the following files:
 * - application/x-cbz
 * - application/x-cbr
 * - application/vnd.comicbook+zip
 * - application/vnd.comicbook+rar
 */
ViewerBase {
    id: root;
    title: imageBrowser.model.title;
    pagesModel: imageBrowser.model;
    pageCount: imageBrowser.model.pageCount;
    onRtlModeChanged: {
        if(rtlMode === true) {
            imageBrowser.layoutDirection = Qt.RightToLeft;
        }
        else {
            imageBrowser.layoutDirection = Qt.LeftToRight;
        }
        root.restoreCurrentPage();
    }
    onRestoreCurrentPage: {
        // This is un-pretty, quite obviously. But thanks to the ListView's inability to
        // stay in place when the geometry changes, well, this makes things simple.
        imageBrowser.positionViewAtIndex(imageBrowser.currentIndex, ListView.Center);
    }

    hasFrames: true;
    onNextFrame: imageBrowser.goNextFrame();
    onPreviousFrame: imageBrowser.goPreviousFrame();

    onCurrentPageChanged: {
        if(currentPage !== imageBrowser.currentIndex) {
            pageChangeAnimation.running = false;
            var currentPos = imageBrowser.contentX;
            var newPos;
            imageBrowser.positionViewAtIndex(currentPage, ListView.Center);
            imageBrowser.currentIndex = currentPage;
            newPos = imageBrowser.contentX;
            pageChangeAnimation.from = currentPos;
            pageChangeAnimation.to = newPos;
            pageChangeAnimation.running = true;
        }
    }
    NumberAnimation { id: pageChangeAnimation; target: imageBrowser; property: "contentX"; duration: applicationWindow().animationDuration; easing.type: Easing.InOutQuad; }

    viewerActions: [
        Kirigami.Action {
            shortcut: "Tab";
            visible: false;
            onTriggered: imageBrowser.switchToNextJump();
            enabled: !Kirigami.Settings.isMobile;
        },
        Kirigami.Action {
            shortcut: "Return";
            visible: false;
            onTriggered: imageBrowser.activateCurrentJump();
            enabled: !Kirigami.Settings.isMobile;
        },
        Kirigami.Action {
            id: translationsAction
            text: i18nc("A submenu which allows the user to chose between translations of the book", "Translations")
            visible: imageBrowser.model.acbfData.metaData.bookInfo.languages.length > 0
            Kirigami.Action {
                text: i18nc("The option used to show no translation should be used", "No Translation")
                onTriggered: imageBrowser.currentLanguage = null
                checked: imageBrowser.currentLanguage === null
                checkable: true
                QQC2.ActionGroup.group: translationSelectionGroup
            }
        }
    ]
    QQC2.ActionGroup { id: translationSelectionGroup }
    Component {
        id: translationActionEntry
        Kirigami.Action {
            id: control
            text: language.language
            visible: language.show
            property QtObject language
            onTriggered: { imageBrowser.currentLanguage = control.language; }
            checked: imageBrowser.currentLanguage && imageBrowser.currentLanguage === control.language
            checkable: true
            QQC2.ActionGroup.group: translationSelectionGroup
        }
    }

    Timer {
        id: initialPageChange;
        interval: applicationWindow().animationDuration;
        running: false;
        repeat: false;
        onTriggered: root.currentPage = imageBrowser.model.currentPage;
    }
    ImageBrowser {
        id: imageBrowser;
        anchors.fill: parent;
        property QtObject currentLanguage: null; // this should probably be read out of the system somehow, or we let the user pick a default preferred?
        model: Peruse.ArchiveBookModel {
            filename: root.file;
            qmlEngine: globalQmlEngine;
            onLoadingCompleted: {
                root.loadingCompleted(success);
                if (success) {
                    initialPageChange.start();
                    for (var i = 0 ; i < imageBrowser.model.acbfData.metaData.bookInfo.languages.length ; ++i) {
                        var language = imageBrowser.model.acbfData.metaData.bookInfo.languages[i];
                        var action = translationActionEntry.createObject(translationsAction, {language: language});
                        translationsAction.children.push(action);
                    }
                }
            }
        }
        onCurrentIndexChanged: {
            if(root.currentPage !== currentIndex) {
                root.currentPage = currentIndex;
            }
        }
        onGoNextPage: root.goNextPage();
        onGoPreviousPage: root.goPreviousPage();
        onGoPage: root.goPage(pageNumber);
        onActivateExternalLink: root.activateExternalLink(link);
        imageWidth: root.width;
        imageHeight: root.height;
    }
}
