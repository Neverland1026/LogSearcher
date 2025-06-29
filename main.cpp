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
    static QString logFileName = "./log/LogSearcher_%1.log";
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        const QString TimeStamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
        logFileName = logFileName.arg(TimeStamp);
        qDebug() << "Create log [" << logFileName << "]";

        // 保留最近10次日志
        const int maxFiles = 10;
        QDir logDir("./log/");
        QStringList nameFilters;
        nameFilters << "*.log";
        QFileInfoList logFiles = logDir.entryInfoList(nameFilters, QDir::Files, QDir::Time | QDir::Reversed);
        while (logFiles.size() > maxFiles - 1) {
            QFile::remove(logFiles.first().absoluteFilePath());
            logFiles.removeFirst();
        }
    });

    QFile outFile(logFileName);
    outFile.open(QIODevice::Append | QIODevice::Text);
    QTextStream ts(&outFile);
    ts << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << "  " << msg << "\n";
    outFile.close();
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/image/logo.ico"));

    app.setApplicationName("LogSearcher");
    app.setOrganizationName("LogSearcher_Organizatio");

    QDir dir;
    dir.mkpath("./log");

    qInstallMessageHandler(myMessageHandler);

    qDebug() << "LogSearcher built on:" << __DATE__ << ", at" << __TIME__;

    QQmlApplicationEngine engine;

    LogSearcher* searcher = new LogSearcher;
    engine.rootContext()->setContextProperty("$LogSearcher", searcher);

    LogModel* logModel = new LogModel();
    engine.rootContext()->setContextProperty("$LogModel", logModel);

    LogModel* summaryModel = new LogModel();
    engine.rootContext()->setContextProperty("$SummaryModel", summaryModel);

    LogModel* findModel = new LogModel();
    engine.rootContext()->setContextProperty("$FindModel", findModel);

    searcher->setSearchModel(logModel, summaryModel, findModel);

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

    if(argc > 1)
    {
        searcher->init();
        QTimer::singleShot(100, [&](){
            searcher->openLog(QDir::fromNativeSeparators(QString::fromLocal8Bit(argv[1])));
        });
    }

    return app.exec();
}
