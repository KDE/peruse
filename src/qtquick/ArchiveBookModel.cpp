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
#include <AcbfDocumentinfo.h>
#include <AcbfMetadata.h>
#include <AcbfPage.h>
#include <AcbfPublishinfo.h>
#include <AcbfStyleSheet.h>

#include <QCoreApplication>
#include <QDir>
#include <QFontDatabase>
#include <QImageReader>
#include <QMimeDatabase>
#include <QQmlEngine>
#include <QTemporaryFile>
#include <QXmlStreamReader>

#include "KRar.h" // "" because it's a custom thing for now
#include <KFileMetaData/UserMetaData>
#include <KLocalizedString>
#include <karchive.h>
#include <kzip.h>

#include <AcbfData.h>
#include <QRegularExpression>
#include <qtquick_debug.h>

class ArchiveBookModel::Private
{
public:
    Private(ArchiveBookModel *qq)
        : q(qq)
        , archive(nullptr)
        , readWrite(false)
        , imageProvider(nullptr)
        , isDirty(false)
        , isLoading(false)
    {
    }
    ~Private()
    {
        for (int fontId : fontIdByFilename.values()) {
            fontDatabase.removeApplicationFont(fontId);
        }
        delete archive;
    }
    ArchiveBookModel *q;
    KArchive *archive;
    QStringList fileEntries;
    QStringList fileEntriesToDelete;
    QHash<QString, const KArchiveFile *> archiveFiles;
    bool readWrite;
    ArchiveImageProvider *imageProvider;
    bool isDirty;
    bool isLoading;
    QMimeDatabase mimeDatabase;
    QFontDatabase fontDatabase;
    QHash<QString, int> fontIdByFilename;
    QString acbfEntryName;

    void closeBook()
    {
        q->beginResetModel();
        if (archive) {
            q->clearPages();
            archiveFiles.clear();
            archive->close();
            delete archive;
            archive = nullptr;
        }
        if (imageProvider) {
            auto engine = qmlEngine(q);
            engine->removeImageProvider(imageProvider->prefix());
        }
        imageProvider = nullptr;
        fileEntries.clear();
        Q_EMIT q->fileEntriesChanged();
        fileEntriesToDelete.clear();
        Q_EMIT q->fileEntriesToDeleteChanged();
        q->endResetModel();
        acbfEntryName.clear();
    }

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

    AdvancedComicBookFormat::Document *createNewAcbfDocumentFromLegacyInformation()
    {
        AdvancedComicBookFormat::Document *acbfDocument = new AdvancedComicBookFormat::Document(q);

        acbfDocument->metaData()->bookInfo()->setTitle(q->title());

        AdvancedComicBookFormat::Author *author = new AdvancedComicBookFormat::Author(acbfDocument->metaData());
        author->setNickName(q->author());
        acbfDocument->metaData()->bookInfo()->addAuthor(author);

        acbfDocument->metaData()->publishInfo()->setPublisher(q->publisher());

        int prefixLength = QString("image://%1/").arg(imageProvider->prefix()).length();
        if (q->pageCount() > 0) {
            // First, let's see if we have something called "*cover*"... because that would be handy and useful
            int cover = -1;
            for (int i = q->pageCount(); i > -1; --i) {
                QString url = q->data(q->index(i, 0, QModelIndex()), BookModel::UrlRole).toString().mid(prefixLength);
                // Yup, this is a bit sort of naughty and stuff... but, assume index 0 is the cover if nothing else has shown up...
                // FIXME this will also fail when there's more than one cover... say, back and front covers...
                if (url.split('/').last().contains("cover", Qt::CaseInsensitive) || i == 0) {
                    acbfDocument->metaData()->bookInfo()->coverpage()->setImageHref(url);
                    acbfDocument->metaData()->bookInfo()->coverpage()->setTitle(q->data(q->index(0, 0, QModelIndex()), BookModel::TitleRole).toString());
                    cover = i;
                    break;
                }
            }

            for (int i = 0; i < q->pageCount(); ++i) {
                if (i == cover) {
                    continue;
                }
                AdvancedComicBookFormat::Page *page = new AdvancedComicBookFormat::Page(acbfDocument);
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

void ArchiveBookModel::setAcbfData(QObject *obj)
{
    auto old = acbfData();

    if (old) {
        if (auto document = qobject_cast<AdvancedComicBookFormat::Document *>(old)) {
            disconnect(document->styleSheet(), &AdvancedComicBookFormat::StyleSheet::stylesChanged, this, nullptr);
        }
    }

    BookModel::setAcbfData(obj);

    if (obj) {
        if (auto document = qobject_cast<AdvancedComicBookFormat::Document *>(obj)) {
            connect(document->styleSheet(), &AdvancedComicBookFormat::StyleSheet::stylesChanged, this, [this]() {
                d->setDirty();
            });
        }
    }
}

ArchiveBookModel::ArchiveBookModel(QObject *parent)
    : BookModel(parent)
    , d(new Private(this))
{
}

ArchiveBookModel::~ArchiveBookModel()
{
    d->closeBook();
    delete d;
}

QStringList recursiveEntries(const KArchiveDirectory *dir)
{
    QStringList entries = dir->entries();
    QStringList allEntries = entries;
    for (const QString &entryName : std::as_const(entries)) {
        const KArchiveEntry *entry = dir->entry(entryName);
        if (entry->isDirectory()) {
            const KArchiveDirectory *subDir = static_cast<const KArchiveDirectory *>(entry);
            QStringList subEntries = recursiveEntries(subDir);
            for (const QString &subEntry : std::as_const(subEntries)) {
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
    d->closeBook();
    beginResetModel();

    QMimeType mime = d->mimeDatabase.mimeTypeForFile(newFilename);
    if (mime.inherits("application/zip")) {
        d->archive = new KZip(newFilename);
    } else if (mime.inherits("application/x-rar")) {
        d->archive = new KRar(newFilename);
    }

    bool success = false;
    if (d->archive) {
        QString prefix = QString("archivebookpage%1").arg(QString::number(Private::counter()));
        if (d->archive->open(QIODevice::ReadOnly)) {
            QMutexLocker locker(&archiveMutex);
            d->imageProvider = new ArchiveImageProvider();
            d->imageProvider->setArchiveBookModel(this);
            d->imageProvider->setPrefix(prefix);
            auto engine = qmlEngine(this);
            engine->addImageProvider(prefix, d->imageProvider);

            d->fileEntries = recursiveEntries(d->archive->directory());
            d->fileEntries.sort();
            Q_EMIT fileEntriesChanged();

            // First check and see if we've got an ACBF document in there...
            QString comicInfoEntry;
            QStringList xmlFiles;
            QLatin1String acbfSuffix(".acbf");
            QLatin1String ComicInfoXML("comicinfo.xml");
            QLatin1String xmlSuffix(".xml");
            QStringList images;
            for (const QString &entry : std::as_const(d->fileEntries)) {
                if (entry.toLower().endsWith(acbfSuffix)) {
                    d->acbfEntryName = entry;
                    break;
                }
                if (entry.toLower().endsWith(xmlSuffix)) {
                    if (entry.toLower().endsWith(ComicInfoXML)) {
                        comicInfoEntry = entry;
                    } else {
                        xmlFiles.append(entry);
                    }
                }
                if (entry.toLower().endsWith(".jpg") || entry.toLower().endsWith(".jpeg") || entry.toLower().endsWith(".gif")
                    || entry.toLower().endsWith(".png") || entry.toLower().endsWith(".webp")) {
                    images.append(entry);
                }
            }
            images.sort();
            if (!d->acbfEntryName.isEmpty()) {
                AdvancedComicBookFormat::Document *acbfDocument = new AdvancedComicBookFormat::Document(this);
                const KArchiveFile *archFile = d->archive->directory()->file(d->acbfEntryName);
                if (acbfDocument->fromXml(QString(archFile->data()))) {
                    setAcbfData(acbfDocument);
                    addPage(QString("image://%1/%2").arg(prefix).arg(acbfDocument->metaData()->bookInfo()->coverpage()->imageHref()),
                            acbfDocument->metaData()->bookInfo()->coverpage()->title());
                    for (AdvancedComicBookFormat::Page *page : acbfDocument->body()->pages()) {
                        addPage(QString("image://%1/%2").arg(prefix).arg(page->imageHref()), page->title());
                    }
                } else {
                    // just in case this is, for whatever reason, being reused...
                    setAcbfData(nullptr);
                }
            } else if (!comicInfoEntry.isEmpty() || !xmlFiles.isEmpty()) {
                AdvancedComicBookFormat::Document *acbfDocument = new AdvancedComicBookFormat::Document(this);
                const KArchiveFile *archFile = d->archive->directory()->file(comicInfoEntry);
                bool loadData = false;

                if (!comicInfoEntry.isEmpty()) {
                    loadData = loadComicInfoXML(archFile->data(), acbfDocument, images, newFilename);
                } else {
                    loadData = loadCoMet(xmlFiles, acbfDocument, images, newFilename);
                }

                if (loadData) {
                    setAcbfData(acbfDocument);
                    QString undesired = QString("%1").arg("/").append("Thumbs.db");
                    addPage(QString("image://%1/%2").arg(prefix).arg(acbfDocument->metaData()->bookInfo()->coverpage()->imageHref()),
                            acbfDocument->metaData()->bookInfo()->coverpage()->title());
                    for (AdvancedComicBookFormat::Page *page : acbfDocument->body()->pages()) {
                        addPage(QString("image://%1/%2").arg(prefix).arg(page->imageHref()), page->title());
                    }
                }
            }
            if (!acbfData()) {
                // fall back to just handling the files directly if there's no ACBF document...
                QString undesired = QString("%1").arg("/").append("Thumbs.db");
                for (const QString &entry : std::as_const(d->fileEntries)) {
                    const KArchiveEntry *archEntry = d->archive->directory()->entry(entry);
                    if (archEntry->isFile() && !entry.endsWith(undesired)) {
                        addPage(QString("image://%1/%2").arg(prefix).arg(entry), entry.split("/").last());
                    }
                }
            }
            d->archive->close();
            success = true;
        } else {
            qCDebug(QTQUICK_LOG) << "Failed to open archive";
        }
    }

    //     QDir dir(newFilename);
    //     if(dir.exists())
    //     {
    //         QFileInfoList entries = dir.entryInfoList(QDir::Files, QDir::Name);
    //         for(const QFileInfo& entry : entries)
    //         {
    //             addPage(QString("file://").append(entry.canonicalFilePath()), entry.fileName());
    //         }
    //     }
    BookModel::setFilename(newFilename);

    KFileMetaData::UserMetaData data(newFilename);
    if (data.hasAttribute("peruse.currentPage"))
        BookModel::setCurrentPage(data.attribute("peruse.currentPage").toInt(), false);

    if (!acbfData() && d->readWrite && d->imageProvider) {
        d->createNewAcbfDocumentFromLegacyInformation();
    }

    d->isLoading = false;
    emit loadingCompleted(success);
    setProcessing(false);
    endResetModel();
}

QString ArchiveBookModel::author() const
{
    AdvancedComicBookFormat::Document *acbfDocument = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData());
    if (acbfDocument) {
        if (acbfDocument->metaData()->bookInfo()->author().count() > 0) {
            return acbfDocument->metaData()->bookInfo()->author().at(0)->displayName();
        }
    }
    return BookModel::author();
}

void ArchiveBookModel::setAuthor(QString newAuthor)
{
    if (!d->isLoading) {
        AdvancedComicBookFormat::Document *acbfDocument = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData());
        if (!acbfDocument) {
            acbfDocument = d->createNewAcbfDocumentFromLegacyInformation();
        }
        if (acbfDocument->metaData()->bookInfo()->author().count() == 0) {
            AdvancedComicBookFormat::Author *author = new AdvancedComicBookFormat::Author(acbfDocument->metaData());
            author->setNickName(newAuthor);
            acbfDocument->metaData()->bookInfo()->addAuthor(author);
        } else {
            acbfDocument->metaData()->bookInfo()->author().at(0)->setNickName(newAuthor);
        }
    }
    BookModel::setAuthor(newAuthor);
}

QString ArchiveBookModel::publisher() const
{
    AdvancedComicBookFormat::Document *acbfDocument = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData());
    if (acbfDocument) {
        if (!acbfDocument->metaData()->publishInfo()->publisher().isEmpty()) {
            return acbfDocument->metaData()->publishInfo()->publisher();
        }
    }
    return BookModel::publisher();
}

void ArchiveBookModel::setPublisher(QString newPublisher)
{
    if (!d->isLoading) {
        AdvancedComicBookFormat::Document *acbfDocument = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData());
        if (!acbfDocument) {
            acbfDocument = d->createNewAcbfDocumentFromLegacyInformation();
        }
        acbfDocument->metaData()->publishInfo()->setPublisher(newPublisher);
    }
    BookModel::setAuthor(newPublisher);
}

QString ArchiveBookModel::title() const
{
    AdvancedComicBookFormat::Document *acbfDocument = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData());
    if (acbfDocument) {
        if (acbfDocument->metaData()->bookInfo()->title().length() > 0) {
            return acbfDocument->metaData()->bookInfo()->title();
        }
    }
    return BookModel::title();
}

void ArchiveBookModel::setTitle(QString newTitle)
{
    if (!d->isLoading) {
        AdvancedComicBookFormat::Document *acbfDocument = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData());
        if (!acbfDocument) {
            acbfDocument = d->createNewAcbfDocumentFromLegacyInformation();
        }
        acbfDocument->metaData()->bookInfo()->setTitle(newTitle);
    }
    BookModel::setTitle(newTitle);
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

int ArchiveBookModel::fileEntryReferenced(const QString &fileEntry) const
{
    int isReferenced{0};
    AdvancedComicBookFormat::Document *document = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData());
    if (document->metaData()->bookInfo()->coverpage()->imageHref() == fileEntry) {
        isReferenced = 1;
    }
    if (!isReferenced) {
        for (const AdvancedComicBookFormat::Page *page : document->body()->pages()) {
            if (page->imageHref() == fileEntry) {
                isReferenced = 1;
                break;
            }
        }
    }
    if (!isReferenced) {
        for (const QObject *obj : document->styleSheet()->styles()) {
            const AdvancedComicBookFormat::Style *style = qobject_cast<const AdvancedComicBookFormat::Style *>(obj);
            const QString styleString{style->toString()};
            if (styleString.contains(fileEntry)) {
                isReferenced = 1;
                break;
            } else if (styleString.contains(fileEntry.split("/").last())) {
                isReferenced = 2;
                break;
            }
        }
    }
    return isReferenced;
}

bool ArchiveBookModel::fileEntryIsDirectory(const QString &fileEntry) const
{
    bool isDirectory{false};
    const KArchiveEntry *entry = d->archive->directory()->entry(fileEntry);
    if (entry && entry->isDirectory()) {
        isDirectory = true;
    }
    return isDirectory;
}

QStringList ArchiveBookModel::fileEntriesToDelete() const
{
    return d->fileEntriesToDelete;
}

void ArchiveBookModel::markArchiveFileForDeletion(const QString &archiveFile, bool markForDeletion)
{
    if (markForDeletion) {
        if (!d->fileEntriesToDelete.contains(archiveFile)) {
            d->fileEntriesToDelete << archiveFile;
            Q_EMIT fileEntriesToDeleteChanged();
        }
    } else {
        if (d->fileEntriesToDelete.contains(archiveFile)) {
            d->fileEntriesToDelete.removeAll(archiveFile);
            Q_EMIT fileEntriesToDeleteChanged();
        }
    }
}

bool ArchiveBookModel::saveBook()
{
    bool success = true;
    if (d->isDirty) {
        QMutexLocker locker(&archiveMutex);
        // TODO get new filenames out of acbf

        setProcessing(true);
        qApp->processEvents();

        QTemporaryFile tmpFile(this);
        tmpFile.open();
        QString archiveFileName = tmpFile.fileName().append(".cbz");
        QFileInfo fileInfo(tmpFile);
        tmpFile.close();
        setProcessingDescription(i18n("Creating archive in %1", archiveFileName));
        KZip *archive = new KZip(archiveFileName);
        archive->open(QIODevice::ReadWrite);

        QString acbfFileName{d->acbfEntryName};
        if (acbfFileName.isEmpty()) {
            acbfFileName = QStringLiteral("metadata.acbf");
        } else {
            // If we actually /have/ an acbf filename already, let's not copy the old one across...
            d->fileEntriesToDelete << acbfFileName;
        }
        // We're a zip file... size isn't used
        setProcessingDescription(i18n("Writing in ACBF data"));
        archive->prepareWriting(acbfFileName, fileInfo.owner(), fileInfo.group(), 0);
        AdvancedComicBookFormat::Document *acbfDocument = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData());
        if (!acbfDocument) {
            acbfDocument = d->createNewAcbfDocumentFromLegacyInformation();
        }
        QByteArray acbfStringUtf8 = acbfDocument->toXml().toUtf8();
        archive->writeData(acbfStringUtf8, acbfStringUtf8.size());
        archive->finishWriting(acbfStringUtf8.size());

        setProcessingDescription(i18n("Copying across all files not marked for deletion"));
        const QStringList allFiles = fileEntries();
        const KArchiveFile *archFile{nullptr};
        for (const QString &file : allFiles) {
            qApp->processEvents();
            if (d->fileEntriesToDelete.contains(file)) {
                qCDebug(QTQUICK_LOG) << "Not copying file marked for deletion:" << file;
            } else {
                setProcessingDescription(i18n("Copying over %1", file));
                archFile = archiveFile(file);
                if (archFile && archFile->isFile()) {
                    archive->prepareWriting(file, archFile->user(), archFile->group(), 0);
                    archive->writeData(archFile->data(), archFile->size());
                    archive->finishWriting(archFile->size());
                }
            }
        }
        d->fileEntriesToDelete.clear();
        Q_EMIT fileEntriesToDeleteChanged();

        archive->close();
        qCDebug(QTQUICK_LOG) << "Archive created and closed...";

        // swap out the two files, tell model we're about to swap things out...
        beginResetModel();

        QString actualFile = d->archive->fileName();
        d->closeBook();

        // This seems roundabout... but it retains ctime and xattrs, which would be gone
        // if we just did a delete+rename
        QFile destinationFile(actualFile);
        if (destinationFile.open(QIODevice::WriteOnly)) {
            QFile originFile(archiveFileName);
            if (originFile.open(QIODevice::ReadOnly)) {
                setProcessingDescription(i18n("Copying all content from %1 to %2", archiveFileName, actualFile));
                while (!originFile.atEnd()) {
                    destinationFile.write(originFile.read(65536));
                    qApp->processEvents();
                }
                destinationFile.close();
                originFile.close();
                if (originFile.remove()) {
                    setProcessingDescription(i18n("Successfully replaced old archive with the new archive - now loading the new archive..."));
                    // now load the new thing...
                    locker.unlock();
                    setFilename(actualFile);
                    locker.relock();
                } else {
                    qCWarning(QTQUICK_LOG) << "Failed to delete" << originFile.fileName();
                }
            } else {
                qCWarning(QTQUICK_LOG) << "Failed to open" << originFile.fileName() << "for reading";
            }
        } else {
            qCWarning(QTQUICK_LOG) << "Failed to open" << destinationFile.fileName() << "for writing";
        }
        endResetModel();
    }
    setProcessing(false);
    setDirty(false);
    return success;
}

void ArchiveBookModel::addPage(QString url, QString title)
{
    // don't do this unless we're done loading... don't want to dirty things up until then!
    if (!d->isLoading) {
        AdvancedComicBookFormat::Document *acbfDocument = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData());
        if (!acbfDocument) {
            acbfDocument = d->createNewAcbfDocumentFromLegacyInformation();
        }
        QUrl imageUrl(url);
        if (pageCount() == 0) {
            acbfDocument->metaData()->bookInfo()->coverpage()->setTitle(title);
            acbfDocument->metaData()->bookInfo()->coverpage()->setImageHref(QString("%1/%2").arg(imageUrl.path().mid(1)).arg(imageUrl.fileName()));
        } else {
            AdvancedComicBookFormat::Page *page = new AdvancedComicBookFormat::Page(acbfDocument);
            page->setTitle(title);
            page->setImageHref(QString("%1/%2").arg(imageUrl.path().mid(1)).arg(imageUrl.fileName()));
            acbfDocument->body()->addPage(page);
        }
    }
    BookModel::addPage(url, title);
}

void ArchiveBookModel::removePage(int pageNumber)
{
    if (!d->isLoading) {
        AdvancedComicBookFormat::Document *acbfDocument = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData());
        if (!acbfDocument) {
            acbfDocument = d->createNewAcbfDocumentFromLegacyInformation();
        } else {
            if (pageNumber == 0) {
                // Page no 0 is the cover page, when removed we'd usually end up with no cover page
                // Normally we'd want to discourage this, but we do need to support the functionality
                AdvancedComicBookFormat::Page *cover = acbfDocument->metaData()->bookInfo()->coverpage();
                if (cover) {
                    cover->deleteLater();
                }
                AdvancedComicBookFormat::Page *page = acbfDocument->body()->page(0);
                acbfDocument->metaData()->bookInfo()->setCoverpage(page);
                if (page) {
                    acbfDocument->body()->removePage(page);
                }
            } else {
                AdvancedComicBookFormat::Page *page = acbfDocument->body()->page(pageNumber - 1);
                if (page) {
                    acbfDocument->body()->removePage(page);
                    page->deleteLater();
                }
            }
        }
    }
    BookModel::removePage(pageNumber);
}

// FIXME any metadata change sets dirty (as we need to replace the whole file in archive when saving)

void ArchiveBookModel::addPageFromFile(QString fileUrl, int insertAfter)
{
    if (d->archive && d->readWrite && !d->isDirty) {
        int insertionIndex = insertAfter;
        if (insertAfter < 0 || pageCount() - 1 < insertAfter) {
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

    AdvancedComicBookFormat::Document *acbfDocument = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData());

    // Cover pages are special, and in acbf they are very special... (otherwise they're page zero)
    if (swapThisIndex == 0) {
        AdvancedComicBookFormat::Page *oldCoverPage = acbfDocument->metaData()->bookInfo()->coverpage();
        AdvancedComicBookFormat::Page *otherPage = acbfDocument->body()->page(withThisIndex - 1);
        acbfDocument->body()->removePage(otherPage);
        acbfDocument->metaData()->bookInfo()->setCoverpage(otherPage);
        acbfDocument->body()->addPage(oldCoverPage, withThisIndex - 1);
    } else if (withThisIndex == 0) {
        AdvancedComicBookFormat::Page *oldCoverPage = acbfDocument->metaData()->bookInfo()->coverpage();
        AdvancedComicBookFormat::Page *otherPage = acbfDocument->body()->page(swapThisIndex - 1);
        acbfDocument->body()->removePage(otherPage);
        acbfDocument->metaData()->bookInfo()->setCoverpage(otherPage);
        acbfDocument->body()->addPage(oldCoverPage, swapThisIndex - 1);
    } else {
        AdvancedComicBookFormat::Page *firstPage = acbfDocument->body()->page(swapThisIndex - 1);
        AdvancedComicBookFormat::Page *otherPage = acbfDocument->body()->page(withThisIndex - 1);
        acbfDocument->body()->swapPages(firstPage, otherPage);
    }

    // FIXME only treat things which have sequential numbers in as pages, split out chapters automatically?

    BookModel::swapPages(swapThisIndex, withThisIndex);
}

bool ArchiveBookModel::createBook(const QUrl &fileName, const QString &title, const QUrl &coverUrl)
{
    bool success = true;

    QString fileTitle = title;
    fileTitle = fileTitle.replace(QRegularExpression("\\W"), {}).simplified();

    ArchiveBookModel *model = new ArchiveBookModel(nullptr);
    model->setReadWrite(true);
    QString prefix = QString("archivebookpage%1").arg(QString::number(Private::counter()));
    model->d->imageProvider = new ArchiveImageProvider();
    model->d->imageProvider->setArchiveBookModel(model);
    model->d->imageProvider->setPrefix(prefix);
    model->d->archive = new KZip(fileName.toLocalFile());
    model->BookModel::setFilename(fileName.toLocalFile());
    model->setTitle(title);
    AdvancedComicBookFormat::Document *acbfDocument = qobject_cast<AdvancedComicBookFormat::Document *>(model->acbfData());
    QString coverArchiveName = QString("cover.%1").arg(QFileInfo(coverUrl.toLocalFile()).completeSuffix());
    acbfDocument->metaData()->bookInfo()->coverpage()->setImageHref(coverArchiveName);
    success = model->saveBook();

    model->d->archive->close();
    model->d->archive->open(QIODevice::ReadWrite);
    model->d->archive->addLocalFile(coverUrl.toLocalFile(), coverArchiveName);
    model->d->fileEntries << coverArchiveName;
    model->d->fileEntries.sort();
    Q_EMIT model->fileEntriesChanged();
    model->d->archive->close();

    model->deleteLater();

    return success;
}

const KArchiveFile *ArchiveBookModel::archiveFile(const QString &filePath) const
{
    if (d->archive->isOpen() == false) {
        d->archive->open(QIODevice::ReadOnly);
    }
    if (d->archive) {
        if (!d->archiveFiles.contains(filePath)) {
            d->archiveFiles[filePath] = d->archive->directory()->file(filePath);
        }
        return d->archiveFiles[filePath];
    }
    return nullptr;
}

bool ArchiveBookModel::loadComicInfoXML(QString xmlDocument, QObject *acbfData, QStringList entries, QString filename)
{
    KFileMetaData::UserMetaData filedata(filename);
    AdvancedComicBookFormat::Document *acbfDocument = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData);
    QXmlStreamReader xmlReader(xmlDocument);
    if (xmlReader.readNextStartElement()) {
        if (xmlReader.name() == QStringLiteral("ComicInfo")) {
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

            while (xmlReader.readNextStartElement()) {
                if (xmlReader.name() == QStringLiteral("Title")) {
                    acbfDocument->metaData()->bookInfo()->setTitle(xmlReader.readElementText(), "");
                }

                // Summary/annotation.
                else if (xmlReader.name() == QStringLiteral("Summary")) {
                    acbfDocument->metaData()->bookInfo()->setAnnotation(xmlReader.readElementText().split("\n\n"), "");
                }

                /*
                 * This ought to go into the kfile metadata.
                 */
                else if (xmlReader.name() == QStringLiteral("Notes")) {
                    if (filedata.userComment().isEmpty()) {
                        filedata.setUserComment(xmlReader.readElementText());
                    } else {
                        xmlReader.skipCurrentElement();
                    }
                } else if (xmlReader.name() == QStringLiteral("Tags")) {
                    QStringList tags = filedata.tags();
                    QStringList newTags = xmlReader.readElementText().split(",");
                    for (int i = 0; i < newTags.size(); i++) {
                        if (!tags.contains(newTags.at(i))) {
                            tags.append(newTags.at(i));
                        }
                    }
                    filedata.setTags(tags);
                } else if (xmlReader.name() == QStringLiteral("PageCount")) {
                    filedata.setAttribute("Peruse.totalPages", xmlReader.readElementText());
                } else if (xmlReader.name() == QStringLiteral("ScanInformation")) {
                    QString userComment = filedata.userComment();
                    userComment.append("\n" + xmlReader.readElementText());
                    filedata.setUserComment(userComment);
                }

                // Series

                else if (xmlReader.name() == QStringLiteral("Series")) {
                    series = xmlReader.readElementText();
                } else if (xmlReader.name() == QStringLiteral("Number")) {
                    number = xmlReader.readElementText().toInt();
                } else if (xmlReader.name() == QStringLiteral("Volume")) {
                    volume = xmlReader.readElementText().toInt();
                }

                // Series alt

                else if (xmlReader.name() == QStringLiteral("AlternateSeries")) {
                    seriesAlt = xmlReader.readElementText();
                } else if (xmlReader.name() == QStringLiteral("AlternateNumber")) {
                    numberAlt = xmlReader.readElementText().toInt();
                } else if (xmlReader.name() == QStringLiteral("AlternateVolume")) {
                    volumeAlt = xmlReader.readElementText().toInt();
                }

                // Publishing date.

                else if (xmlReader.name() == QStringLiteral("Year")) {
                    year = xmlReader.readElementText().toInt();
                } else if (xmlReader.name() == QStringLiteral("Month")) {
                    month = xmlReader.readElementText().toInt();
                } else if (xmlReader.name() == QStringLiteral("Day")) {
                    day = xmlReader.readElementText().toInt();
                }

                // Publisher

                else if (xmlReader.name() == QStringLiteral("Publisher")) {
                    publisher.append(xmlReader.readElementText());
                } else if (xmlReader.name() == QStringLiteral("Imprint")) {
                    publisher.append(xmlReader.readElementText());
                }

                // Genre
                else if (xmlReader.name() == QStringLiteral("Genre")) {
                    QString key = xmlReader.readElementText();
                    QString genreKey = key.toLower().replace(" ", "_");
                    if (acbfDocument->metaData()->bookInfo()->availableGenres().contains(genreKey)) {
                        acbfDocument->metaData()->bookInfo()->setGenre(genreKey);
                    } else {
                        // There must always be a genre in a proper acbf file...
                        acbfDocument->metaData()->bookInfo()->setGenre("other");
                        keywords.append(key);
                    }
                }

                // Language

                else if (xmlReader.name() == QStringLiteral("LanguageISO")) {
                    acbfDocument->metaData()->bookInfo()->addLanguage(xmlReader.readElementText());
                }

                // Sources/Weblink
                else if (xmlReader.name() == QStringLiteral("Web")) {
                    acbfDocument->metaData()->documentInfo()->setSource(QStringList(xmlReader.readElementText()));
                }

                // One short, trade, etc.
                else if (xmlReader.name() == QStringLiteral("Format")) {
                    keywords.append(xmlReader.readElementText());
                }

                // Is this a manga?
                else if (xmlReader.name() == QStringLiteral("Manga")) {
                    if (xmlReader.readElementText() == "Yes") {
                        acbfDocument->metaData()->bookInfo()->setGenre("manga");
                        acbfDocument->metaData()->bookInfo()->setRightToLeft(true);
                    }
                }
                // Content rating...
                else if (xmlReader.name() == QStringLiteral("AgeRating")) {
                    acbfDocument->metaData()->bookInfo()->addContentRating(xmlReader.readElementText());
                }

                // Authors...
                else if (xmlReader.name() == QStringLiteral("Writer")) {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Writer", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                } else if (xmlReader.name() == QStringLiteral("Plotter")) {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Writer", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                } else if (xmlReader.name() == QStringLiteral("Scripter")) {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Writer", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                } else if (xmlReader.name() == QStringLiteral("Penciller")) {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Penciller", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                } else if (xmlReader.name() == QStringLiteral("Inker")) {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Inker", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                } else if (xmlReader.name() == QStringLiteral("Colorist")) {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Colorist", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                } else if (xmlReader.name() == QStringLiteral("CoverArtist")) {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("CoverArtist", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                } else if (xmlReader.name() == QStringLiteral("Letterer")) {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Letterer", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                } else if (xmlReader.name() == QStringLiteral("Editor")) {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Editor", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                } else if (xmlReader.name() == QStringLiteral("Other")) {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addAuthor("Other", "", "", "", "", people.at(i).trimmed(), empty, empty);
                    }
                }
                // Characters...
                else if (xmlReader.name() == QStringLiteral("Characters")) {
                    QStringList people = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < people.size(); i++) {
                        acbfDocument->metaData()->bookInfo()->addCharacter(people.at(i).trimmed());
                    }
                }
                // Throw the rest into the keywords.
                else if (xmlReader.name() == QStringLiteral("Teams")) {
                    QStringList teams = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < teams.size(); i++) {
                        keywords.append(teams.at(i).trimmed());
                    }
                } else if (xmlReader.name() == QStringLiteral("Locations")) {
                    QStringList locations = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < locations.size(); i++) {
                        keywords.append(locations.at(i).trimmed());
                    }
                } else if (xmlReader.name() == QStringLiteral("StoryArc")) {
                    QStringList arc = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < arc.size(); i++) {
                        keywords.append(arc.at(i).trimmed());
                    }
                } else if (xmlReader.name() == QStringLiteral("SeriesGroup")) {
                    QStringList group = xmlReader.readElementText().split(",", Qt::SkipEmptyParts);
                    for (int i = 0; i < group.size(); i++) {
                        keywords.append(group.at(i).trimmed());
                    }
                }

                // Pages...
                else if (xmlReader.name() == QStringLiteral("Pages")) {
                    while (xmlReader.readNextStartElement()) {
                        if (xmlReader.name() == QStringLiteral("Page")) {
                            int index = xmlReader.attributes().value(QStringLiteral("Image")).toInt();
                            QString type = xmlReader.attributes().value(QStringLiteral("Type")).toString();
                            QString bookmark = xmlReader.attributes().value(QStringLiteral("Bookmark")).toString();
                            AdvancedComicBookFormat::Page *page = new AdvancedComicBookFormat::Page(acbfDocument);
                            page->setImageHref(entries.at(index));
                            if (type == QStringLiteral("FrontCover")) {
                                acbfDocument->metaData()->bookInfo()->setCoverpage(page);
                            } else {
                                if (bookmark.isEmpty()) {
                                    page->setTitle(type.append(QString::number(index)));
                                } else {
                                    page->setTitle(bookmark);
                                }
                                acbfDocument->body()->addPage(page, index - 1);
                            }
                            xmlReader.readNext();
                        }
                    }
                }

                else {
                    qCWarning(QTQUICK_LOG) << Q_FUNC_INFO << "currently unsupported subsection:" << xmlReader.name();
                    xmlReader.skipCurrentElement();
                }
            }

            if (!series.isEmpty() && number > -1) {
                acbfDocument->metaData()->bookInfo()->addSequence(number, series, volume);
            }
            if (!seriesAlt.isEmpty() && numberAlt > -1) {
                acbfDocument->metaData()->bookInfo()->addSequence(numberAlt, seriesAlt, volumeAlt);
            }

            if (year > 0 || month > 0 || day > 0) {
                // acbfDocument->metaData()->publishInfo()->setPublishDateFromInts(year, month, day);
            }

            if (publisher.size() > 0) {
                acbfDocument->metaData()->publishInfo()->setPublisher(publisher.join(", "));
            }

            if (keywords.size() > 0) {
                acbfDocument->metaData()->bookInfo()->setKeywords(keywords, "");
            }

            if (acbfDocument->metaData()->bookInfo()->languages().size() > 0) {
                QString lang = acbfDocument->metaData()->bookInfo()->languageEntryList().at(0);
                acbfDocument->metaData()->bookInfo()->setTitle(acbfDocument->metaData()->bookInfo()->title(""), lang);
                acbfDocument->metaData()->bookInfo()->setAnnotation(acbfDocument->metaData()->bookInfo()->annotation(""), lang);
                acbfDocument->metaData()->bookInfo()->setKeywords(acbfDocument->metaData()->bookInfo()->keywords(""), lang);
            }
        }
    }

    if (xmlReader.hasError()) {
        qCWarning(QTQUICK_LOG) << Q_FUNC_INFO << "Failed to read Comic Info XML document at token" << xmlReader.name() << "(" << xmlReader.lineNumber() << ":"
                               << xmlReader.columnNumber() << ") The reported error was:" << xmlReader.errorString();
    }
    qCDebug(QTQUICK_LOG) << Q_FUNC_INFO << "Completed ACBF document creation from ComicInfo.xml for" << acbfDocument->metaData()->bookInfo()->title();
    acbfData = acbfDocument;
    return !xmlReader.hasError();
}

bool ArchiveBookModel::loadCoMet(QStringList xmlDocuments, QObject *acbfData, QStringList entries, QString filename)
{
    KFileMetaData::UserMetaData filedata(filename);
    AdvancedComicBookFormat::Document *acbfDocument = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData);
    for (const QString &xmlDocument : std::as_const(xmlDocuments)) {
        QMutexLocker locker(&archiveMutex);
        const KArchiveFile *archFile = d->archive->directory()->file(xmlDocument);
        QXmlStreamReader xmlReader(archFile->data());
        if (xmlReader.readNextStartElement()) {
            if (xmlReader.name() == QStringLiteral("comet")) {
                // We'll need to collect several items to generate a series. Thankfully, comicinfo only has two types of series.
                QString series;
                int number = -1;
                int volume = 0;

                QStringList keywords;
                QStringList empty;

                while (xmlReader.readNextStartElement()) {
                    if (xmlReader.name() == QStringLiteral("title")) {
                        acbfDocument->metaData()->bookInfo()->setTitle(xmlReader.readElementText(), "");
                    }

                    // Summary/annotation.
                    else if (xmlReader.name() == QStringLiteral("description")) {
                        acbfDocument->metaData()->bookInfo()->setAnnotation(xmlReader.readElementText().split("\n\n"), "");
                    }

                    /*
                     * This ought to go into the kfile metadata.
                     */
                    // pages
                    else if (xmlReader.name() == QStringLiteral("pages")) {
                        filedata.setAttribute("Peruse.totalPages", xmlReader.readElementText());
                    }
                    // curentpage -- only read this when there's no such entry.
                    else if (xmlReader.name() == QStringLiteral("lastMark")) {
                        if (!filedata.hasAttribute("Peruse.currentPage")) {
                            filedata.setAttribute("Peruse.currentPage", xmlReader.readElementText());
                        } else {
                            xmlReader.skipCurrentElement();
                        }
                    }

                    // Series

                    else if (xmlReader.name() == QStringLiteral("series")) {
                        series = xmlReader.readElementText();
                    } else if (xmlReader.name() == QStringLiteral("issue")) {
                        number = xmlReader.readElementText().toInt();
                    } else if (xmlReader.name() == QStringLiteral("volume")) {
                        volume = xmlReader.readElementText().toInt();
                    }

                    // Publishing date.

                    else if (xmlReader.name() == QStringLiteral("date")) {
                        acbfDocument->metaData()->publishInfo()->setPublishDate(QDate::fromString(xmlReader.readElementText(), Qt::ISODate));
                    }

                    // Publisher

                    else if (xmlReader.name() == QStringLiteral("publisher")) {
                        acbfDocument->metaData()->publishInfo()->setPublisher(xmlReader.readElementText());
                    } else if (xmlReader.name() == QStringLiteral("rights")) {
                        acbfDocument->metaData()->publishInfo()->setLicense(xmlReader.readElementText());
                    } else if (xmlReader.name() == QStringLiteral("identifier")) {
                        acbfDocument->metaData()->publishInfo()->setIsbn(xmlReader.readElementText());
                    }

                    // Genre
                    else if (xmlReader.name() == QStringLiteral("genre")) {
                        QString key = xmlReader.readElementText();
                        QString genreKey = key.toLower().replace(" ", "_");
                        if (acbfDocument->metaData()->bookInfo()->availableGenres().contains(genreKey)) {
                            acbfDocument->metaData()->bookInfo()->setGenre(genreKey);
                        } else {
                            keywords.append(key);
                        }
                    }

                    // Language

                    else if (xmlReader.name() == QStringLiteral("language")) {
                        acbfDocument->metaData()->bookInfo()->addLanguage(xmlReader.readElementText());
                    }

                    // Sources/Weblink
                    else if (xmlReader.name() == QStringLiteral("isVersionOf")) {
                        acbfDocument->metaData()->documentInfo()->setSource(QStringList(xmlReader.readElementText()));
                    }

                    // One short, trade, etc.
                    else if (xmlReader.name() == QStringLiteral("format")) {
                        keywords.append(xmlReader.readElementText());
                    }

                    // Is this a manga?
                    else if (xmlReader.name() == QStringLiteral("readingDirection")) {
                        if (xmlReader.readElementText() == "rtl") {
                            acbfDocument->metaData()->bookInfo()->setRightToLeft(true);
                        }
                    }
                    // Content rating...
                    else if (xmlReader.name() == QStringLiteral("rating")) {
                        acbfDocument->metaData()->bookInfo()->addContentRating(xmlReader.readElementText());
                    }

                    // Authors...
                    else if (xmlReader.name() == QStringLiteral("writer")) {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Writer", "", "", "", "", person, empty, empty);
                    } else if (xmlReader.name() == QStringLiteral("creator")) {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Writer", "", "", "", "", person, empty, empty);
                    } else if (xmlReader.name() == QStringLiteral("penciller")) {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Penciller", "", "", "", "", person, empty, empty);
                    } else if (xmlReader.name() == QStringLiteral("editor")) {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Editor", "", "", "", "", person, empty, empty);
                    } else if (xmlReader.name() == QStringLiteral("coverDesigner")) {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("CoverArtist", "", "", "", "", person, empty, empty);
                    } else if (xmlReader.name() == QStringLiteral("letterer")) {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Letterer", "", "", "", "", person, empty, empty);
                    } else if (xmlReader.name() == QStringLiteral("inker")) {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Inker", "", "", "", "", person, empty, empty);
                    } else if (xmlReader.name() == QStringLiteral("colorist")) {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addAuthor("Colorist", "", "", "", "", person, empty, empty);
                    }

                    // Characters
                    else if (xmlReader.name() == QStringLiteral("character")) {
                        QString person = xmlReader.readElementText();
                        acbfDocument->metaData()->bookInfo()->addCharacter(person);
                    }

                    // Get the cover image, set it, remove it from entries, then remove all other entries.
                    else if (xmlReader.name() == QStringLiteral("coverImage")) {
                        QString url = xmlReader.readElementText();
                        AdvancedComicBookFormat::Page *cover = new AdvancedComicBookFormat::Page(acbfDocument);
                        cover->setImageHref(url);
                        acbfDocument->metaData()->bookInfo()->setCoverpage(cover);
                        entries.removeAll(url);
                        for (const QString &entry : std::as_const(entries)) {
                            AdvancedComicBookFormat::Page *page = new AdvancedComicBookFormat::Page(acbfDocument);
                            page->setImageHref(entry);
                            acbfDocument->body()->addPage(page);
                        }
                        xmlReader.readNext();
                    }

                    else {
                        qCWarning(QTQUICK_LOG) << Q_FUNC_INFO << "currently unsupported subsection:" << xmlReader.name();
                        xmlReader.skipCurrentElement();
                    }
                }

                if (!series.isEmpty() && number > -1) {
                    acbfDocument->metaData()->bookInfo()->addSequence(number, series, volume);
                }

                if (acbfDocument->metaData()->bookInfo()->genres().size() == 0) {
                    // There must always be a genre in a proper acbf file...
                    acbfDocument->metaData()->bookInfo()->setGenre("other");
                }

                if (keywords.size() > 0) {
                    acbfDocument->metaData()->bookInfo()->setKeywords(keywords, "");
                }

                if (acbfDocument->metaData()->bookInfo()->languages().size() > 0) {
                    QString lang = acbfDocument->metaData()->bookInfo()->languageEntryList().at(0);
                    acbfDocument->metaData()->bookInfo()->setTitle(acbfDocument->metaData()->bookInfo()->title(""), lang);
                    acbfDocument->metaData()->bookInfo()->setAnnotation(acbfDocument->metaData()->bookInfo()->annotation(""), lang);
                    acbfDocument->metaData()->bookInfo()->setKeywords(acbfDocument->metaData()->bookInfo()->keywords(""), lang);
                }
            }
            if (xmlReader.hasError()) {
                qCWarning(QTQUICK_LOG) << Q_FUNC_INFO << "Failed to read CoMet document at token" << xmlReader.name() << "(" << xmlReader.lineNumber() << ":"
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

QString ArchiveBookModel::previewForId(const QString &id) const
{
    static const QString directorySplit{"/"};
    static const QString period{"."};
    static const QString acbfSuffix{"acbf"};
    if (d->archive) {
        if (id.split(directorySplit).last().contains(period)) {
            const QString suffix = id.split(period).last().toLower();
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

QString ArchiveBookModel::fontFamilyName(const QString &fontFileName)
{
    QString familyName;
    if (!fontFileName.isEmpty()) {
        if (d->fontIdByFilename.contains(fontFileName)) {
            familyName = QFontDatabase::applicationFontFamilies(d->fontIdByFilename.value(fontFileName)).first();
        } else {
            AdvancedComicBookFormat::Document *acbf = qobject_cast<AdvancedComicBookFormat::Document *>(acbfData());
            if (acbf) {
                AdvancedComicBookFormat::Binary *binary = qobject_cast<AdvancedComicBookFormat::Binary *>(acbf->objectByID(fontFileName));
                if (binary) {
                    int id = QFontDatabase::addApplicationFontFromData(binary->data());
                    if (id > -1) {
                        d->fontIdByFilename[fontFileName] = id;
                        familyName = QFontDatabase::applicationFontFamilies(d->fontIdByFilename.value(fontFileName)).first();
                    }
                }
            }
            if (familyName.isEmpty()) {
                QString foundEntry;
                // If there's more files by the same name, just assume it's the first one in a DFS, because it won't be sensibly deducible anyway
                for (const QString &entry : d->fileEntries) {
                    if (entry.endsWith(fontFileName)) {
                        foundEntry = entry;
                        break;
                    }
                }
                auto file = archiveFile(foundEntry);
                if (file) {
                    int id = QFontDatabase::addApplicationFontFromData(file->data());
                    if (id > -1) {
                        d->fontIdByFilename[fontFileName] = id;
                        familyName = QFontDatabase::applicationFontFamilies(d->fontIdByFilename.value(fontFileName)).first();
                    }
                }
            }
        }
    }
    return familyName;
}

QString ArchiveBookModel::firstAvailableFont(const QStringList &fontList)
{
    QString availableFont;
    for (const QString &fontName : fontList) {
        QString actualName = fontName;
        if (fontName.toLower().endsWith("ttf") || fontName.toLower().endsWith("ttc")) {
            actualName = fontFamilyName(fontName);
        }
        if (actualName.isEmpty()) {
            if (d->fontDatabase.hasFamily(fontName)) {
                actualName = fontName;
            }
        }
        if (!actualName.isEmpty()) {
            availableFont = actualName;
            break;
        }
    }
    return availableFont;
}
