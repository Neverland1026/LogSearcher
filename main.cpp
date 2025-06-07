#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QQmlContext>
#include <Windows.h>

#include "LogSearcher.h"

#include <QColor>
QColor randomColorRGB_Safe__()
{
    QColor color;
    do {
        color = QColor(rand() % 256, rand() % 256, rand() % 256);
    } while (color.red() > 230 && color.green() > 230 && color.blue() > 230);

    return color;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/image/logo.ico"));

    app.setApplicationName("LogSearcher");
    app.setOrganizationName("LogSearcher_Organizatio");

    QQmlApplicationEngine engine;

    LogSearcher* searcher = new LogSearcher;
    engine.rootContext()->setContextProperty("$LogSearcher", searcher);

    LogModel* logModel1 = new LogModel();
    engine.rootContext()->setContextProperty("$LogModel1", logModel1);

    LogModel* logModel2 = new LogModel();
    engine.rootContext()->setContextProperty("$LogModel2", logModel2);

    searcher->setLogModel(logModel1, logModel2);

    for(int i = 0; i < 999; ++ i)
    {
        logModel1->appendLog(QString("<font color='%1'>QObject::connect(engine,QQmlApplicationEngine::objectCreated</font>").arg("#FFFFFF"));
    }

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
