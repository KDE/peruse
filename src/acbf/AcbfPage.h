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
/**
 * \brief Class to handle page objects.
 * 
 * A proper ACBF document should have entries for all pages
 * and said entries should point at the images that make up
 * the comic.
 * 
 * However, ACBF also has room from frame definitions, transcriptions,
 * translations, table of contents, jumps and more.
 * 
 * The frame definitions are used to navigate a page and zoom efficiently on a
 * small screen. Similarly, transscriptions and translations can be used to show
 * text when the image itself is too blurry.
 * 
 * Title is used to generate a table of contents.
 * 
 * Transition is to indicate extra information about how the page should be entered.
 * 
 * bgcolor is used by the reading software to determine what background color to give.
 * 
 * Jumps can be used to move around in the comic.
 * 
 * TODO: Frame and Jump seem to be missing classes despite being used here?
 */
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
    explicit Page(Document* parent = nullptr);
    ~Page() override;

    /**
     * \brief Write the page into the xml writer.
     */
    void toXml(QXmlStreamWriter* writer);
    /**
     * \brief load a page element into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader);

    /**
     * @return the background color as a QString.
     * 
     * It should be an 8bit per channel rgb hexcode.
     */
    QString bgcolor() const;
    /**
     * \brief set the background color.
     * 
     * @param newColor - a String with an 8bit per channel rgb hexcode (#ff00ff, or the like)
     */
    void setBgcolor(const QString& newColor = QString());

    /**
     * @return transition type as a string.
     */
    QString transition() const;
    /**
     * \brief set the transition type.
     * @param transition - the transition type, possible entries are in the availableTransitions() stringlist.
     */
    void setTransition(const QString& transition);
    /**
     * @returns a list of strings that can be used for the transition.
     */
    static QStringList availableTransitions();

    /**
     * @return all titles for this page in all languages.
     */
    QStringList titleForAllLanguages() const;
    /**
     * @param language - the language of the entry in language code, country
     * code format joined by a dash (not an underscore).
     * @return the title for this language.
     */
    QString title(const QString& language = QString()) const;
    /**
     * \brief set the title for this language.
     * @param language - the language of the entry in language code, country
     * code format joined by a dash (not an underscore).
     */
    void setTitle(const QString& title, const QString& language = QString());

    /**
     * @returns the URI for the image of this page as a QString
     */
    QString imageHref() const;
    /**
     * \brief set the URI for the image of this page.
     * @param imageHref - the URI to an image.
     * 
     *  - A Binary representation will use the ID of that representation.
     *  - A reference to a file on the internet will start with "http://" or "https://"
     *  - A reference to a file in a zip will be prefixed with "zip:"
     *  - Everything else is presumed to be a file on disk.
     */
    void setImageHref(const QString& imageHref);

    /**
     * @returns all the textlayers objects.
     */
    QList<Textlayer*> textLayersForAllLanguages() const;
    /**
     * @param language - the language of the entry in language code, country
     * code format joined by a dash (not an underscore).
     * @returns the TextLayer object for that language.
     */
    Textlayer* textLayer(const QString& language = QString()) const;
    /**
     * 
     * @param language - the language of the entry in language code, country
     * code format joined by a dash (not an underscore). Setting the textlayer
     * for a language to null removes that language (as with other translated
     * entries, though this one not being text warranted a comment)
     */
    void setTextLayer(Textlayer* textlayer, const QString& language = "");

    /**
     * @returns a list of frames in this page.
     */
    QList<Frame*> frames() const;
    /**
     * @param index - index of the frame.
     * @return the frame of that index.
     */
    Frame* frame(int index) const;
    /**
     * @param frame - the frame you want to index of.
     * @returns the index of the given frame.
     */
    int frameIndex(Frame* frame) const;

    /**
     * \brief add a frame to the list of frames.
     * @param frame - the frame to add.
     * @param index - the index to add it at. If afterIndex is larger than
     * zero, the insertion will happen at that index
     */
    void addFrame(Frame* frame, int index = -1);
    /**
     * \brief remove the given frame from the framelist.
     * @param frame - the frame to remove.
     */
    void removeFrame(Frame* frame);
    /**
     * \brief Swap two frames in the list.
     * @param swapThis - the first frame to swap.
     * @param withThis - the second frame to swap.
     */
    bool swapFrames(Frame* swapThis, Frame* withThis);

    /**
     * @return the list of jump objects for this page.
     */
    QList<Jump*> jumps() const;
    /**
     * @param index - the index for which you want the jump object.
     * @return a jump object for the given frame.
     */
    Jump* jump(int index) const;
    /**
     * @param jump - the jump you want to index of.
     * @returns the index of the given jump.
     */
    int jumpIndex(Jump* jump) const;

    /**
     * \brief add a jump to the list of frames.
     * @param jump - the jump to add.
     * @param index - the index to add it at. If afterIndex is larger than
     * zero, the insertion will happen at that index
     */
    void addJump(Jump* jump, int index = -1);
    /**
     * \brief remove the given jump from the list of jumps.
     * @param jump - the jump to remove.
     */
    void removeJump(Jump* jump);
    /**
     * \brief Swap two jumps in the list.
     * @param swapThis - the first jumps to swap.
     * @param withThis - the second jumps to swap.
     */
    bool swapJumps(Jump* swapThis, Jump* withThis);

    /**
     * @returns whether this is the cover page.
     */
    bool isCoverPage() const;
    /**
     * \brief toggle whether this is the cover page.
     * 
     */
    void setIsCoverPage(bool isCoverPage = false);
private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif//ACBFPAGE_H
