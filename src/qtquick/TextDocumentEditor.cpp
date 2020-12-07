/*
 * Copyright (C) 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "TextDocumentEditor.h"

#include <QQuickTextDocument>
#include <QTextBlock>

class TextDocumentEditor::Private {
public:
    Private() {}
    QQuickTextDocument* textDocument{nullptr};
};

TextDocumentEditor::TextDocumentEditor(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

TextDocumentEditor::~TextDocumentEditor() = default;

QObject * TextDocumentEditor::textDocument() const
{
    return d->textDocument;
}

void TextDocumentEditor::setTextDocument(QObject* textDocument)
{
    if (d->textDocument != textDocument) {
        d->textDocument = qobject_cast<QQuickTextDocument*>(textDocument);
        Q_EMIT textDocumentChanged();
    }
}

QPoint TextDocumentEditor::linkStartEnd(int cursorPosition)
{
    QPoint point;
    if (d->textDocument) {
        QTextBlock block = d->textDocument->textDocument()->findBlock(cursorPosition);
        while (block.isValid() && block.contains(cursorPosition)) {
            // Don't be so heavy on large documents...
            qApp->processEvents();
            QTextBlock::iterator it;
            for (it = block.begin(); !(it.atEnd()); ++it) {
                QTextFragment currentFragment = it.fragment();
                if (currentFragment.isValid() && currentFragment.contains(cursorPosition)) {
                    point.setX(currentFragment.position());
                    point.setY(currentFragment.position() + currentFragment.length());
                    break;
                }
            }
            block = block.next();
        }
    }
    return point;
}

QString TextDocumentEditor::linkText(int cursorPosition)
{
    QString text;
    if (d->textDocument) {
        QTextBlock block = d->textDocument->textDocument()->findBlock(cursorPosition);
        while (block.isValid() && block.contains(cursorPosition)) {
            // Don't be so heavy on large documents...
            qApp->processEvents();
            QTextBlock::iterator it;
            for (it = block.begin(); !(it.atEnd()); ++it) {
                QTextFragment currentFragment = it.fragment();
                if (currentFragment.isValid() && currentFragment.contains(cursorPosition)) {
                    text = currentFragment.text();
                    break;
                }
            }
            if (!text.isEmpty()) {
                break;
            }
            block = block.next();
        }
    }
    return text;
}

QString TextDocumentEditor::linkHref(int cursorPosition)
{
    QString text;
    if (d->textDocument) {
        QTextBlock block = d->textDocument->textDocument()->findBlock(cursorPosition);
        while (block.isValid() && block.contains(cursorPosition)) {
            // Don't be so heavy on large documents...
            qApp->processEvents();
            QTextBlock::iterator it;
            for (it = block.begin(); !(it.atEnd()); ++it) {
                QTextFragment currentFragment = it.fragment();
                if (currentFragment.isValid() && currentFragment.contains(cursorPosition)) {
                    text = currentFragment.charFormat().anchorHref();
                    break;
                }
            }
            if (!text.isEmpty()) {
                break;
            }
            block = block.next();
        }
    }
    return text;
}

QStringList TextDocumentEditor::paragraphs() const
{
    QStringList paragraphs;
    if (d->textDocument) {
        // This is some FUN HEURISTIC STUFF based on QTextDocument's internals, so yay
        // QTextDocument's toHtml function spits out something which potentially has a bunch of header
        // and footer info, but we only want the paragraphs and nothing else - that is, only things
        // that are inside the body tag (which might have more things)
        paragraphs = d->textDocument->textDocument()->toHtml().split("\n");
        QMutableStringListIterator it(paragraphs);
        // First, remove the header bit, which always ends with the string below
        static const QString headerEnd{"</style></head><body"};
        while (it.hasNext()) {
            const QString paragraph = it.next();
            it.remove();
            if (paragraph.startsWith(headerEnd)) {
                break;
            }
        }
        // Then remove the last bit of the last item, which is always just </body></html>
        QString last = paragraphs.takeLast();
        paragraphs << last.left(last.length() - 14);
    }
    return paragraphs;
}
