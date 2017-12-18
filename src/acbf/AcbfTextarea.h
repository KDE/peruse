/*
 * Copyright (C) 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#ifndef ACBFTEXTAREA_H
#define ACBFTEXTAREA_H

#include "AcbfTextlayer.h"

#include <QPoint>

namespace AdvancedComicBookFormat
{
class ACBF_EXPORT Textarea : public QObject
{
    Q_OBJECT
public:
    explicit Textarea(Textlayer* parent = 0);
    ~Textarea() override;

    void toXml(QXmlStreamWriter* writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    QList<QPoint> points();
    QPoint point(int index);
    int pointIndex(QPoint point);
    // If afterIndex is larger than zero, the insertion will happen at that index
    void addPoint(QPoint point, int index = -1);
    void removePoint(QPoint point);
    bool swapPoints(QPoint swapThis, QPoint withThis);

    QString bgcolor();
    void setBgcolor(QString newColor = "");

    int textRotation();
    void setTextRotation(int rotation = 0);

    QString type();
    void setType(QString type = "speech");
    static QStringList availableTypes();

    bool inverted();
    void setInverted(bool inverted = false);

    bool transparent();
    void setTransparent(bool transparent = false);

    QStringList paragraphs();
    // Allowed sub-elements: strong, emphasis, strikethrough, sub, sup, a (with mandatory href attribute only)
    // Deprecated sub-elements (superceded by...): code (type option code), inverted (textarea option inverted)
    void setParagraphs(QStringList paragraphs);
private:
    class Private;
    Private* d;
};
}

#endif//ACBFTEXTAREA_H
