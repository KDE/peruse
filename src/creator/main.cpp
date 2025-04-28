/*
 * Copyright (C) 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#include <KAboutData>
#include <KLocalizedQmlContext>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QIcon>
#include <QQmlApplicationEngine>

#include "../../config-peruse.h"
#include "peruse_helpers.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("peruse-creator")));
    app.setApplicationDisplayName("Peruse Creator");
    app.setOrganizationDomain("kde.org");

    KLocalizedString::setApplicationDomain("peruse");
    KAboutData about(QStringLiteral("perusecreator"),
                     i18n("Peruse Creator"),
                     PERUSE_VERSION_STRING,
                     i18n("Comic Book Creator by KDE"),
                     KAboutLicense::GPL,
                     i18n("© 2016-2021 KDE"));
    about.addAuthor(i18n("Dan Leinir Turthra Jensen"),
                    QString("Maintainer and Lead Developer"),
                    QStringLiteral("admin@leinir.dk"),
                    QStringLiteral("https://leinir.dk/"),
                    QStringLiteral("leinir"));
    about.setProductName("peruse/perusecreator");
    about.setProgramLogo(app.windowIcon());
    KAboutData::setApplicationData(about);

    QCommandLineParser parser;
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

    QQmlApplicationEngine engine;
    KLocalization::setupLocalizedContext(&engine);
    engine.loadFromModule("org.kde.peruse.creator", "Main");
    return app.exec();
}
