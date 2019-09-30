/*
 * Copyright (C) 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <QStandardPaths>
#include <QDir>

#include <KAboutData>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QIcon>

#include <iostream>

#include "peruse_helpers.h"

#include <app_debug.h>

Q_DECL_EXPORT
int main(int argc, char** argv)
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("peruse")));
    app.setApplicationDisplayName("Peruse");
    app.setOrganizationDomain("kde.org");

    KLocalizedString::setApplicationDomain("peruse");
    KAboutData about(QStringLiteral("peruse"), i18n("Peruse Reader"), "2.0", i18n("Comic Book Reader by KDE"),
                     KAboutLicense::GPL, i18n("© 2016-2019 KDE"));
    about.addAuthor(i18n("Dan Leinir Turthra Jensen"), QString("Maintainer and Lead Developer"), QStringLiteral("admin@leinir.dk"), QStringLiteral("https://leinir.dk/"), QStringLiteral("leinir"));
    about.setProductName("peruse/peruse");
    about.setProgramLogo(app.windowIcon());
    KAboutData::setApplicationData(about);

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringLiteral("clear-db"), i18n("Clear the metainfo cache and perform a full rescan.")));
    parser.addPositionalArgument(QStringLiteral("file"), i18n("Open file in peruse."));
    parser.addHelpOption();
    parser.process(app);


    QString filename;
    if (parser.positionalArguments().size() > 0) {
        filename = parser.positionalArguments().at(0);
    }

    if (parser.positionalArguments().size() > 1) {
        parser.showHelp(1);
    }

    if (parser.isSet(QStringLiteral("clear-db"))) {
        QString dbfile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/library.sqlite";
        if(QFile::exists(dbfile)) {
            qCDebug(APP_LOG) << "Remove database at" << dbfile;
            QFile::remove(dbfile);
        }
    }
    QString platformEnv(qgetenv("PLASMA_PLATFORM"));
    QString path = platformEnv.startsWith("phone")
        ? QStringLiteral("qrc:///qml/MobileMain.qml")
        : QStringLiteral("qrc:///qml/Main.qml");

    return PeruseHelpers::init(path, app, filename);
}
