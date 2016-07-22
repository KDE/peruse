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

#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>

#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QThread>

#include <KDeclarative/KDeclarative>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <iostream>

#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QOpenGLFunctions>

int getMaxTextureSize()
{
    int maxSize = 0;

    // Create a temp context - required if this is called from another thread
    QOpenGLContext ctx;
    if ( !ctx.create() )
    {
        // TODO handle the error
        qDebug() << "No OpenGL context could be created, this is clearly bad...";
        exit(-1);
    }

    // rather than using a QWindow - which actually dosen't seem to work in this case either!
    QOffscreenSurface surface;
    surface.setFormat( ctx.format() );
    surface.create();

    ctx.makeCurrent(&surface);

    // Now the call works
    QOpenGLFunctions glFuncs(QOpenGLContext::currentContext());
    glFuncs.glEnable(GL_TEXTURE_2D);
    glFuncs.glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

    return maxSize;
}


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationDisplayName("Peruse Creator");

    QCommandLineParser parser;
    // TODO file option for opening comics by passing them through on the command line
    parser.addHelpOption();
    parser.process(app);

    if (parser.positionalArguments().size() > 1) {
        parser.showHelp(1);
    }

    KDeclarative::KDeclarative kdeclarative;
    QQmlEngine engine;
    kdeclarative.setDeclarativeEngine(&engine);
    kdeclarative.setupBindings();

    bool osIsWindows = false;
#ifdef Q_OS_WIN
    // Because windows is a bit funny with paths and whatnot, just so the thing with the lib paths...
    QDir appdir(qApp->applicationDirPath());
    appdir.cdUp();
    engine.addImportPath(appdir.canonicalPath() + "/lib/qml");
    osIsWindows = true;
    // Hey, let's try and avoid all those extra stale processes, right?
    qputenv("KDE_FORK_SLAVES", "true");
#endif
    engine.rootContext()->setContextProperty("osIsWindows", osIsWindows);

    QQmlContext* objectContext = engine.rootContext();
    QString platformEnv(qgetenv("PLASMA_PLATFORM"));
    engine.rootContext()->setContextProperty("PLASMA_PLATFORM", platformEnv);
    // Yes, i realise this is a touch on the ugly side. I have found no better way to allow for
    // things like the archive book model to create imageproviders for the archives
    engine.rootContext()->setContextProperty("globalQmlEngine", &engine);
    engine.rootContext()->setContextProperty("maxTextureSize", getMaxTextureSize());

    QString path= QStandardPaths::locate(QStandardPaths::DataLocation, "qml/Main.qml");
    int rt = 0;
    QQmlComponent component(&engine, path);
    if (component.isError())
    {
        qCritical() << "Failed to load the component from disk. Reported error was:" << component.errorString();
        rt = -1;
    }
    else
    {
        if(component.status() == QQmlComponent::Ready)
        {
            QObject* obj = component.create(objectContext);
            if(obj)
            {
                rt = app.exec();
            }
            else
            {
                qCritical() << "Failed to create an object from our component";
                rt = -2;
            }
        }
        else
        {
            qCritical() << "Failed to make the Qt Quick component ready. Status is:" << component.status();
            rt = -3;
        }
    }

    return rt;
}
