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

#include "AcbfDocument.h"
#include "AcbfStyle.h"
#include "acbf_debug.h"

#include <QString>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

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
    writer->writeCharacters("");  // to ensure we close the tag correctly and don't end up with a <p />
    const QString styleText = contents.join("\n");
    writer->device()->write(styleText.toUtf8().constData(), styleText.toUtf8().length());
    writer->writeEndElement();
}

bool StyleSheet::fromXml(QXmlStreamReader *xmlReader, const QString& xmlData)
{
    int startPoint = xmlReader->characterOffset();
    int endPoint{startPoint};
    while(xmlReader->readNext()) {
        if (xmlReader->isEndElement() && xmlReader->name() == QStringLiteral("style")) {
            endPoint = xmlReader->characterOffset();
            break;
        }
    }
    setContents(xmlData.mid(startPoint, endPoint - startPoint - 8));
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

static void imposeStyle(Style* imposeThis, Style* onThis) {
    if (imposeThis && onThis) {
        if (!imposeThis->color().isEmpty()) {
            onThis->setColor(imposeThis->color());
        }
        if (!imposeThis->fontFamily().isEmpty()) {
            onThis->setFontFamily(imposeThis->fontFamily());
        }
        if (!imposeThis->fontStyle().isEmpty()) {
            onThis->setFontStyle(imposeThis->fontStyle());
        }
        if (!imposeThis->fontWeight().isEmpty()) {
            onThis->setFontWeight(imposeThis->fontWeight());
        }
        if (!imposeThis->fontStretch().isEmpty()) {
            onThis->setFontStretch(imposeThis->fontStretch());
        }
    }
}

QObject * AdvancedComicBookFormat::StyleSheet::style(const QString& element, const QString& type, bool inverted)
{
// 0) Apply the * style
    Style* everyStyle{nullptr};
// 1) Apply the style for the element, if one exists (e.g. \<text-area\>)
    Style* elementStyle{nullptr};
// 2) If there is a type specified, apply the style for the specified type, if one exists (e.g. \<text-area type=\"commentary\">)
    Style* typeStyle{nullptr};
// 3) If the element is marked as inverted, apply that element's inverted style, if one exists (e.g. \<text-area inverted=true\>)
    Style* invertedStyle{nullptr};
// 4) If the element is both a specified type and marked as inverted, apply the style for that type's inverted style, if one exists (e.g. \<text-area type=\"thought\" inverted=true\>)
    Style* invertedTypeStyle{nullptr};

    for( QObject* obj: d->styles) {
        Style* aStyle = qobject_cast<Style*>(obj);
        if (aStyle->element() == element && aStyle->type() == type && aStyle->inverted() == true) {
            invertedTypeStyle = aStyle;
        } else if (aStyle->element() == element && aStyle->type() == type && aStyle->inverted() == false) {
            invertedStyle = aStyle;
        } else if (aStyle->element() == element && aStyle->type() == type) {
            typeStyle = aStyle;
        } else if (aStyle->element() == element && aStyle->type().isEmpty()) {
            elementStyle = aStyle;
        } else if (aStyle->element() == QStringLiteral("*")) {
            everyStyle = aStyle;
        }
    }
    Style* aStyle = new Style(this);
    imposeStyle(everyStyle, aStyle);
    imposeStyle(elementStyle, aStyle);
    imposeStyle(typeStyle, aStyle);
    if (inverted) {
        imposeStyle(invertedStyle, aStyle);
        imposeStyle(invertedTypeStyle, aStyle);
    }
    qCDebug(ACBF_LOG) << aStyle->fontFamily() << aStyle->color();
    return elementStyle;
}

void StyleSheet::setContents(const QString& css)
{
    QVector<QStringView> classes = QStringView{css}.split('}', Qt::SkipEmptyParts);
    for(QStringView cssClass : classes)
    {
        Style* newStyle = new Style(this);
        if (newStyle->fromString(cssClass.trimmed())) {
            d->addStyle(newStyle);
        }
    }
}
