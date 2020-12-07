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

#ifndef ACBFSTYLE_H
#define ACBFSTYLE_H

#include <QObject>
#include <memory>

#include "acbf_export.h"

namespace AdvancedComicBookFormat {
/**
 * \brief A single entry in a stylesheet
 * @see AdvancedComicBookFormat::StyleSheet
 *
 * The application procedure for styles are as follows:
 * 0) Apply the * style
 * 1) Apply the style for the element, if one exists (e.g. \<text-area\>)
 * 2) If there is a type specified, apply the style for the specified type, if one exists (e.g. \<text-area type=\"commentary\">)
 * 3) If the element is marked as inverted, apply that element's inverted style, if one exists (e.g. \<text-area inverted=true\>)
 * 4) If the element is both a specified type and marked as inverted, apply the style for that type's inverted style, if one exists (e.g. \<text-area type=\"thought\" inverted=true\>)
 * Note that style application will go through the entire stack. It will not stop just because one of the above items is not available for application.
 */
class ACBF_EXPORT Style : public QObject {
    Q_OBJECT
    /**
     * The specific element this style will change the look of. Can be one of the following:
     * - * (catch-all entry)
     * - text-area
     * - emphasis
     * - strong
     */
    Q_PROPERTY(QString element READ element WRITE setElement NOTIFY elementChanged)
    /**
     * Whether or not this style should be used to style elements which request an inverted scheme
     */
    Q_PROPERTY(bool inverted READ inverted WRITE setInverted NOTIFY invertedChanged)
    /**
     * The sub-element selector type this style will change the look of. Can be one of the following:
     * - code
     * - letter
     * - commentary
     * - formal
     * - heading
     * - audio
     * - thought
     * - sign
     */
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    /**
     * The color of the element's text
     */
    Q_PROPERTY(QString color READ color WRITE setColor NOTIFY colorChanged)
    /**
     * A fail-through list for selecting which font family to use for this element. You should always end
     * this list with a generic one, which would be one of the following:
     * - serif
     * - sans-serif
     * - cursive
     * - fantasy
     * - monospace
     */
    Q_PROPERTY(QStringList fontFamily READ fontFamily WRITE setFontFamily NOTIFY fontFamilyChanged)
    /**
     * The style used for this element's text. One of the following:
     * - normal
     * - italic
     * - oblique
     */
    Q_PROPERTY(QString fontStyle READ fontStyle WRITE setFontStyle NOTIFY fontStyleChanged)
    /**
     * The weight of the font used for this element's text. Use one of the following:
     * - A number from 100 (the lightest) through 900 (the boldest)
     * - normal (the same as 400)
     * - bold (the same as 700)
     * - bolder (the same as 900)
     * - lighter (the same as 100)
     */
    Q_PROPERTY(QString fontWeight READ fontWeight WRITE setFontWeight NOTIFY fontWeightChanged)
    /**
     * The amount the text in this element will be stretched or squeezed. Use one of the following:
     * - A number between 50 and 200 (the percentage of stretching to do)
     * - ultra-condensed
     * - extra-condensed
     * - condensed
     * - semi-condensed
     * - normal
     * - semi-expanded
     * - expanded
     * - extra-expanded
     * - ultra-expanded
     */
    Q_PROPERTY(QString fontStretch READ fontStretch WRITE setFontStretch NOTIFY fontStretchChanged)
    /**
     * The string representation of this style.
     * Note, to set the data from a string, use the function fromString directly. It is not exposed as a property,
     * as the intention is that if properties are used, the individual settings should be used instead.
     */
    Q_PROPERTY(QString string READ toString NOTIFY styleDataChanged)
public:
    explicit Style(QObject* parent = nullptr);
    virtual ~Style();

    /**
     * \brief Build a style string from this object
     */
    QString toString() const;
    /**
     * \brief load a stylesheet entry into this object.
     * @return True if the parser encountered no errors.
     */
    bool fromString(const QString &style);
    /**
     * \brief load a stylesheet entry into this object
     * @return True if the parser encountered no errors.
     */
    bool fromString(QStringRef style);

    QString element() const;
    void setElement(const QString& element);
    Q_SIGNAL void elementChanged();

    bool inverted() const;
    void setInverted(bool inverted);
    Q_SIGNAL void invertedChanged();

    QString type() const;
    void setType(const QString& type);
    Q_SIGNAL void typeChanged();

    QString color() const;
    void setColor(const QString& color);
    Q_SIGNAL void colorChanged();

    QStringList fontFamily() const;
    void setFontFamily(const QStringList& fontFamily);
    Q_SIGNAL void fontFamilyChanged();

    QString fontStyle() const;
    void setFontStyle(const QString& fontStyle);
    Q_SIGNAL void fontStyleChanged();

    QString fontWeight() const;
    void setFontWeight(const QString& fontWeight);
    Q_SIGNAL void fontWeightChanged();

    QString fontStretch() const;
    void setFontStretch(const QString& fontStretch);
    Q_SIGNAL void fontStretchChanged();

    Q_SIGNAL void styleDataChanged();
private:
    class Private;
    std::unique_ptr<Private> d;
};
};
Q_DECLARE_METATYPE(AdvancedComicBookFormat::Style*);

#endif//ACBFSTYLE_H
