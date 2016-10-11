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

#include "ArchiveBookModel.h"
#include "ArchiveImageProvider.h"

#include <AcbfAuthor.h>
#include <AcbfBody.h>
#include <AcbfBookinfo.h>
#include <AcbfDocument.h>
#include <AcbfMetadata.h>
#include <AcbfPage.h>
#include <AcbfPublishinfo.h>

#include <QDir>
#include <QQmlEngine>
#include <QMimeDatabase>
#include <QDebug>
#include <QTemporaryFile>

#include <KFileMetaData/UserMetaData>
#include <karchive.h>
#include <kzip.h>
#include "KRar.h" // "" because it's a custom thing for now

class ArchiveBookModel::Private
{
public:
    Private(ArchiveBookModel* qq)
        : q(qq)
        , engine(0)
        , archive(0)
        , readWrite(false)
        , imageProvider(0)
        , isDirty(false)
        , isLoading(false)
    {}
    ArchiveBookModel* q;
    QQmlEngine* engine;
    KArchive* archive;
    bool readWrite;
    ArchiveImageProvider* imageProvider;
    bool isDirty;
    bool isLoading;

    static int counter()
    {
        static int count = 0;
        return count++;
    }

    void setDirty()
    {
        isDirty = true;
        emit q->hasUnsavedChangedChanged();
    }

    void createNewAcbfDocumentFromLegacyInformation()
    {
        AdvancedComicBookFormat::Document* acbfDocument = new AdvancedComicBookFormat::Document(q);

        acbfDocument->metaData()->bookInfo()->setTitle(q->title());

        AdvancedComicBookFormat::Author* author = new AdvancedComicBookFormat::Author(acbfDocument->metaData());
        author->setNickName(q->author());
        acbfDocument->metaData()->bookInfo()->addAuthor(author);

        acbfDocument->metaData()->publishInfo()->setPublisher(q->publisher());

        int prefixLength = QString("image://%1/").arg(imageProvider->prefix()).length();
        if(q->pageCount() > 0)
        {
            // First, let's see if we have something called "*cover*"... because that would be handy and useful
            int cover = -1;
            for(int i = q->pageCount(); i > -1; --i)
            {
                QString url = q->data(q->index(i, 0, QModelIndex()), BookModel::UrlRole).toString().mid(prefixLength);
                // Yup, this is a bit sort of naughty and stuff... but, assume index 0 is the cover if nothing else has shown up...
                // FIXME this will also fail when there's more than one cover... say, back and front covers...
                if(url.split('/').last().contains("cover", Qt::CaseInsensitive) || i == 0) {
                    acbfDocument->metaData()->bookInfo()->coverpage()->setImageHref(url);
                    acbfDocument->metaData()->bookInfo()->coverpage()->setTitle(q->data(q->index(0, 0, QModelIndex()), BookModel::TitleRole).toString());
                    cover = i;
                    break;
                }
            }

            for(int i = 0; i < q->pageCount(); ++i)
            {
                if(i == cover)
                {
                    continue;
                }
                AdvancedComicBookFormat::Page* page = new AdvancedComicBookFormat::Page(acbfDocument);
                page->setImageHref(q->data(q->index(i, 0, QModelIndex()), BookModel::UrlRole).toString().mid(prefixLength));
                page->setTitle(q->data(q->index(i, 0, QModelIndex()), BookModel::TitleRole).toString());
                acbfDocument->body()->addPage(page);
            }
        }

        q->setAcbfData(acbfDocument);
        setDirty();
    }
};

ArchiveBookModel::ArchiveBookModel(QObject* parent)
    : BookModel(parent)
    , d(new Private(this))
{
}

ArchiveBookModel::~ArchiveBookModel()
{
    delete d;
}

QStringList recursiveEntries(const KArchiveDirectory* dir, const QString& dirName = "")
{
    QStringList entries = dir->entries();
    QStringList allEntries = entries;
    Q_FOREACH(const QString& entryName, entries)
    {
        const KArchiveEntry* entry = dir->entry(entryName);
        if(entry->isDirectory())
        {
            const KArchiveDirectory* subDir = static_cast<const KArchiveDirectory*>(entry);
            QStringList subEntries = recursiveEntries(subDir, dirName + entryName + "/");
            Q_FOREACH(const QString& subEntry, subEntries)
            {
                entries.append(dirName + entryName + "/" + subEntry);
            }
        }
    }
    return entries;
}

void ArchiveBookModel::setFilename(QString newFilename)
{
    d->isLoading = true;
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(newFilename);
    if(d->archive)
    {
        delete d->archive;
        // TODO clean up imageproviders...
    }
    d->archive = 0;
    if(mime.inherits("application/zip"))
    {
        d->archive = new KZip(newFilename);
    }
    else if (mime.inherits("application/x-rar"))
    {
        d->archive = new KRar(newFilename);
    }

    bool success = false;
    if(d->archive)
    {
        QString prefix = QString("archivebookpage%1").arg(QString::number(Private::counter()));
        if(d->archive->open(QIODevice::ReadOnly))
        {
            d->imageProvider = new ArchiveImageProvider();
            d->imageProvider->setArchiveBookModel(this);
            d->imageProvider->setPrefix(prefix);
            d->engine->addImageProvider(prefix, d->imageProvider);

            QStringList entries = recursiveEntries(d->archive->directory());

            // First check and see if we've got an ACBF document in there...
            QString acbfEntry;
            QLatin1String acbfSuffix(".acbf");
            Q_FOREACH(const QString& entry, entries)
            {
                if(entry.toLower().endsWith(acbfSuffix))
                {
                    acbfEntry = entry;
                    break;
                }
            }
            if(!acbfEntry.isEmpty())
            {
                AdvancedComicBookFormat::Document* acbfDocument = new AdvancedComicBookFormat::Document(this);
                const KArchiveFile* archFile = d->archive->directory()->file(acbfEntry);
                if(acbfDocument->fromXml(QString(archFile->data())))
                {
                    setAcbfData(acbfDocument);
                    addPage(QString("image://%1/%2").arg(prefix).arg(acbfDocument->metaData()->bookInfo()->coverpage()->imageHref()), acbfDocument->metaData()->bookInfo()->coverpage()->title());
                    Q_FOREACH(AdvancedComicBookFormat::Page* page, acbfDocument->body()->pages())
                    {
                        addPage(QString("image://%1/%2").arg(prefix).arg(page->imageHref()), page->title());
                    }
                }
                else
                {
                    // just in case this is, for whatever reason, being reused...
                    setAcbfData(0);
                }
            }
            if(!acbfData())
            {
                // fall back to just handling the files directly if there's no ACBF document...
                entries.sort();
                QString undesired = QString("%1").arg("/").append("Thumbs.db");
                Q_FOREACH(const QString& entry, entries)
                {
                    const KArchiveEntry* archEntry = d->archive->directory()->entry(entry);
                    if(archEntry->isFile() && !entry.endsWith(undesired))
                    {
                        addPage(QString("image://%1/%2").arg(prefix).arg(entry), entry.split("/").last());
                    }
                }
            }

            success = true;
        }
        else {
            qDebug() << "Failed to open archive";
        }
    }

//     QDir dir(newFilename);
//     if(dir.exists())
//     {
//         QFileInfoList entries = dir.entryInfoList(QDir::Files, QDir::Name);
//         Q_FOREACH(const QFileInfo& entry, entries)
//         {
//             addPage(QString("file://").append(entry.canonicalFilePath()), entry.fileName());
//         }
//     }
    BookModel::setFilename(newFilename);

    KFileMetaData::UserMetaData data(newFilename);
    if(data.hasAttribute("peruse.currentPage"))
        BookModel::setCurrentPage(data.attribute("peruse.currentPage").toInt(), false);

    d->isLoading = false;
    emit loadingCompleted(success);
}

QObject * ArchiveBookModel::qmlEngine() const
{
    return d->engine;
}

void ArchiveBookModel::setQmlEngine(QObject* newEngine)
{
    d->engine = qobject_cast<QQmlEngine*>(newEngine);
    emit qmlEngineChanged();
}

bool ArchiveBookModel::readWrite() const
{
    return d->readWrite;
}

void ArchiveBookModel::setReadWrite(bool newReadWrite)
{
    d->readWrite = newReadWrite;
    emit readWriteChanged();
}

bool ArchiveBookModel::hasUnsavedChanged() const
{
    return d->isDirty;
}

bool ArchiveBookModel::saveBook()
{
    bool success = true;
//     if(d->isDirty)
    {
        // TODO actually save! D:

        // get new filenames out of acbf
        // write into archive in acbf-order

        QTemporaryFile tmpFile(this);
        tmpFile.open();
        QString archiveFileName = tmpFile.fileName().append(".cbz");
        QFileInfo fileInfo(tmpFile);
        tmpFile.close();
        qDebug() << "Creating archive in" << archiveFileName;
        KArchive* archive = new KZip(archiveFileName);
        archive->open(QIODevice::ReadWrite);

        // We're a zip file... size isn't used
        qDebug() << "Writing in ACBF data";
        archive->prepareWriting("metadata.acbf", fileInfo.owner(), fileInfo.group(), 0);
        AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData());
        if(!acbfDocument)
        {
            d->createNewAcbfDocumentFromLegacyInformation();
            acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData());
        }
        QString acbfString = acbfDocument->toXml();
        archive->writeData(acbfString.toUtf8(), acbfString.size());
        archive->finishWriting(acbfString.size());

        qDebug() << "Copying across cover page";
        archive->prepareWriting(acbfDocument->metaData()->bookInfo()->coverpage()->imageHref(), fileInfo.owner(), fileInfo.group(), 0);
        const KArchiveFile* archFile = archiveFile(acbfDocument->metaData()->bookInfo()->coverpage()->imageHref());
        archive->writeData(archFile->data(), archFile->size());
        archive->finishWriting(archFile->size());

        Q_FOREACH(AdvancedComicBookFormat::Page* page, acbfDocument->body()->pages())
        {
            qDebug() << "Copying over" << page->title();
            archive->prepareWriting(page->imageHref(), fileInfo.owner(), fileInfo.group(), 0);
            archFile = archiveFile(page->imageHref());
            archive->writeData(archFile->data(), archFile->size());
            archive->finishWriting(archFile->size());
        }

        archive->close();
        qDebug() << "Archive created and closed...";
    }
    return success;
}

void ArchiveBookModel::addPage(QString url, QString title)
{
    // don't do this unless we're done loading... don't want to dirty things up until then!
    if(!d->isLoading)
    {
        AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData());
        if(!acbfDocument)
        {
            d->createNewAcbfDocumentFromLegacyInformation();
            acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData());
        }
        AdvancedComicBookFormat::Page* page = new AdvancedComicBookFormat::Page(acbfDocument);
        page->setTitle(title);
        QUrl imageUrl(url);
        page->setImageHref(QString("%1/%2").arg(imageUrl.path().mid(1)).arg(imageUrl.fileName()));
        acbfDocument->body()->addPage(page);
    }
    BookModel::addPage(url, title);
}

// FIXME any metadata change sets dirty (as we need to replace the whole file in archive when saving)

void ArchiveBookModel::addPageFromFile(QString fileUrl, int insertAfter)
{
    if(d->archive && d->readWrite && !d->isDirty)
    {
        int insertionIndex = insertAfter;
        if(insertAfter < 0 || pageCount() - 1 < insertAfter) {
            insertionIndex = pageCount();
        }

        // This is a permanent thing, renaming in zip files is VERY expensive (literally not possible without
        // rewriting the entire archive...)
        QString archiveFileName = QString("page-%1.jpg").arg(QString::number(insertionIndex));
        d->archive->close();
        d->archive->open(QIODevice::ReadWrite);
        d->archive->addLocalFile(fileUrl, archiveFileName);
        d->archive->close();
        d->archive->open(QIODevice::ReadOnly);
        addPage(QString("image://%1/%2").arg(d->imageProvider->prefix()).arg(archiveFileName), archiveFileName.split("/").last());
        saveBook();
    }
}

void ArchiveBookModel::swapPages(int swapThisIndex, int withThisIndex)
{
    d->setDirty();

    AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData());

    // Cover pages are special, and in acbf they are very special... (otherwise they're page zero)
    if(swapThisIndex == 0)
    {
        AdvancedComicBookFormat::Page* oldCoverPage = acbfDocument->metaData()->bookInfo()->coverpage();
        AdvancedComicBookFormat::Page* otherPage = acbfDocument->body()->page(withThisIndex - 1);
        acbfDocument->body()->removePage(otherPage);
        acbfDocument->metaData()->bookInfo()->setCoverpage(otherPage);
        acbfDocument->body()->addPage(oldCoverPage, withThisIndex - 1);
    }
    else if(withThisIndex == 0)
    {
        AdvancedComicBookFormat::Page* oldCoverPage = acbfDocument->metaData()->bookInfo()->coverpage();
        AdvancedComicBookFormat::Page* otherPage = acbfDocument->body()->page(swapThisIndex - 1);
        acbfDocument->body()->removePage(otherPage);
        acbfDocument->metaData()->bookInfo()->setCoverpage(otherPage);
        acbfDocument->body()->addPage(oldCoverPage, swapThisIndex - 1);
    }
    else
    {
        AdvancedComicBookFormat::Page* firstPage = acbfDocument->body()->page(swapThisIndex - 1);
        AdvancedComicBookFormat::Page* otherPage = acbfDocument->body()->page(withThisIndex - 1);
        acbfDocument->body()->swapPages(firstPage, otherPage);
    }

    // FIXME only treat things which have sequential numbers in as pages, split out chapters automatically?

    BookModel::swapPages(swapThisIndex, withThisIndex);
}

const KArchiveFile * ArchiveBookModel::archiveFile(const QString& filePath)
{
    if(d->archive)
    {
        return d->archive->directory()->file(filePath);
    }
    return 0;
}
