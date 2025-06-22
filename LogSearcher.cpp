#include "LogSearcher.h"
#include "LogAnalysis/LogUtils.h"
#include <windows.h>

LogSearcher::LogSearcher(QObject *parent /*= nullptr*/)
    : QObject{parent}
{

    // 设置合适的线程数
    QThreadPool::globalInstance()->setMaxThreadCount(QThread::idealThreadCount());
}

LogSearcher::~LogSearcher()
{
}

void LogSearcher::setWId(WId winid)
{
    m_winId = winid;

    //    ::SetWindowPos((HWND)(/*this->winId()*/m_winId),
    //                   HWND_TOPMOST, 0, 0, 0, 0,
    //                   SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void LogSearcher::setSearchModel(LogModel* model1, LogModel* model2, LogModel* model3)
{
    m_logModel = model1;
    m_resultModel = model2;
    m_findModel = model3;
}

void LogSearcher::init()
{
    QString config = {};
    if(!QFileInfo::exists(m_settingsPath))
    {
        const QString keyword = "__Default__";
        QSettings settings(m_settingsPath, QSettings::Format::IniFormat);
        settings.setValue("Global/Keywords", keyword);
        settings.sync();

        config.push_back(keyword + ";");
    }
    else
    {
        QSettings settings(m_settingsPath, QSettings::Format::IniFormat);
        config = settings.value("Global/Keywords").toString();
    }

    QStringList keywords = config.split(";");
    keywords.removeAll("");

    for(int i = 0; i < keywords.size(); ++i)
    {
        insertKeyword(-1, keywords[i], LogUtils::GenerateRandomColorRGB_Safe().name());
    }
}

void LogSearcher::insertKeyword(const int index, const QString& keyword, const QString& color)
{
    QString finalColor = color;
    if(finalColor.isEmpty())
    {
        finalColor = LogUtils::GenerateRandomColorRGB_Safe().name();
    }

    if(index < 0)
    {
        // 添加
        LogUtils::Keywords()[LogUtils::Keywords().size()] = { keyword, finalColor };
        emit addKeywordFinish(keyword.isEmpty() ? "     " : keyword, finalColor);
    }
    else
    {
        // 修改
        LogUtils::Keywords()[index] = { keyword, finalColor };
    }

    LogUtils::FormatedKeywordMap();

    refreshSettings__();
}

void LogSearcher::removeKeyword(const int index)
{
    LogUtils::Keywords().erase(std::next(LogUtils::Keywords().begin(), index));

    LogUtils::FormatedKeywordMap();

    refreshSettings__();

    emit removeKeywordFinish(index);
}

void LogSearcher::openLog(const QString& filePath, const bool repeatOpen /*= false*/)
{
    qDebug() << "LogSearcher::openLog >>>" << filePath;

    // 非重载日志的话，更新聚焦的日志
    if(!repeatOpen)
    {
        m_focusedLog = filePath;

        // remove "file:///"
        const QString prefix = "file:///";
        if(m_focusedLog.contains(prefix))
        {
            m_focusedLog = m_focusedLog.mid(prefix.size());
        }
    }

    // 开始监视日志
    m_fileSystemWatcher.disconnect();
    m_fileSystemWatcher.removePaths(m_fileSystemWatcher.files());
    m_fileSystemWatcher.addPath(m_focusedLog);
    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &path) {
        emit logContentModified();
    });

    // 创建并启动工作线程
    m_thread = new QThread(this);
    m_logLoaderThread = new LogLoaderThread;
    m_logLoaderThread->moveToThread(m_thread);
    QObject::connect(m_thread, &QThread::started, m_logLoaderThread, &LogLoaderThread::analyze);
    QObject::connect(m_logLoaderThread,
                     &LogLoaderThread::lineNumWidth,
                     this,
                     [&](int width) {
        emit lineNumWidth(width);
    });
    QObject::connect(m_logLoaderThread,
                     &LogLoaderThread::newLogAvailable,
                     this,
                     [&](const bool containKeyword,
                     const int lineIndex,
                     const QString log) {
        m_logModel->appendLog(-1, log);
        if(containKeyword)
        {
            m_resultModel->appendLog(lineIndex, log);
        }
    });
    QObject::connect(m_logLoaderThread, &LogLoaderThread::loadFinish, this, [&](){ emit loadFinish(m_focusedLog); });
    //QObject::connect(m_thread, &QThread::finished, m_logLoaderThread, &QObject::deleteLater);
    //QObject::connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);

    // 设置查询属性
    m_logLoaderThread->setTargetLog(m_focusedLog);
    m_logLoaderThread->setTargetKeywordAndColor(LogUtils::FormatedKeywordMap());

    // 清空上一次结果
    m_logModel->clearAll();
    m_resultModel->clearAll();
    m_findModel->clearAll();

    // 开始查询
    emit loadStart();
    m_thread->start();
}

void LogSearcher::openLatestIndexLog(const int latestIndex)
{
    if(m_focusedLog.isEmpty())
        return;

    QFileInfo fi(m_focusedLog);
    QDir dir(fi.absoluteDir());
    dir.setFilter(QDir::Files);
    QFileInfoList fileInfoList = dir.entryInfoList();

    const QString prefix = fi.baseName().left(5);
    QMap<QDateTime, QString> filePathMap;
    for (const QFileInfo &fileInfo : fileInfoList)
    {
        if (fileInfo.fileName().startsWith(prefix, Qt::CaseInsensitive))
        {
            filePathMap[fileInfo.lastModified()] = fileInfo.filePath();
        }
    }

    if(filePathMap.size() > latestIndex)
    {
        const auto iter = std::next(filePathMap.begin(), filePathMap.size() - 1 - latestIndex);
        openLog(iter.value());
    }
}

void LogSearcher::find(const QString& targetKeyword)
{
    if(targetKeyword.isEmpty())
        return;

    m_elapsedTimer.start();

    watcher.disconnect();
    QObject::connect(&watcher, &QFutureWatcher<LogSearcher::LineNumber_Line_Pair>::finished, &watcher, [this, targetKeyword](){
        QVector<int> lineNumbers = {};
        QVector<QString> logs{};

        const auto& results = watcher.future().results();
#pragma omp parallel for
        for(int i = 0; i < results.size(); ++i)
        {
            if(!results[i].second.isEmpty())
            {
                QString dstLine;
                LogUtils::ConvertHTML(results[i].second, targetKeyword, dstLine);

#pragma omp ordered
                {
                    lineNumbers.push_back(results[i].first);
                    logs.push_back(dstLine);
                }
            }
        }

        m_findModel->clearAll();
        m_findModel->appendLog(lineNumbers, logs);

        emit findFinish(targetKeyword, m_findModel->rowCount(), m_elapsedTimer.elapsed());
    });

    // 启动并行搜索
    const QVector<QPair<int, QString>> lines = m_logLoaderThread->getAllLines();
    QFuture<LogSearcher::LineNumber_Line_Pair> future = QtConcurrent::mapped(
                lines,
                [targetKeyword, this](const LineNumber_Line_Pair& line) { return find__(line, targetKeyword); }
    );

    watcher.setFuture(future);
}

LogSearcher::LineNumber_Line_Pair LogSearcher::find__(const LineNumber_Line_Pair& line, const QString &keyword)
{
    if (line.second.contains(keyword))
    {
        return line;
    }

    return LineNumber_Line_Pair{};
}

void LogSearcher::refreshSettings__()
{
    std::async(std::launch::async, [&](){
        QString keywords = "";
        const QMap<int, QPair<QString, QString>>& keywordMap = LogUtils::Keywords();
        for(auto iter = keywordMap.begin(); iter != keywordMap.end(); ++iter)
        {
            if(!iter.value().first.isEmpty())
            {
                keywords += iter.value().first + ";";
            }
        }

        QSettings settings(m_settingsPath, QSettings::Format::IniFormat);
        settings.setValue("Global/Keywords", keywords);
        settings.sync();
    });
}
