#include "peruse_helpers.h"
#include <QQmlEngine>
#include <QString>
#include <QQmlComponent>
#include <QQmlContext>
#include <QStandardPaths>
#include <QApplication>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QDir>

#include <KDeclarative/KDeclarative>
#include <KAboutData>

#include "peruse_helpers.h"

namespace PeruseHelpers {
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

int init(QString &path, QApplication& app, const QString &filename) {
    KDeclarative::KDeclarative kdeclarative;
    QQmlEngine engine;
    kdeclarative.setDeclarativeEngine(&engine);
    kdeclarative.setupEngine(&engine);
    kdeclarative.setupContext();

    bool osIsWindows = false;
#ifdef Q_OS_WIN
    // Because windows is a bit funny with paths and whatnot, just so the thing with the lib paths...
    QDir appdir(qApp->applicationDirPath());
    appdir.cdUp();
    qApp->addLibraryPath(appdir.canonicalPath() + "/lib");
    engine.addImportPath(appdir.canonicalPath() + "/lib/qml");
    engine.addImportPath(appdir.canonicalPath() + "/qml");
    osIsWindows = true;
    // Hey, let's try and avoid all those extra stale processes, right?
    qputenv("KDE_FORK_SLAVES", "true");
#endif

    engine.rootContext()->setContextProperty("osIsWindows", osIsWindows);

    engine.rootContext()->setContextProperty("fileToOpen", filename);

    QQmlContext* objectContext = engine.rootContext();
    QString platformEnv(qgetenv("PLASMA_PLATFORM"));
    engine.rootContext()->setContextProperty("PLASMA_PLATFORM", platformEnv);

    // Yes, i realise this is a touch on the ugly side. I have found no better way to allow for
    // things like the archive book model to create imageproviders for the archives
    engine.rootContext()->setContextProperty("globalQmlEngine", &engine);
    engine.rootContext()->setContextProperty("maxTextureSize", getMaxTextureSize());
    engine.rootContext()->setContextProperty(QStringLiteral("peruseAboutData"), QVariant::fromValue(KAboutData::applicationData()));

    QQmlComponent component(&engine, path);
    if (component.isError()) {
        qCritical() << "Failed to load the component from disk. Reported error was:" << component.errorString();
        return -1;
    }
    if (component.status() != QQmlComponent::Ready) {
        qCritical() << "Failed to make the Qt Quick component ready. Status is:" << component.status();
        return -3;
    }

    QObject* obj = component.create(objectContext);
    if (!obj) {
        qCritical() << "Failed to create an object from our component";
        return -2;
    }

    return app.exec();
}
}
