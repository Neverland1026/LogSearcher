#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QQmlContext>
#include <Windows.h>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>

#include "LogSearcher.h"

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString text;
//    switch (type) {
//    case QtDebugMsg:
//        text = QString("Debug: %1").arg(msg);
//        break;
//    case QtInfoMsg:
//        text = QString("Info: %1").arg(msg);
//        break;
//    case QtWarningMsg:
//        text = QString("Warning: %1 in file %2:%3, line %4, function %5")
//                   .arg(msg)
//                   .arg(context.file())
//                   .arg(context.line())
//                   .arg(context.function());
//        break;
//    case QtCriticalMsg:
//        text = QString("Critical: %1 in file %2:%3, line %4, function %5")
//                   .arg(msg)
//                   .arg(context.file())
//                   .arg(context.line())
//                   .arg(context.function());
//        break;
//    case QtFatalMsg:
//        text = QString("Fatal: %1 in file %2:%3, line %4, function %5")
//                   .arg(msg)
//                   .arg(context.file())
//                   .arg(context.line())
//                   .arg(context.function());
//        abort(); // 或者你可以选择其他方式来处理致命错误
//    }

    text = QString("Info: %1").arg(msg);

    QFile outFile("log.txt");
    outFile.open(QIODevice::Append | QIODevice::Text);
    QTextStream ts(&outFile);
    ts << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << " - " << text << "\n";
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/image/logo.ico"));

    app.setApplicationName("LogSearcher");
    app.setOrganizationName("LogSearcher_Organizatio");

    qInstallMessageHandler(myMessageHandler);

    QQmlApplicationEngine engine;

    LogSearcher* searcher = new LogSearcher;
    engine.rootContext()->setContextProperty("$LogSearcher", searcher);

    LogModel* logModel = new LogModel();
    engine.rootContext()->setContextProperty("$LogModel", logModel);

    LogModel* resultModel = new LogModel();
    engine.rootContext()->setContextProperty("$ResultModel", resultModel);

    searcher->setSearchModel(logModel, resultModel);

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
