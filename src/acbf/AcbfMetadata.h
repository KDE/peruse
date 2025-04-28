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

#ifndef ACBFMETADATA_H
#define ACBFMETADATA_H

#include "AcbfBookinfo.h"
#include "AcbfDocumentinfo.h"
#include "AcbfPublishinfo.h"
#include "acbf_export.h"

#include <QObject>

#include <memory>

/**
 * \brief Class to handle the metadata section of ACBF.
 *
 * ACBF metadata is split into three sections:
 *
 * - Book-info, which handles the metadata about the story.
 * - Publishing-info, which handles the metadata about the publishing.
 * - Document-info, which handles the metadata about this specific ACBF document.
 *
 * All three are mandatory.
 */
class QXmlStreamWriter;
class QXmlStreamReader;
namespace AdvancedComicBookFormat
{
class Document;

class ACBF_EXPORT Metadata : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AdvancedComicBookFormat::BookInfo *bookInfo READ bookInfo NOTIFY bookInfoChanged)
    Q_PROPERTY(AdvancedComicBookFormat::PublishInfo *publishInfo READ publishInfo NOTIFY publishInfoChanged)
    Q_PROPERTY(AdvancedComicBookFormat::DocumentInfo *documentInfo READ documentInfo NOTIFY documentInfoChanged)
public:
    explicit Metadata(Document *parent = nullptr);
    ~Metadata() override;

    Document *document() const;

    /**
     * \brief Write the metadata into the xml writer.
     */
    void toXml(QXmlStreamWriter *writer);
    /**
     * \brief load the metadata element into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader, const QString &xmlData);

    /**
     * @return the bookinfo object.
     */
    BookInfo *bookInfo() const;
    /**
     * \brief triggers when the bookinfo is changed.
     */
    Q_SIGNAL void bookInfoChanged();
    /**
     * @returns the publishinfo object.
     */
    PublishInfo *publishInfo() const;
    /**
     * @brief fires when the publishing info changes.
     */
    Q_SIGNAL void publishInfoChanged();
    /**
     * @returns the documentinfo object.
     */
    DocumentInfo *documentInfo() const;
    /**
     * @brief fires when the document info changes.
     */
    Q_SIGNAL void documentInfoChanged();

private:
    class Private;
    std::unique_ptr<Private> d;
};
}

Q_DECLARE_METATYPE(AdvancedComicBookFormat::Metadata *)

#endif // ACBFMETADATA_H
