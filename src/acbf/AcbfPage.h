/*
 * Copyright (C) 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#ifndef ACBFPAGE_H
#define ACBFPAGE_H

#include "AcbfDocument.h"

class QXmlStreamWriter;
class QXmlStreamReader;
namespace AdvancedComicBookFormat
{
class Textlayer;
class Frame;
class Jump;
class ACBF_EXPORT Page : public QObject
{
    Q_OBJECT
public:
    // Pages can also be cover pages, which means they can also be children of BookInfo
    explicit Page(Document* parent = 0);
    ~Page() override;

    void toXml(QXmlStreamWriter* writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    QString bgcolor();
    void setBgcolor(QString newColor = "");

    QString transition();
    void setTransition(QString transition);
    static QStringList availableTransitions();

    QStringList titleForAllLanguages();
    QString title(QString language = "");
    void setTitle(QString title, QString language = "");

    QString imageHref();
    void setImageHref(QString imageHref);

    QList<Textlayer*> textLayersForAllLanguages();
    Textlayer* textLayer(QString language = "");
    // Setting the textlayer for a language to null removes that language (as with other translated entries, though this one not being text warranted a comment)
    void setTextLayer(Textlayer* textlayer, QString language = "");

    QList<Frame*> frames();
    Frame* frame(int index);
    int frameIndex(Frame* frame);
    // If afterIndex is larger than zero, the insertion will happen at that index
    void addFrame(Frame* frame, int index = -1);
    void removeFrame(Frame* frame);
    bool swapFrames(Frame* swapThis, Frame* withThis);

    QList<Jump*> jumps();
    Jump* jump(int index);
    int jumpIndex(Jump* jump);
    // If afterIndex is larger than zero, the insertion will happen at that index
    void addJump(Jump* jump, int index = -1);
    void removeJump(Jump* jump);
    bool swapJumps(Jump* swapThis, Jump* withThis);

    bool isCoverPage();
    void setIsCoverPage(bool isCoverPage = false);
private:
    class Private;
    Private* d;
};
}

#endif//ACBFPAGE_H
