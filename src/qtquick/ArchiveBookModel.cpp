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
#include <AcbfDocumentinfo.h>

#include <QCoreApplication>
#include <QDir>
#include <QImageReader>
#include <QMimeDatabase>
#include <QQmlEngine>
#include <QTemporaryFile>
#include <QXmlStreamReader>

#include <KFileMetaData/UserMetaData>
#include <karchive.h>
#include <kzip.h>
#include "KRar.h" // "" because it's a custom thing for now

#include <qtquick_debug.h>

class ArchiveBookModel::Private
{
public:
    Private(ArchiveBookModel* qq)
        : q(qq)
        , engine(nullptr)
        , archive(nullptr)
        , readWrite(false)
        , imageProvider(nullptr)
        , isDirty(false)
        , isLoading(false)
    {}
    ~Private() {
        qDeleteAll(archiveFiles.values());
    }
    ArchiveBookModel* q;
    QQmlEngine* engine;
    KArchive* archive;
    QStringList fileEntries;
    QHash<QString, const KArchiveFile*> archiveFiles;
    bool readWrite;
    ArchiveImageProvider* imageProvider;
    bool isDirty;
    bool isLoading;
    QMimeDatabase mimeDatabase;

    static int counter()
    {
        static int count = 0;
        return count++;
    }

    void setDirty()
    {
        isDirty = true;
        emit q->hasUnsavedChangesChanged();
    }

    AdvancedComicBookFormat::Document* createNewAcbfDocumentFromLegacyInformation()
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
        return acbfDocument;
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

QStringList recursiveEntries(const KArchiveDirectory* dir)
{
    QStringList entries = dir->entries();
    QStringList allEntries = entries;
    Q_FOREACH(const QString& entryName, entries)
    {
        const KArchiveEntry* entry = dir->entry(entryName);
        if(entry->isDirectory())
        {
            const KArchiveDirectory* subDir = static_cast<const KArchiveDirectory*>(entry);
            QStringList subEntries = recursiveEntries(subDir);
            Q_FOREACH(const QString& subEntry, subEntries)
            {
                entries.append(entryName + "/" + subEntry);
            }
        }
    }
    return entries;
}

void ArchiveBookModel::setFilename(QString newFilename)
{
    setProcessing(true);
    d->isLoading = true;

    if(d->archive)
    {
        clearPages();
        delete d->archive;
    }
    d->archive = nullptr;
    if(d->imageProvider && d->engine) {
        d->engine->removeImageProvider(d->imageProvider->prefix());
    }
    d->imageProvider = nullptr;
    d->fileEntries.clear();
    Q_EMIT fileEntriesChanged();

    QMimeType mime = d->mimeDatabase.mimeTypeForFile(newFilename);
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
            QMutexLocker locker(&archiveMutex);
            d->imageProvider = new ArchiveImageProvider();
            d->imageProvider->setArchiveBookModel(this);
            d->imageProvider->setPrefix(prefix);
            if(d->engine) {
                d->engine->addImageProvider(prefix, d->imageProvider);
            }

            d->fileEntries = recursiveEntries(d->archive->directory());
            d->fileEntries.sort();
            Q_EMIT fileEntriesChanged();

            // First check and see if we've got an ACBF document in there...
            QString acbfEntry;
            QString comicInfoEntry;
            QStringList xmlFiles;
            QLatin1String acbfSuffix(".acbf");
            QLatin1String ComicInfoXML("comicinfo.xml");
            QLatin1String xmlSuffix(".xml");
            QStringList images;
            Q_FOREACH(const QString& entry, d->fileEntries)
            {
                if(entry.toLower().endsWith(acbfSuffix))
                {
                    acbfEntry = entry;
                    break;
                }
                if(entry.toLower().endsWith(xmlSuffix)) {
                    if(entry.toLower().endsWith(ComicInfoXML)) {
                        comicInfoEntry = entry;
                    } else {
                        xmlFiles.append(entry);
                    }
                }
                if (entry.toLower().endsWith(".jpg") || entry.toLower().endsWith(".jpeg")
                        || entry.toLower().endsWith(".gif") || entry.toLower().endsWith(".png")) {
                    images.append(entry);
                }
            }
            images.sort();
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
                    setAcbfData(nullptr);
                }
            }
            else if (!comicInfoEntry.isEmpty() || !xmlFiles.isEmpty()) {
                AdvancedComicBookFormat::Document* acbfDocument = new AdvancedComicBookFormat::Document(this);
                const KArchiveFile* archFile = d->archive->directory()->file(comicInfoEntry);
                bool loadData = false;

                if (!comicInfoEntry.isEmpty()) {
                    loadData = loadComicInfoXML(archFile->data(), acbfDocument, images, newFilename);
                } else {
                    loadData = loadCoMet(xmlFiles, acbfDocument, images, newFilename);
                }

                if (loadData) {
                    setAcbfData(acbfDocument);
                    QString undesired = QString("%1").arg("/").append("Thumbs.db");
                    addPage(QString("image://%1/%2").arg(prefix).arg(acbfDocument->metaData()->bookInfo()->coverpage()->imageHref()), acbfDocument->metaData()->bookInfo()->coverpage()->title());
                    Q_FOREACH(AdvancedComicBookFormat::Page* page, acbfDocument->body()->pages())
                    {
                        addPage(QString("image://%1/%2").arg(prefix).arg(page->imageHref()), page->title());
                    }
                }
            }
            if(!acbfData())
            {
                // fall back to just handling the files directly if there's no ACBF document...
                QString undesired = QString("%1").arg("/").append("Thumbs.db");
                Q_FOREACH(const QString& entry, d->fileEntries)
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
            qCDebug(QTQUICK_LOG) << "Failed to open archive";
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

    if(!acbfData() && d->readWrite && d->imageProvider)
    {
        d->createNewAcbfDocumentFromLegacyInformation();
    }

    d->isLoading = false;
    emit loadingCompleted(success);
    setProcessing(false);
}

QString ArchiveBookModel::author() const
{
    AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData());
    if(acbfDocument)
    {
        if(acbfDocument->metaData()->bookInfo()->author().count() > 0)
        {
            return acbfDocument->metaData()->bookInfo()->author().at(0)->displayName();
        }
    }
    return BookModel::author();
}

void ArchiveBookModel::setAuthor(QString newAuthor)
{
    if(!d->isLoading)
    {
        AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData());
        if(!acbfDocument)
        {
            acbfDocument = d->createNewAcbfDocumentFromLegacyInformation();
        }
        if(acbfDocument->metaData()->bookInfo()->author().count() == 0)
        {
            AdvancedComicBookFormat::Author* author = new AdvancedComicBookFormat::Author(acbfDocument->metaData());
            author->setNickName(newAuthor);
            acbfDocument->metaData()->bookInfo()->addAuthor(author);
        }
        else
        {
            acbfDocument->metaData()->bookInfo()->author().at(0)->setNickName(newAuthor);
        }
    }
    BookModel::setAuthor(newAuthor);
}

QString ArchiveBookModel::publisher() const
{
    AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData());
    if(acbfDocument)
    {
        if(!acbfDocument->metaData()->publishInfo()->publisher().isEmpty())
        {
            return acbfDocument->metaData()->publishInfo()->publisher();
        }
    }
    return BookModel::publisher();
}

void ArchiveBookModel::setPublisher(QString newPublisher)
{
    if(!d->isLoading)
    {
        AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData());
        if(!acbfDocument)
        {
            acbfDocument = d->createNewAcbfDocumentFromLegacyInformation();
        }
        acbfDocument->metaData()->publishInfo()->setPublisher(newPublisher);
    }
    BookModel::setAuthor(newPublisher);
}

QString ArchiveBookModel::title() const
{
    AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData());
    if(acbfDocument)
    {
        if(acbfDocument->metaData()->bookInfo()->title().length() > 0)
        {
            return acbfDocument->metaData()->bookInfo()->title();
        }
    }
    return BookModel::title();
}

void ArchiveBookModel::setTitle(QString newTitle)
{
    if(!d->isLoading)
    {
        AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData());
        if(!acbfDocument)
        {
            acbfDocument = d->createNewAcbfDocumentFromLegacyInformation();
        }
        acbfDocument->metaData()->bookInfo()->setTitle(newTitle);
    }
    BookModel::setTitle(newTitle);
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

bool ArchiveBookModel::hasUnsavedChanges() const
{
    return d->isDirty;
}

void ArchiveBookModel::setDirty(bool isDirty)
{
    d->isDirty = isDirty;
    emit hasUnsavedChangesChanged();
}

QStringList ArchiveBookModel::fileEntries() const
{
    return d->fileEntries;
}

bool ArchiveBookModel::saveBook()
{
    bool success = true;
    if(d->isDirty)
    {
        QMutexLocker locker(&archiveMutex);
        // TODO get new filenames out of acbf

        setProcessing(true);
        qApp->processEvents();

        QTemporaryFile tmpFile(this);
        tmpFile.open();
        QString archiveFileName = tmpFile.fileName().append(".cbz");
        QFileInfo fileInfo(tmpFile);
        tmpFile.close();
        qCDebug(QTQUICK_LOG) << "Creating archive in" << archiveFileName;
        KZip* archive = new KZip(archiveFileName);
        archive->open(QIODevice::ReadWrite);

        // We're a zip file... size isn't used
        qCDebug(QTQUICK_LOG) << "Writing in ACBF data";
        archive->prepareWriting("metadata.acbf", fileInfo.owner(), fileInfo.group(), 0);
        AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData());
        if(!acbfDocument)
        {
            acbfDocument = d->createNewAcbfDocumentFromLegacyInformation();
        }
        QString acbfString = acbfDocument->toXml();
        archive->writeData(acbfString.toUtf8(), acbfString.size());
        archive->finishWriting(acbfString.size());

        qCDebug(QTQUICK_LOG) << "Copying across cover page";
        const KArchiveFile* archFile = archiveFile(acbfDocument->metaData()->bookInfo()->coverpage()->imageHref());
        if(archFile)
        {
            archive->prepareWriting(acbfDocument->metaData()->bookInfo()->coverpage()->imageHref(), fileInfo.owner(), fileInfo.group(), 0);
            archive->writeData(archFile->data(), archFile->size());
            archive->finishWriting(archFile->size());
        }

        Q_FOREACH(AdvancedComicBookFormat::Page* page, acbfDocument->body()->pages())
        {
            qApp->processEvents();
            qCDebug(QTQUICK_LOG) << "Copying over" << page->title();
            archFile = archiveFile(page->imageHref());
            if(archFile)
            {
                archive->prepareWriting(page->imageHref(), archFile->user(), archFile->group(), 0);
                archive->writeData(archFile->data(), archFile->size());
                archive->finishWriting(archFile->size());
            }
        }

        archive->close();
        qCDebug(QTQUICK_LOG) << "Archive created and closed...";

        // swap out the two files, tell model we're about to swap things out...
        beginResetModel();

        QString actualFile = d->archive->fileName();
        d->archive->close();

        // This seems roundabout... but it retains ctime and xattrs, which would be gone
        // if we just did a delete+rename
        QFile destinationFile(actualFile);
        if(destinationFile.open(QIODevice::WriteOnly))
        {
            QFile originFile(archiveFileName);
            if(originFile.open(QIODevice::ReadOnly)) {
                qCDebug(QTQUICK_LOG) << "Copying all content from" << archiveFileName << "to" << actualFile;
                while(!originFile.atEnd())
                {
                    destinationFile.write(originFile.read(65536));
                    qApp->processEvents();
                }
                destinationFile.close();
                originFile.close();
                if(originFile.remove())
                {
                    qCDebug(QTQUICK_LOG) << "Success! Now loading the new archive...";
                    // now load the new thing...
                    setFilename(actualFile);
                }
                else
                {
                    qCWarning(QTQUICK_LOG) << "Failed to delete" << originFile.fileName();
                }
            }
            else
            {
                qCWarning(QTQUICK_LOG) << "Failed to open" << originFile.fileName() << "for reading";
            }
        }
        else
        {
            qCWarning(QTQUICK_LOG) << "Failed to open" << destinationFile.fileName() << "for writing";
        }
    }
    endResetModel();
    setProcessing(false);
    setDirty(false);
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
            acbfDocument = d->createNewAcbfDocumentFromLegacyInformation();
        }
        QUrl imageUrl(url);
        if(pageCount() == 0)
        {
            acbfDocument->metaData()->bookInfo()->coverpage()->setTitle(title);
            acbfDocument->metaData()->bookInfo()->coverpage()->setImageHref(QString("%1/%2").arg(imageUrl.path().mid(1)).arg(imageUrl.fileName()));
        }
        else
        {
            AdvancedComicBookFormat::Page* page = new AdvancedComicBookFormat::Page(acbfDocument);
            page->setTitle(title);
            page->setImageHref(QString("%1/%2").arg(imageUrl.path().mid(1)).arg(imageUrl.fileName()));
            acbfDocument->body()->addPage(page);
        }
    }
    BookModel::addPage(url, title);
}

void ArchiveBookModel::removePage(int pageNumber)
{
    if(!d->isLoading)
    {
        AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData());
        if(!acbfDocument)
        {
            acbfDocument = d->createNewAcbfDocumentFromLegacyInformation();
        }
        else
        {
            if(pageNumber == 0)
            {
                //Page no 0 is the cover page, when removed we'll take the next page.
                AdvancedComicBookFormat::Page* page = acbfDocument->body()->page(0);
                acbfDocument->metaData()->bookInfo()->setCoverpage(page);
                acbfDocument->body()->removePage(page);
            }
            else {
                AdvancedComicBookFormat::Page* page = acbfDocument->body()->page(pageNumber-1);
                acbfDocument->body()->removePage(page);
            }
        }
    }
    BookModel::removePage(pageNumber);
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
        QString archiveFileName = QString("page-%1.%2").arg(QString::number(insertionIndex), QFileInfo(fileUrl).completeSuffix());
        d->archive->close();
        d->archive->open(QIODevice::ReadWrite);
        d->archive->addLocalFile(fileUrl, archiveFileName);
        d->archive->close();
        d->archive->open(QIODevice::ReadOnly);
        addPage(QString("image://%1/%2").arg(d->imageProvider->prefix()).arg(archiveFileName), archiveFileName.split("/").last());
        d->fileEntries << archiveFileName;
        d->fileEntries.sort();
        Q_EMIT fileEntriesChanged();
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

QString ArchiveBookModel::createBook(QString folder, QString title, QString coverUrl)
{
    bool success = true;

    QString fileTitle = title.replace( QRegExp("\\W"),QString("")).simplified();
    QString filename = QString("%1/%2.cbz").arg(folder).arg(fileTitle);
    int i = 1;
    while(QFile(filename).exists())
    {
        filename = QString("%1/%2 (%3).cbz").arg(folder).arg(fileTitle).arg(QString::number(i++));
    }

    ArchiveBookModel* model = new ArchiveBookModel(nullptr);
    model->setQmlEngine(qmlEngine());
    model->setReadWrite(true);
    QString prefix = QString("archivebookpage%1").arg(QString::number(Private::counter()));
    model->d->imageProvider = new ArchiveImageProvider();
    model->d->imageProvider->setArchiveBookModel(model);
    model->d->imageProvider->setPrefix(prefix);
    model->d->archive = new KZip(filename);
    model->BookModel::setFilename(filename);
    model->setTitle(title);
    AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(model->acbfData());
    QString coverArchiveName = QString("cover.%1").arg(QFileInfo(coverUrl).completeSuffix());
    acbfDocument->metaData()->bookInfo()->coverpage()->setImageHref(coverArchiveName);
    success = model->saveBook();

    model->d->archive->close();
    model->d->archive->open(QIODevice::ReadWrite);
    model->d->archive->addLocalFile(coverUrl, coverArchiveName);
    model->d->fileEntries << coverArchiveName;
    model->d->fileEntries.sort();
    Q_EMIT model->fileEntriesChanged();
    model->d->archive->close();

    model->deleteLater();

    if(!success)
        return QLatin1String("");
    return filename;
}

const KArchiveFile * ArchiveBookModel::archiveFile(const QString& filePath) const
{
    if(d->archive)
    {
        if(!d->archiveFiles.contains(filePath)) {
            d->archiveFiles[filePath] = d->archive->directory()->file(filePath);
        }
        return d->archiveFiles[filePath];
    }
    return nullptr;
}

bool ArchiveBookModel::loadComicInfoXML(QString xmlDocument, QObject *acbfData, QStringList entries, QString filename)
{
    KFileMetaData::UserMetaData filedata(filename);
    AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData);
    QXmlStreamReader xmlReader(xmlDocument);
    if(xmlReader.readNextStartElement())
    {
        if(xmlReader.name() == QStringLiteral("ComicInfo"))
        {
            // We'll need to collect several items to generate a series. Thankfully, comicinfo only has two types of series.
            QString series;
            int number = -1;
            int volume = 0;

            QString seriesAlt;
            int numberAlt = -1;
            int volumeAlt = 0;

            // Also publishing date.
            int year = 0;
            int month = 0;
            int day = 0;

            QStringList publisher;
            QStringList keywords;
            QStringList empty;

            while(xmlReader.readNextStartElement())
            {
                if(xmlReader.name() == QStringLiteral("Title"))
                {
                    acbfDocument->metaData()->bookInfo()->setTitle(xmlReader.readElementText(),"");
                }

                //Summary/annotation.
                else if(xmlReader.name() == QStringLiteral("Summary"))
                {
                    acbfDocument->metaData()->bookInfo()->setAnnotation(xmlReader.readElementText().split("\n\n"), "");
                }

                /*
                 * This ought to go into the kfile metadata.
                 */
                else if(xmlReader.name() == QStringLiteral("Notes"))
                {
                    if (filedata.userComment().isEmpty()) {
                        filedata.setUserComment(xmlReader.readElementText());
                    } else {
                        xmlReader.skipCurrentElement();
                    }
                }
                else if(xmlReader.name() == QStringLiteral("Tags"))
                {
                    QStringList tags = filedata.tags();
                    QStringList newTags = xmlReader.readElementText().split(",");
                    for (int i=0; i < newTags.size(); i++) {
                        if (!tags.contains(newTags.at(i))) {
                            tags.append(newTags.at(i));
                        }
                    }
                    filedata.setTags(tags);
                }
                else if(xmlReader.name() == QStringLiteral("PageCount"))
                {
                    filedata.setAttribute("Peruse.totalPages", xmlReader.readElementText());
                }
                else if(xmlReader.name() == QStringLiteral("ScanInformation"))
                {
                    QString userComment = filedata.userComment();
                    userComment.append("\n"+xmlReader.readElementText());
                    filedata.setUserComment(userComment);
                }

                //Series

                else if(xmlReader.name() == QStringLiteral("Series"))
                {
                    series = xmlReader.readElementText();
                }
                else if(xmlReader.name() == QStringLiteral("Number"))
                {
                    number = xmlReader.readElementText().toInt();
                }
                else if(xmlReader.name() == QStringLiteral("Volume"))
                {
                    volume = xmlReader.readElementText().toInt();
                }

                // Series alt

                else if(xmlReader.name() == QStringLiteral("AlternateSeries"))
                {
                    seriesAlt = xmlReader.readElementText();
                }
                else if(xmlReader.name() == QStringLiteral("AlternateNumber"))
                {
                    numberAlt = xmlReader.readElementText().toInt();
                }
                else if(xmlReader.name() == QStringLiteral("AlternateVolume"))
                {
                    volumeAlt = xmlReader.readElementText().toInt();
                }

                // Publishing date.

                else if(xmlReader.name() == QStringLiteral("Year"))
                {
                    year = xmlReader.readElementText().toInt();
                }
                else if(xmlReader.name() == QStringLiteral("Month"))
                {
                    month = xmlReader.readElementText().toInt();
                }
                else if(xmlReader.name() == QStringLiteral("Day"))
                {
                    day = xmlReader.readElementText().toInt();
                }

                //Publisher

                else if(xmlReader.name() == QStringLiteral("Publisher"))
                {
                    publisher.append(xmlReader.readElementText());
                }
                else if(xmlReader.name() == QStringLiteral("Imprint"))
                {
                    publisher.append(xmlReader.readElementText());
                }

                //Genre
                else if(xmlReader.name() == QStringLiteral("Genre"))
                {
                    QString key = xmlReader.readElementText();
                    QString genreKey = key.toLower().replace(" ", "_");
                    if (acbfDocument->metaData()->bookInfo()->availableGenres().contains(genreKey)) {
                        acbfDocument->metaData()->bookInfo()->setGenre(genreKey);
                    } else {
                        //There must always be a genre in a proper acbf file...
                        acbfDocument->metaData()->bookInfo()->setGenre("other");
                        keywords.append(key);
                    }
                }

                //Language

                else if(xmlReader.name() == QStringLiteral("LanguageISO"))
                {
                    acbfDocument->metaData()->bookInfo()->addLanguage(xmlReader.readElementText());
                }

                //Sources/Weblink
                else if(xmlReader.name() == QStringLiteral("Web"))
                {
                    acbfDocument->metaData()->documentInfo()->setSource(QStringList(xmlReader.readElementText()));
                }

                //One short, trade, etc.
                else if(xmlReader.name() == QStringLiteral("Format"))
                {
                    keywords.append(xmlReader.readElementText());
                }

                //Is this a manga?
                else if(xmlReader.name() == QStringLiteral("Manga"))
                {
                    if (xmlReader.readElementText() == "Yes") {
                        acbfDocument->metaData()->bookInfo()->setGenre("manga");
                        acbfDocument->metaData()->bookInfo()->setRightToLeft(true);
                    }
                }
                //Content rating...
                else if(xmlReader.name() == QStringLiteral("AgeRating"))
                {
                    acbfDocument->metaData()->bookInfo()->addContentRating(xmlReader.readElementText());
                }

                //Authors...
                else if(xmlReader.name() == QStringLiteral("Writer"))
                {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Writer", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                }
                else if(xmlReader.name() == QStringLiteral("Plotter"))
                {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Writer", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                }
                else if(xmlReader.name() == QStringLiteral("Scripter"))
                {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Writer", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                }
                else if(xmlReader.name() == QStringLiteral("Penciller"))
                {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Penciller", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                }
                else if(xmlReader.name() == QStringLiteral("Inker"))
                {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Inker", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                }
                else if(xmlReader.name() == QStringLiteral("Colorist"))
                {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Colorist", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                }
                else if(xmlReader.name() == QStringLiteral("CoverArtist"))
                {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("CoverArtist", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                }
                else if(xmlReader.name() == QStringLiteral("Letterer"))
                {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Letterer", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                }
                else if(xmlReader.name() == QStringLiteral("Editor"))
                {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Editor", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                }
                else if(xmlReader.name() == QStringLiteral("Other"))
                {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Other", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                }
                //Characters...
                else if(xmlReader.name() == QStringLiteral("Characters"))
                {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addCharacter(people.at(i).trimmed());
                    }
                }
                //Throw the rest into the keywords.
                else if(xmlReader.name() == QStringLiteral("Teams"))
                {
                    QStringList teams = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < teams.size(); i++) {
                        keywords.append(teams.at(i).trimmed());
                    }
                }
                else if(xmlReader.name() == QStringLiteral("Locations"))
                {
                    QStringList locations = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < locations.size(); i++) {
                        keywords.append(locations.at(i).trimmed());
                    }
                }
                else if(xmlReader.name() == QStringLiteral("StoryArc"))
                {
                    QStringList arc = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < arc.size(); i++) {
                        keywords.append(arc.at(i).trimmed());
                    }
                }
                else if(xmlReader.name() == QStringLiteral("SeriesGroup"))
                {
                    QStringList group = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i=0; i < group.size(); i++) {
                        keywords.append(group.at(i).trimmed());
                    }
                }

                //Pages...
                else if(xmlReader.name() == QStringLiteral("Pages")) {
                    while(xmlReader.readNextStartElement()) {
                        if(xmlReader.name() == QStringLiteral("Page"))
                        {
                            int index = xmlReader.attributes().value(QStringLiteral("Image")).toInt();
                            QString type = xmlReader.attributes().value(QStringLiteral("Type")).toString();
                            QString bookmark = xmlReader.attributes().value(QStringLiteral("Bookmark")).toString();
                            AdvancedComicBookFormat::Page* page = new AdvancedComicBookFormat::Page(acbfDocument);
                            page->setImageHref(entries.at(index));
                            if (type ==  QStringLiteral("FrontCover")) {
                                acbfDocument->metaData()->bookInfo()->setCoverpage(page);
                            } else {
                                if (bookmark.isEmpty()) {
                                    page->setTitle(type.append(QString::number(index)));
                                } else {
                                    page->setTitle(bookmark);
                                }
                                acbfDocument->body()->addPage(page, index-1);
                            }
                            xmlReader.readNext();
                        }
                    }
                }

                else
                {
                    qCWarning(QTQUICK_LOG) << Q_FUNC_INFO << "currently unsupported subsection:" << xmlReader.name();
                    xmlReader.skipCurrentElement();
                }
            }

            if (!series.isEmpty() && number>-1) {
                acbfDocument->metaData()->bookInfo()->addSequence(number, series, volume);
            }
            if (!seriesAlt.isEmpty() && numberAlt>-1) {
                acbfDocument->metaData()->bookInfo()->addSequence(numberAlt, seriesAlt, volumeAlt);
            }

            if (year > 0 || month > 0 || day > 0) {
                //acbfDocument->metaData()->publishInfo()->setPublishDateFromInts(year, month, day);
            }

            if (publisher.size()>0) {
                acbfDocument->metaData()->publishInfo()->setPublisher(publisher.join(", "));
            }

            if (keywords.size()>0) {
                acbfDocument->metaData()->bookInfo()->setKeywords(keywords, "");
            }

            if (acbfDocument->metaData()->bookInfo()->languages().size()>0) {
                QString lang = acbfDocument->metaData()->bookInfo()->languageEntryList().at(0);
                acbfDocument->metaData()->bookInfo()->setTitle(acbfDocument->metaData()->bookInfo()->title(""), lang);
                acbfDocument->metaData()->bookInfo()->setAnnotation(acbfDocument->metaData()->bookInfo()->annotation(""), lang);
                acbfDocument->metaData()->bookInfo()->setKeywords(acbfDocument->metaData()->bookInfo()->keywords(""), lang);
            }
        }
    }

    if (xmlReader.hasError()) {
        qCWarning(QTQUICK_LOG) << Q_FUNC_INFO << "Failed to read Comic Info XML document at token" << xmlReader.name() << "(" << xmlReader.lineNumber() << ":" << xmlReader.columnNumber() << ") The reported error was:" << xmlReader.errorString();
    }
    qCDebug(QTQUICK_LOG) << Q_FUNC_INFO << "Completed ACBF document creation from ComicInfo.xml for" << acbfDocument->metaData()->bookInfo()->title();
    acbfData = acbfDocument;
    return !xmlReader.hasError();
}

bool ArchiveBookModel::loadCoMet(QStringList xmlDocuments, QObject *acbfData, QStringList entries, QString filename)
{
    KFileMetaData::UserMetaData filedata(filename);
    AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(acbfData);
    Q_FOREACH(const QString xmlDocument, xmlDocuments) {
        QMutexLocker locker(&archiveMutex);
        const KArchiveFile* archFile = d->archive->directory()->file(xmlDocument);
        QXmlStreamReader xmlReader(archFile->data());
        if(xmlReader.readNextStartElement())
        {
            if(xmlReader.name() == QStringLiteral("comet"))
            {
                // We'll need to collect several items to generate a series. Thankfully, comicinfo only has two types of series.
                QString series;
                int number = -1;
                int volume = 0;

                QStringList keywords;
                QStringList empty;

                while(xmlReader.readNextStartElement())
                {
                    if(xmlReader.name() == QStringLiteral("title"))
                    {
                        acbfDocument->metaData()->bookInfo()->setTitle(xmlReader.readElementText(),"");
                    }

                    //Summary/annotation.
                    else if(xmlReader.name() == QStringLiteral("description"))
                    {
                        acbfDocument->metaData()->bookInfo()->setAnnotation(xmlReader.readElementText().split("\n\n"), "");
                    }

                    /*
                     * This ought to go into the kfile metadata.
                     */
                    //pages
                    else if(xmlReader.name() == QStringLiteral("pages"))
                    {
                        filedata.setAttribute("Peruse.totalPages", xmlReader.readElementText());
                    }
                    //curentpage -- only read this when there's no such entry.
                    else if(xmlReader.name() == QStringLiteral("lastMark"))
                    {
                        if (!filedata.hasAttribute("Peruse.currentPage")) {
                            filedata.setAttribute("Peruse.currentPage", xmlReader.readElementText());
                        } else {
                            xmlReader.skipCurrentElement();
                        }
                    }

                    //Series

                    else if(xmlReader.name() == QStringLiteral("series"))
                    {
                        series = xmlReader.readElementText();
                    }
                    else if(xmlReader.name() == QStringLiteral("issue"))
                    {
                        number = xmlReader.readElementText().toInt();
                    }
                    else if(xmlReader.name() == QStringLiteral("volume"))
                    {
                        volume = xmlReader.readElementText().toInt();
                    }

                    // Publishing date.

                    else if(xmlReader.name() == QStringLiteral("date"))
                    {
                        acbfDocument->metaData()->publishInfo()->setPublishDate(QDate::fromString(xmlReader.readElementText(), Qt::ISODate));
                    }

                    //Publisher

                    else if(xmlReader.name() == QStringLiteral("publisher"))
                    {
                        acbfDocument->metaData()->publishInfo()->setPublisher(xmlReader.readElementText());
                    }
                    else if(xmlReader.name() == QStringLiteral("rights"))
                    {
                        acbfDocument->metaData()->publishInfo()->setLicense(xmlReader.readElementText());
                    }
                    else if(xmlReader.name() == QStringLiteral("identifier"))
                    {
                        acbfDocument->metaData()->publishInfo()->setIsbn(xmlReader.readElementText());
                    }

                    //Genre
                    else if(xmlReader.name() == QStringLiteral("genre"))
                    {
                        QString key = xmlReader.readElementText();
                        QString genreKey = key.toLower().replace(" ", "_");
                        if (acbfDocument->metaData()->bookInfo()->availableGenres().contains(genreKey)) {
                            acbfDocument->metaData()->bookInfo()->setGenre(genreKey);
                        } else {
                            keywords.append(key);
                        }
                    }

                    //Language

                    else if(xmlReader.name() == QStringLiteral("language"))
                    {
                        acbfDocument->metaData()->bookInfo()->addLanguage(xmlReader.readElementText());
                    }

                    //Sources/Weblink
                    else if(xmlReader.name() == QStringLiteral("isVersionOf"))
                    {
                        acbfDocument->metaData()->documentInfo()->setSource(QStringList(xmlReader.readElementText()));
                    }

                    //One short, trade, etc.
                    else if(xmlReader.name() == QStringLiteral("format"))
                    {
                        keywords.append(xmlReader.readElementText());
                    }

                    //Is this a manga?
                    else if(xmlReader.name() == QStringLiteral("readingDirection"))
                    {
                        if (xmlReader.readElementText() == "rtl") {
                            acbfDocument->metaData()->bookInfo()->setRightToLeft(true);
                        }
                    }
                    //Content rating...
                    else if(xmlReader.name() == QStringLiteral("rating"))
                    {
                        acbfDocument->metaData()->bookInfo()->addContentRating(xmlReader.readElementText());
                    }

                    //Authors...
                    else if(xmlReader.name() == QStringLiteral("writer"))
                    {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Writer", "", "", "", "", person, empty, empty);
                    }
                    else if(xmlReader.name() == QStringLiteral("creator"))
                    {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Writer", "", "", "", "", person, empty, empty);
                    }
                    else if(xmlReader.name() == QStringLiteral("penciller"))
                    {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Penciller", "", "", "", "", person, empty, empty);
                    }
                    else if(xmlReader.name() == QStringLiteral("editor"))
                    {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Editor", "", "", "", "", person, empty, empty);
                    }
                    else if(xmlReader.name() == QStringLiteral("coverDesigner"))
                    {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("CoverArtist", "", "", "", "", person, empty, empty);
                    }
                    else if(xmlReader.name() == QStringLiteral("letterer"))
                    {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Letterer", "", "", "", "", person, empty, empty);
                    }
                    else if(xmlReader.name() == QStringLiteral("inker"))
                    {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Inker", "", "", "", "", person, empty, empty);
                    }
                    else if(xmlReader.name() == QStringLiteral("colorist"))
                    {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Colorist", "", "", "", "", person, empty, empty);
                    }

                    //Characters
                    else if(xmlReader.name() == QStringLiteral("character"))
                    {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addCharacter(person);
                    }


                    //Get the cover image, set it, remove it from entries, then remove all other entries.
                    else if(xmlReader.name() == QStringLiteral("coverImage"))
                    {
                        QString url = xmlReader.readElementText();
                        AdvancedComicBookFormat::Page* cover = new AdvancedComicBookFormat::Page(acbfDocument);
                        cover->setImageHref(url);
                        acbfDocument->metaData()->bookInfo()->setCoverpage(cover);
                        entries.removeAll(url);
                        Q_FOREACH(QString entry, entries) {
                            AdvancedComicBookFormat::Page* page = new AdvancedComicBookFormat::Page(acbfDocument);
                            page->setImageHref(entry);
                            acbfDocument->body()->addPage(page);
                        }
                        xmlReader.readNext();
                    }

                    else
                    {
                        qCWarning(QTQUICK_LOG) << Q_FUNC_INFO << "currently unsupported subsection:" << xmlReader.name();
                        xmlReader.skipCurrentElement();
                    }
                }

                if (!series.isEmpty() && number>-1) {
                    acbfDocument->metaData()->bookInfo()->addSequence(number, series, volume);
                }

                if (acbfDocument->metaData()->bookInfo()->genres().size()==0) {
                    //There must always be a genre in a proper acbf file...
                    acbfDocument->metaData()->bookInfo()->setGenre("other");
                }

                if (keywords.size()>0) {
                    acbfDocument->metaData()->bookInfo()->setKeywords(keywords, "");
                }

                if (acbfDocument->metaData()->bookInfo()->languages().size()>0) {
                    QString lang = acbfDocument->metaData()->bookInfo()->languageEntryList().at(0);
                    acbfDocument->metaData()->bookInfo()->setTitle(acbfDocument->metaData()->bookInfo()->title(""), lang);
                    acbfDocument->metaData()->bookInfo()->setAnnotation(acbfDocument->metaData()->bookInfo()->annotation(""), lang);
                    acbfDocument->metaData()->bookInfo()->setKeywords(acbfDocument->metaData()->bookInfo()->keywords(""), lang);
                }
            }
            if (xmlReader.hasError()) {
                qCWarning(QTQUICK_LOG) << Q_FUNC_INFO << "Failed to read CoMet document at token" << xmlReader.name()
                           << "(" << xmlReader.lineNumber() << ":"
                           << xmlReader.columnNumber() << ") The reported error was:" << xmlReader.errorString();
            }
            qCDebug(QTQUICK_LOG) << Q_FUNC_INFO << "Completed ACBF document creation from CoMet for" << acbfDocument->metaData()->bookInfo()->title();
            acbfData = acbfDocument;
            return !xmlReader.hasError();
        } else {
            xmlReader.skipCurrentElement();
        }
    }
    return false;
}

QString ArchiveBookModel::previewForId(const QString& id) const
{
    static const QString directorySplit{"/"};
    static const QString period{"."};
    static const QString acbfSuffix{"acbf"};
    if (d->archive) {
        if (id.splitRef(directorySplit).last().contains(period)) {
            const QString suffix = id.splitRef(period).last().toString().toLower();
            if (d->imageProvider && QImageReader::supportedImageFormats().contains(suffix.toLatin1())) {
                return QString("image://%1/%2").arg(d->imageProvider->prefix()).arg(id);
            } else if (suffix == acbfSuffix) {
                return QString{"image://icon/data-information"};
            } else {
                QList<QMimeType> mimetypes = d->mimeDatabase.mimeTypesForFileName(id);
                if (mimetypes.count() > 0) {
                    return QString("image://icon/").append(mimetypes.first().iconName());
                }
            }
        } else {
            return QString{"image://icon/folder"};
        }
    }
    return QString();
}
