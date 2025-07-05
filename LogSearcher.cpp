#include "LogSearcher.h"
#include "LogAnalysis/LogUtils.h"
#include <windows.h>
#include <iomanip>

#define OPERATE_BEGIN m_thread = new QThread(this);  \
m_logLoaderThread = new LogLoaderThread;             \
    m_logLoaderThread->moveToThread(m_thread);

#define OPERATE_END m_thread->start();

#define OPERATE_DELETE { /*m_thread->quit(); m_thread->deleteLater(); m_logLoaderThread->deleteLater();*/ };


LogSearcher::LogSearcher(QObject *parent /*= nullptr*/)
    : QObject{parent}
{
    m_rootPath = QCoreApplication::applicationDirPath();

    // 设置合适的线程数
    QThreadPool::globalInstance()->setMaxThreadCount(QThread::idealThreadCount());
}

LogSearcher::~LogSearcher()
{
}

void LogSearcher::setWId(WId winid)
{
    m_winId = winid;
    //toggleTOPMOST();
}

void LogSearcher::setSearchModel(LogModel* model1, LogModel* model2, LogModel* model3)
{
    m_logModel = model1;
    m_summaryModel = model2;
    m_findModel = model3;
}

void LogSearcher::init()
{
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        QString config = {};
        if(!QFileInfo::exists(getSettingsAbsolutePath__()))
        {
            const QString keyword = "__TIME_COST_STATISTICS__";
            QSettings settings(getSettingsAbsolutePath__(), QSettings::Format::IniFormat);
            settings.setValue("Global/Keywords", keyword);
            settings.sync();

            config.push_back(keyword + ";");
        }
        else
        {
            QSettings settings(getSettingsAbsolutePath__(), QSettings::Format::IniFormat);
            config = settings.value("Global/Keywords").toString();
        }

        QStringList keywords = config.split(";");
        keywords.removeAll("");

        for(int i = 0; i < keywords.size(); ++i)
        {
            insertKeyword(-1, keywords[i], LogUtils::GenerateRandomColorRGB_Safe().name());
        }
    });
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

    OPERATE_BEGIN;
    QObject::connect(m_thread, &QThread::started, m_logLoaderThread, [this, index]() { m_logLoaderThread->remove(index); });
    QObject::connect(m_logLoaderThread, &LogLoaderThread::removeSingleLine, this, [&](const int lineIndex, const int summaryLineIndex, const QString log) {
        m_logModel->updateRow(lineIndex, log);
        m_summaryModel->removeRow(summaryLineIndex);
    });
    QObject::connect(m_logLoaderThread, &LogLoaderThread::operateFinish, this, [this, index]() { emit removeKeywordFinish(index); OPERATE_DELETE; });
    OPERATE_END;
}

void LogSearcher::openLog(const QString& filePath, const bool repeatOpen /*= false*/)
{
    qDebug() << std::boolalpha << "LogSearcher::openLog [ filePath repeatOpen ] = [" << filePath  << repeatOpen << "]";

    emit loadStart();

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

    // 清空上一次结果
    m_logModel->clearAll();
    m_summaryModel->clearAll();
    m_findModel->clearAll();

    // 开始监视日志
    if(QFileInfo(m_focusedLog).suffix().toLower() != FILE_SUUIFX_ZST)
    {
        m_fileSystemWatcher.disconnect();
        m_fileSystemWatcher.removePaths(m_fileSystemWatcher.files());
        m_fileSystemWatcher.addPath(m_focusedLog);
        QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &path) {
            emit logContentModified();
        });
    }

    OPERATE_BEGIN;
    QObject::connect(m_thread, &QThread::started, this, [this]() { m_logLoaderThread->analyze(m_focusedLog); });
    QObject::connect(m_logLoaderThread, &LogLoaderThread::lineNumWidth, this, [this](int width) { emit lineNumWidth(width); });
    QObject::connect(m_logLoaderThread, &LogLoaderThread::newLogAvailable, this, [this](const bool containKeyword, const int lineIndex, const QString log) {
        m_logModel->appendLog(lineIndex, log);
        if(containKeyword)
        {
            m_summaryModel->appendLog(lineIndex, log);
        }
    });
    QObject::connect(m_logLoaderThread, &LogLoaderThread::openFileFailed, this, [this]() {
        qDebug() << "LogSearcher::openLog failed";
    });
    QObject::connect(m_logLoaderThread, &LogLoaderThread::operateFinish, this, [this]() { emit loadFinish(m_focusedLog, QFileInfo::exists(m_focusedLog)); OPERATE_DELETE; });
    OPERATE_END;
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

void LogSearcher::refilterSearchResult()
{
    m_summaryModel->clearAll();

    OPERATE_BEGIN;
    QObject::connect(m_thread, &QThread::started, this, [this]() { m_logLoaderThread->refilterSearchResult(); });
    QObject::connect(m_logLoaderThread, &LogLoaderThread::newLogAvailable, this, [this](const bool containKeyword, const int lineIndex, const QString log) {
        m_logModel->updateRow(lineIndex, log);
        if(containKeyword)
        {
            m_summaryModel->appendLog(lineIndex, log);
        }
    });
    QObject::connect(m_logLoaderThread, &LogLoaderThread::operateFinish, this, [this]() { /* */ OPERATE_DELETE; });
    OPERATE_END;
}

void LogSearcher::recolorfulKeyword(const int index, const bool ignoreKeyword)
{
    if(index < 0 || LogUtils::Keywords().size() == 0 || index >= LogUtils::Keywords().size())
        return;

    OPERATE_BEGIN;
    QObject::connect(m_thread, &QThread::started, this, [this, index, ignoreKeyword]() { m_logLoaderThread->recolorful(index, ignoreKeyword); });
    QObject::connect(m_logLoaderThread, &LogLoaderThread::updateSingleLine, this, [&](const int lineIndex, const int summaryLineIndex, const QString log, const bool ignoreKeyword) {
        m_logModel->updateRow(lineIndex, log);
        if(ignoreKeyword)
        {
            m_summaryModel->hideRow(summaryLineIndex);
        }
        else
        {
            m_summaryModel->showRow(summaryLineIndex);
            m_summaryModel->updateRow(summaryLineIndex, log);
        }
    });
    QObject::connect(m_logLoaderThread, &LogLoaderThread::operateFinish, this, [this]() { /* */ OPERATE_DELETE; });
    OPERATE_END;
}

void LogSearcher::find(const QString& targetKeyword,
                       const bool caseSensitivity /*= true*/,
                       const bool wholeWordWrap /*= true*/)
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
    QFuture<LogSearcher::LineNumber_Line_Pair> future = QtConcurrent::mapped(
        LogUtils::SplitFileAllLines(),
        [targetKeyword, wholeWordWrap, caseSensitivity, this](const LineNumber_Line_Pair& line)
        {
            return LogUtils::Find(line.second, targetKeyword, caseSensitivity, wholeWordWrap) >= 0 ? line : LineNumber_Line_Pair{};
        }
        );

    watcher.setFuture(future);
}

bool LogSearcher::isKeyword(const QString& word)
{
    for(auto iter = LogUtils::Keywords().begin(); iter != LogUtils::Keywords().end(); ++iter)
    {
        if(word == iter.value().first)
            return true;
    }
    return false;
}

QList<int> LogSearcher::getKeywordPos(const int lineIndex, const QString& word)
{
    if(lineIndex < 0 || lineIndex >= LogUtils::SplitFileAllLines().size() || word.isEmpty())
        return { -1, -1 };

    int beginPos = LogUtils::SplitFileAllLines()[lineIndex].second.indexOf(word);
    if(beginPos >= 0)
    {
        const int endPos = beginPos + word.length();
        return { beginPos, endPos };
    }
    return { -1, -1 };
}

void LogSearcher::toggleTOPMOST()
{
    static bool TOPMOST__ = false;

    TOPMOST__ = !TOPMOST__;
    if(TOPMOST__)
    {
        ::SetWindowPos((HWND)(/*this->winId()*/m_winId),
                       HWND_TOPMOST, 0, 0, 0, 0,
                       SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }
    else
    {
        ::SetWindowPos((HWND)(/*this->winId()*/m_winId),
                       HWND_NOTOPMOST, 0, 0, 0, 0,
                       SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }
}

bool LogSearcher::isHighlight(const int index)
{
    return LogUtils::HighlightLines().contains(index);
}

void LogSearcher::toggleHighlight(const int index)
{
    if(LogUtils::HighlightLines().contains(index))
    {
        LogUtils::HighlightLines().remove(index);
    }
    else
    {
        LogUtils::HighlightLines().insert(index);
    }
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

        QSettings settings(getSettingsAbsolutePath__(), QSettings::Format::IniFormat);
        settings.setValue("Global/Keywords", keywords);
        settings.sync();
    });
}
