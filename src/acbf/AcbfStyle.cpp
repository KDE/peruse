/*
 * Copyright 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#include "AcbfStyle.h"
#include "acbf_debug.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

using namespace AdvancedComicBookFormat;

class Style::Private {
public:
    Private() {}
    QString element;
    bool inverted{false};
    QString type;
    QString color;
    QStringList fontFamily;
    QString fontStyle;
    QString fontWeight;
    QString fontStretch;
};

Style::Style(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<Style*>("Style*");
    Q_UNUSED(typeId);
    connect(this, &Style::elementChanged, &Style::styleDataChanged);
    connect(this, &Style::invertedChanged, &Style::styleDataChanged);
    connect(this, &Style::typeChanged, &Style::styleDataChanged);
    connect(this, &Style::colorChanged, &Style::styleDataChanged);
    connect(this, &Style::fontFamilyChanged, &Style::styleDataChanged);
    connect(this, &Style::fontStyleChanged, &Style::styleDataChanged);
    connect(this, &Style::fontWeightChanged, &Style::styleDataChanged);
    connect(this, &Style::fontStretchChanged, &Style::styleDataChanged);
}

Style::~Style() = default;

QString Style::toString() const {
    QString contents{d->element};

    // First, let's piece together the selectors (a comma separated list with two potential items, or nothing)
    QStringList selectors;
    if (d->inverted) {
        selectors << QString{"inverted=\"True\""};
    }
    if (!d->type.isEmpty()) {
        selectors << QString{"type=\"%1\""}.arg(d->type);
    }

    // Now, let's add together all of the fields we've got, and then mash them into some curly braces with semicolons between each bit
    QStringList fields;
    if (!d->color.isEmpty()) {
        fields << QString{"color: %1"}.arg(d->color);
    }
    if (d->fontFamily.length() > 0) {
        fields << QString{"font-family: %1"}.arg(d->fontFamily.join(", "));
    }
    if (!d->fontStyle.isEmpty()) {
        fields << QString{"font-style: %1"}.arg(d->fontStyle);
    }
    if (!d->fontWeight.isEmpty()) {
        fields << QString{"font-weight: %1"}.arg(d->fontWeight);
    }
    if (!d->fontStretch.isEmpty()) {
        fields << QString{"font-stretch: %1"}.arg(d->fontStretch);
    }

    if (selectors.length() > 0) {
        contents = QString{"%1[%2] {\n    %3 }"}.arg(d->element).arg(selectors.join(",")).arg(fields.join(";\n    "));
    } else {
        contents = QString{"%1 {\n    %2 }"}.arg(d->element).arg(fields.join(";\n    "));
    }

    return contents;
}

bool Style::fromString(const QString &style)
{
    return fromString(QStringRef(&style));
}

bool Style::fromString(QStringRef style)
{
    bool success{true};
    // Just some parsing helper strings... if we're using qstringrefs, let's not just
    // willy nilly create piles of temporary qstrings if we can avoid it ;)
    static const QString invertedString{"inverted"};
    static const QString typeString{"type"};
    static const QString colorString{"color"};
    static const QString fontFamilyString{"font-family"};
    static const QString fontStyleString{"font-style"};
    static const QString fontWeightString{"font-weight"};
    static const QString fontStretchString{"font-stretch"};
    static const QString colonString{":"};
    static const QString semiColonString{";"};
    static const QString commaString{","};
    static const QString quoteString{"\""};
    static const QString equalsString{"="};
    static const QString squareOpenBracketString{"["};
    static const QString squareEndBracketString{"]"};
    static const QString curlyOpenBracketString{"{"};
    static const QString trueString{"true"};
    static const QString newlineString{"\n"};

    // First, split the name, type, and possible inversion bits out from the parameters
    QVector<QStringRef>  split = style.split(curlyOpenBracketString, Qt::SkipEmptyParts);
    if (split.count() == 2) {

        // Then get the name, type and inversion bits sorted out
        QVector<QStringRef>  splitElement = split.value(0).split(squareOpenBracketString, Qt::SkipEmptyParts);
        if (splitElement.count() > 0) {
            d->element = splitElement.value(0).toString().simplified().trimmed();
            if (splitElement.count() == 2) {
                // There's an end brace in here somewhere, and possibly some spaces, so let's get rid of those
                QStringRef parameters = splitElement.value(1).trimmed();
                while (parameters.endsWith(squareEndBracketString)) {
                    parameters = parameters.left(parameters.length() - 1).trimmed();
                }
                QVector<QStringRef>  splitParameters = parameters.split(commaString, Qt::SkipEmptyParts);
                for (QStringRef ref : splitParameters) {
                    QVector<QStringRef>  splitData = ref.split(equalsString, Qt::SkipEmptyParts);
                    if (splitData.count() == 2) {
                        QStringRef name = splitData.value(0).trimmed();
                        QStringRef value = splitData.value(1);
                        if (value.indexOf(quoteString) > -1) {
                            value = value.chopped(value.lastIndexOf(quoteString)).trimmed();
                            while (value.startsWith(quoteString)) {
                                value = value.right(value.length() - 1);
                            }
                        }
                        if (name.compare(invertedString, Qt::CaseInsensitive) == 0) {
                            d->inverted = (value.compare(trueString, Qt::CaseInsensitive) == 0);
                        } else if (name.compare(typeString, Qt::CaseInsensitive) == 0) {
                            d->type = value.toString();
                        }
                    } else {
                        qCWarning(ACBF_LOG) << "While attempting to parse the sub-settings part of the selector section of of" << style << "we ended up with something other than a key/value pair (likely a unary value, which is not supported). The entry we attempted to parse was:" << ref;
                    }
                }
            } else if (splitElement.count() == 1) {
                // If there's only the one thing, it's just a name, so name our thing and keep going
                d->element = splitElement.value(0).trimmed().toString();
            } else {
                qCWarning(ACBF_LOG) << "While attempting to parse a selector in" << style << "we ended up with more than one set of selector details. We don't really know how to handle this, so you just get the whole thing as the element name now. The selector section we attempted to parse was:" << split.value(0);
                d->element = split.value(0).toString();
            }

            // Now let's figure out the parameters
            QVector<QStringRef>  splitParameters = split.value(1).split(semiColonString, Qt::SkipEmptyParts);
            // Some more parser helpers
            for (QStringRef parameter : splitParameters) {
                auto splitParameter = parameter.split(colonString, Qt::SkipEmptyParts);
                if (splitParameter.count() == 2) {
                    QStringRef name = splitParameter.value(0).trimmed();
                    QStringRef value = splitParameter.value(1).trimmed();
                    if (name.compare(colorString, Qt::CaseInsensitive) == 0) {
                        d->color = value.toString();
                    } else if (name.compare(fontFamilyString, Qt::CaseInsensitive) == 0) {
                        d->fontFamily = value.toString().split(commaString, Qt::SkipEmptyParts);
                    } else if (name.compare(fontStyleString, Qt::CaseInsensitive) == 0) {
                        d->fontStyle = value.toString();
                    } else if (name.compare(fontWeightString, Qt::CaseInsensitive) == 0) {
                        d->fontWeight = value.toString();
                    } else if (name.compare(fontStretchString, Qt::CaseInsensitive) == 0) {
                        d->fontStretch = value.toString();
                    } else {
                        qCWarning(ACBF_LOG) << "While parsing the parameter section of" << style << "we found an unsupported parameter :" << name << value;
                        // This isn't a fatal thing, we'll let it pass...
                    }
                } else {
                    qCWarning(ACBF_LOG) << "While parsing the parameter section of" << style << "we came across a unary value (which we don't support):" << parameter;
                    // This isn't a fatal thing, we'll let it pass...
                }
            }
        } else {
            qCWarning(ACBF_LOG) << "While attempting to parse the style, we somehow ended up with an empty selector section. The style we attempted to parse was:" << style;
            success = false;
        }
    } else {
        // We will occasionally end up with things that are just a bunch of spaces, so... let's not complain
        // about those, but also we've not successfully created a style, so still return false
        if (style.isEmpty() || style == newlineString) {
            qCDebug(ACBF_LOG) << "An empty style was encountered. This is fine, but since styles are supposed to be useful, we'll just quietly report we failed to parse that style.";
        } else {
            qCWarning(ACBF_LOG) << "We encountered something other than exactly one { in a style entry, which suggests a serious problem with the entire stylesheet. The style we attempted to parse was:" << style;
        }
        success = false;
    }
    return success;
}

QString Style::element() const
{
    return d->element;
}

void Style::setElement(const QString& element)
{
    if (d->element != element) {
        d->element = element;
        Q_EMIT elementChanged();
    }
}

bool Style::inverted() const
{
    return d->inverted;
}

void Style::setInverted(bool inverted)
{
    if (d->inverted != inverted) {
        d->inverted = inverted;
        Q_EMIT invertedChanged();
    }
}

QString Style::type() const
{
    return d->type;
}

void Style::setType(const QString& type)
{
    if (d->type != type) {
        d->type = type;
        Q_EMIT typeChanged();
    }
}

QString Style::color() const
{
    return d->color;
}

void Style::setColor(const QString& color)
{
    if (d->color != color) {
        d->color = color;
        Q_EMIT colorChanged();
    }
}

QStringList Style::fontFamily() const
{
    return d->fontFamily;
}

void Style::setFontFamily(const QStringList& fontFamily)
{
    if (d->fontFamily != fontFamily) {
        d->fontFamily = fontFamily;
        Q_EMIT fontFamilyChanged();
    }
}

QString Style::fontStyle() const
{
    return d->fontStyle;
}

void Style::setFontStyle(const QString& fontStyle)
{
    if (d->fontStyle != fontStyle) {
        d->fontStyle = fontStyle;
        Q_EMIT fontStyleChanged();
    }
}

QString Style::fontWeight() const
{
    return d->fontWeight;
}

void Style::setFontWeight(const QString& fontWeight)
{
    if (d->fontWeight != fontWeight) {
        d->fontWeight = fontWeight;
        Q_EMIT fontWeightChanged();
    }
}

QString Style::fontStretch() const
{
    return d->fontStretch;
}

void Style::setFontStretch(const QString& fontStretch)
{
    if (d->fontStretch != fontStretch) {
        d->fontStretch = fontStretch;
        Q_EMIT fontStretchChanged();
    }
}
