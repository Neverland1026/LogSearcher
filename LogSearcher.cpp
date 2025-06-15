#include "LogSearcher.h"
#include "LogAnalysis/LogUtils.h"
#include <windows.h>

QColor randomColorRGB_Safe()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    const int random_number_1 = dis(gen);
    const int random_number_2 = dis(gen);
    const int random_number_3 = dis(gen);

    QColor color;
    do {
        color = QColor(random_number_1 % 256, random_number_2 % 256, random_number_3 % 256);
    } while (color.red() > 230 && color.green() > 230 && color.blue() > 230);

    return color;
}

LogSearcher::LogSearcher(QObject *parent /*= nullptr*/)
    : QObject{parent}
{

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
        insertKeyword(-1, keywords[i], randomColorRGB_Safe().name());
    }
}

void LogSearcher::insertKeyword(const int index, const QString& keyword, const QString& color)
{
    QString finalColor = color;
    if(finalColor.isEmpty())
    {
        finalColor = randomColorRGB_Safe().name();
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

    refreshSettings__();
}

void LogSearcher::removeKeyword(const int index)
{
    LogUtils::Keywords().erase(std::next(LogUtils::Keywords().begin(), index));
    refreshSettings__();

    emit removeKeywordFinish(index);
}

void LogSearcher::openLog(const QString& filePath)
{
    // remove "file:///"
    QString convertedFilepath = filePath;
    const QString prefix = "file:///";
    if(convertedFilepath.contains(prefix))
    {
        convertedFilepath = convertedFilepath.mid(prefix.size());
    }

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
    QObject::connect(m_logLoaderThread, &LogLoaderThread::loadFinish, this, [&](){ emit loadFinish(); });
    //QObject::connect(m_thread, &QThread::finished, m_logLoaderThread, &QObject::deleteLater);
    //QObject::connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);

    // 设置查询属性
    m_logLoaderThread->setTargetLog(convertedFilepath);
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
    const QString prefix = "ScanService";
    QDir dir("C:/Users/Neverland_LY/Desktop/");
    if (!dir.exists())
    {
        return;
    }

    dir.setFilter(QDir::Files);
    QFileInfoList fileInfoList = dir.entryInfoList();

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
    m_elapsedTimer.start();

    watcher.disconnect();
    QObject::connect(&watcher, &QFutureWatcher<LogSearcher::LineNumber_Line_Pair>::finished, &watcher, [this, targetKeyword](){
        m_findModel->clearAll();
        for (const LogSearcher::LineNumber_Line_Pair& partialResult : watcher.future().results())
        {
            if(!partialResult.second.isEmpty())
            {
                QString dstLine;
                LogUtils::ConvertHTML(partialResult.second, dstLine, targetKeyword);
                m_findModel->appendLog(partialResult.first, dstLine);
            }
        }
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
