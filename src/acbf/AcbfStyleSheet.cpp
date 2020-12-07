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

#include "AcbfStyle.h"
#include "acbf_debug.h"

using namespace AdvancedComicBookFormat;

class StyleSheet::Private
{
public:
    Private(StyleSheet* qq)
        : q(qq)
    {}
    StyleSheet* q;
    QObjectList styles;

    void addStyle(Style* style) {
        styles << style;
        QObject::connect(style, &Style::styleDataChanged, q, &StyleSheet::stylesChanged);
        QObject::connect(style, &QObject::destroyed, q, [this, style](){ styles.removeAll(style); Q_EMIT q->stylesChanged(); });
        q->stylesChanged();
    }
};

StyleSheet::StyleSheet(Document* parent)
    : QObject(parent)
    , d(new Private(this))
{
    static const int typeId = qRegisterMetaType<StyleSheet*>("StyleSheet*");
    Q_UNUSED(typeId);
}

StyleSheet::~StyleSheet() = default;

void StyleSheet::toXml(QXmlStreamWriter* writer) {
    writer->writeStartElement(QStringLiteral("style"));
    QStringList contents;
    for(QObject* object : d->styles)
    {
        Style* style = qobject_cast<Style*>(object);
        if (style) {
            contents.append(style->toString());
        } else {
            qCWarning(ACBF_LOG) << "We somehow have an entry in our list of styles that is not a Style object, this really should not be possible. The object in question is:" << object;
        }
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
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created a stylesheet section with"<<d->styles.count()<<"classes";
    return !xmlReader->hasError();
}

QObjectList StyleSheet::styles() const
{
    return d->styles;
}

Style * StyleSheet::addStyle()
{
    Style* newStyle = new Style(this);
    d->addStyle(newStyle);
    return newStyle;
}

void StyleSheet::setContents(const QString& css)
{
    QVector<QStringRef> classes = css.splitRef('}', Qt::SkipEmptyParts);
    for(QStringRef cssClass : classes)
    {
        Style* newStyle = new Style(this);
        if (newStyle->fromString(cssClass.trimmed())) {
            d->addStyle(newStyle);
        }
    }
}
