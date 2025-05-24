#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QQmlContext>
#include <Windows.h>

#include "LogSearcher.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/image/logo.ico"));

    app.setApplicationName("LogSearcher");
    app.setOrganizationName("LogSearcher_Organizatio");

    QQmlApplicationEngine engine;

    LogSearcher* searcher = new LogSearcher;
    engine.rootContext()->setContextProperty("$LogSearcher", searcher);

    const QUrl url(u"qrc:/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    QList<QObject*> objectList = engine.rootObjects();
    if(objectList.size() > 0)
    {
        QObject* rootObject = objectList[0];
        if(rootObject)
        {
            QWindow *window = qobject_cast<QWindow *>(rootObject);
            if(window)
            {
                searcher->setWId(window->winId());
            }
        }
    }

    return app.exec();
}
