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

#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>

#include <QStandardPaths>
#include <QDebug>
#include <QThread>

#include <KDBusService>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <iostream>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationDisplayName("Peruse");

    KDBusService service(KDBusService::Unique);

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption("reset", i18n("Reset the database")));
    parser.addHelpOption();
    parser.process(app);

    if (parser.positionalArguments().size() > 1) {
        parser.showHelp(1);
    }

    if (parser.isSet("reset")) {
//         KokoConfig config;
//         config.reset();

//         ImageStorage::reset();
    }

    QThread trackerThread;

    QStringList locations = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    locations << QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
    Q_ASSERT(locations.size() >= 1);
//     qDebug() << locations;
//     FileSystemTracker tracker;
//     tracker.setFolder(locations.first());
//     tracker.moveToThread(&trackerThread);

//     Koko::Processor processor;
//     QObject::connect(&tracker, &FileSystemTracker::imageAdded, &processor, &Koko::Processor::addFile);
//     QObject::connect(&tracker, &FileSystemTracker::imageRemoved, &processor, &Koko::Processor::removeFile);
//     QObject::connect(&tracker, &FileSystemTracker::initialScanComplete, &processor, &Koko::Processor::initialScanCompleted);
// 
//     trackerThread.start();
//     tracker.init();
// 
//     KokoConfig config;

    QQmlEngine engine;
    QQmlContext* objectContext = engine.rootContext();
    QString platformEnv(qgetenv("PLASMA_PLATFORM"));
    engine.rootContext()->setContextProperty("PLASMA_PLATFORM", platformEnv);
    engine.rootContext()->setContextProperty("bookLocations", locations);
    // Yes, i realise this is a touch on the ugly side. I have found no better way to allow for
    // things like the archive book model to create imageproviders for the archives
    engine.rootContext()->setContextProperty("globalQmlEngine", &engine);

    QString path;
//     if (platformEnv.startsWith("phone")) {
    // Once we've got a functional desktop version, restore this check. Right now it doesn't make a lot of sense.
    if(true) {
        path = QStandardPaths::locate(QStandardPaths::DataLocation, "qml/MobileMain.qml");
    } else {
        path = QStandardPaths::locate(QStandardPaths::DataLocation, "qml/Main.qml");
    }
    QQmlComponent component(&engine, path);
    if (component.isError()) {
        std::cout << component.errorString().toUtf8().constData() << std::endl;
        Q_ASSERT(0);
    }
    Q_ASSERT(component.status() == QQmlComponent::Ready);

    QObject* obj = component.create(objectContext);
    Q_ASSERT(obj);

    int rt = app.exec();
//     trackerThread.quit();
    return rt;
}
