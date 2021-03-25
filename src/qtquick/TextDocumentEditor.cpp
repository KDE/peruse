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
        QTextBlock block = d->textDocument->textDocument()->firstBlock();
        while (block.isValid()) {
            QString paragraph;
            QStringList tagStack;
            QTextBlock::iterator it;
            for (it = block.begin(); !(it.atEnd()); ++it) {
                QTextFragment fragment = it.fragment();
                if (fragment.isValid()) {
                    QTextCharFormat format = fragment.charFormat();
                    if (fragment.isValid()) {
                        QStringList endStack;
                        // First set all the tags that ACBF supports, and add the tags we need to close to our tag stack
                        // <strong>...</strong>
                        if (format.fontWeight() == QFont::Bold) {
                            paragraph += "<strong>";
                            endStack << "</strong>";
                        }
                        // <emphasis>...</emphasis>
                        if (format.fontItalic()) {
                            paragraph += "<emphasis>";
                            endStack << "</emphasis>";
                        }
                        // <strikethrough>...</strikethrough>
                        if (format.fontStrikeOut()) {
                            paragraph += "<strikethrough>";
                            endStack << "</strikethrough>";
                        }
                        // <sub>...</sub>
                        if (format.verticalAlignment() == QTextCharFormat::AlignSubScript) {
                            paragraph += "<sub>";
                            endStack << "</sub>";
                        }
                        // <sup>...</sup>
                        if (format.verticalAlignment() == QTextCharFormat::AlignSuperScript) {
                            paragraph += "<sup>";
                            endStack << "</sup>";
                        }
                        // <a href="">...</a>
                        if (format.isAnchor()) {
                            paragraph += QString("<a href=\"%1\">").arg(format.anchorHref().toHtmlEscaped());
                            endStack << "</a>";
                        }

                        // Add in the fragment's text contents
                        paragraph += fragment.text().toHtmlEscaped();

                        // Finally, close our own tags again, from the inside
                        while (endStack.count() > 0) {
                            paragraph += endStack.takeLast();
                        }
                    }
                }
            }
            paragraphs << paragraph;
            block = block.next();
        }
    }
    return paragraphs;
}
