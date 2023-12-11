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

#ifndef ACBFSTYLESHEET_H
#define ACBFSTYLESHEET_H

#include "acbf_export.h"

#include "AcbfStyle.h"

#include <QObject>

#include <memory>

class QXmlStreamWriter;
class QXmlStreamReader;
namespace AdvancedComicBookFormat
{
class Document;
/**
 * @brief Class to handle the CSS stylesheet.
 * 
 * ACBF files have a CSS stylesheet that can be used
 * to style the different types of textarea, such as
 * speech, inverted, commentary, and so forth.
 * 
 * This in turn allows generated text overlays to be
 * closer stylistically to a given acbf file.
 * 
 * This class does some minimal parsing on the css to
 * separate the definitions and the selectors.
 */
class ACBF_EXPORT StyleSheet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObjectList styles READ styles NOTIFY stylesChanged)
public:
    explicit StyleSheet(Document* parent = nullptr);
    ~StyleSheet() override;

    /**
     * \brief Write the stylesheet into the xml writer.
     */
    void toXml(QXmlStreamWriter* writer);
    /**
     * \brief load a stylesheet element into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader, const QString& xmlData);

    /**
     * The styles contained within this stylesheet
     * @see AdvancedComicBookFormat::Style
     */
    QObjectList styles() const;
    Q_SIGNAL void stylesChanged();
    Q_INVOKABLE AdvancedComicBookFormat::Style* addStyle();

    /**
     * Fetch a specific style based on the identifying markers
     */
    Q_INVOKABLE QObject* style(const QString& element, const QString& type, bool inverted);

    /**
     * @brief set the contents of the style section
     * 
     * @param css a QString containing a css stylesheet.
     */
    void setContents(const QString& css = QString());

private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif // ACBFSTYLESHEET_H
