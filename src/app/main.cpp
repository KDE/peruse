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
#include <QDebug>

#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <iostream>

#include "peruse_helpers.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationDisplayName("Peruse");
    app.setOrganizationDomain("kde.org");

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringLiteral("clear-db"), i18n("Clear the metainfo cache and perform a full rescan.")));
    // TODO file option for opening comics by passing them through on the command line
    parser.addHelpOption();
    parser.process(app);

    if (parser.positionalArguments().size() > 1) {
        parser.showHelp(1);
    }

    if (parser.isSet(QStringLiteral("clear-db"))) {
        QString dbfile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/library.sqlite";
        if(QFile::exists(dbfile)) {
            qDebug() << "Remove database at" << dbfile;
            QFile::remove(dbfile);
        }
    }
    QString platformEnv(qgetenv("PLASMA_PLATFORM"));
    QString path = QStandardPaths::locate(QStandardPaths::AppDataLocation,
        platformEnv.startsWith("phone") ? "qml/MobileMain.qml" : "qml/Main.qml");

    return PeruseHelpers::init(path, app);
}
