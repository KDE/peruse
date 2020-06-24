/*
 * Copyright 2018  Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
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

#include "AcbfStyleSheet.h"
#include <QString>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <acbf_debug.h>

using namespace AdvancedComicBookFormat;

class StyleSheet::Private
{
public:
    Private(){}
    QHash<QString, QString> classes;
};

StyleSheet::StyleSheet(Document* parent)
    : QObject(parent)
    , d(new Private)
{
}

StyleSheet::~StyleSheet() = default;

void StyleSheet::toXml(QXmlStreamWriter* writer) {
    writer->writeStartElement(QStringLiteral("style"));
    QStringList contents;
    Q_FOREACH(const QString selector, d->classes.keys())
    {
        contents.append(QStringLiteral("%1 {\n%2\n}").arg(selector, d->classes[selector]));
    }
    writer->writeCharacters(contents.join("\n"));
    writer->writeEndElement();
}

bool StyleSheet::fromXml(QXmlStreamReader *xmlReader)
{
    setContents(xmlReader->readElementText(QXmlStreamReader::IncludeChildElements));
    if (xmlReader->hasError()) {
        qCWarning(ACBF_LOG) << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader->name() << "(" << xmlReader->lineNumber() << ":" << xmlReader->columnNumber() << ") The reported error was:" << xmlReader->errorString();
    }
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created a stylesheet section with"<<d->classes.keys().count()<<"classes";
    return !xmlReader->hasError();
}

QHash<QString, QString> StyleSheet::classes() const
{
    return d->classes;
}
void StyleSheet::setContents(const QString& css)
{
    QStringList classes = css.split('}', Qt::SkipEmptyParts);
    Q_FOREACH(const QString &cssClass, classes)
    {
        QStringList selectorContent = cssClass.split('{', Qt::SkipEmptyParts);
        if (selectorContent.count() == 2) {
            d->classes.insert(selectorContent[0], selectorContent[1]);
        }
    }
}
