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

#ifndef TEXTDOCUMENTEDITOR_H
#define TEXTDOCUMENTEDITOR_H

#include <QObject>
#include <QPoint>

#include <memory>

/**
 * \brief A helper object for editing text documents using e.g. a QML TextEdit or a TextArea
 */
class TextDocumentEditor : public QObject {
    Q_OBJECT
    Q_PROPERTY(QObject* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)
public:
    explicit TextDocumentEditor(QObject *parent = nullptr);
    virtual ~TextDocumentEditor();

    QObject* textDocument() const;
    void setTextDocument(QObject *textDocument);
    Q_SIGNAL void textDocumentChanged();

    Q_INVOKABLE QPoint linkStartEnd(int cursorPosition);
    Q_INVOKABLE QString linkText(int cursorPosition);
    Q_INVOKABLE QString linkHref(int cursorPosition);

    /**
     * Returns the paragraphs of text found in the body of the text document
     * @return A list of paragraphs of text
     */
    Q_INVOKABLE QStringList paragraphs() const;
private:
    class Private;
    std::unique_ptr<Private> d;
};

#endif//TEXTDOCUMENTEDITOR_H
