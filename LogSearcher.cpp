#include "LogSearcher.h"
#include <QtConcurrent>
#include <windows.h>

QColor randomColorRGB_Safe()
{
    QColor color;
    do {
        color = QColor(rand() % 256, rand() % 256, rand() % 256);
    } while (color.red() > 230 && color.green() > 230 && color.blue() > 230);

    return color;
}

LogSearcher::LogSearcher(QObject *parent /*= nullptr*/)
    : QObject{parent}
{
    // 创建并启动工作线程
    m_thread = new QThread(this);
    m_logLoaderThread = new LogLoaderThread;
    m_logLoaderThread->moveToThread(m_thread);

    QObject::connect(m_thread, &QThread::started, m_logLoaderThread, &LogLoaderThread::analyze);
    QObject::connect(m_logLoaderThread, &LogLoaderThread::lineNumWidth, this, [&](int width){ emit lineNumWidth(width); });
    QObject::connect(m_logLoaderThread, &LogLoaderThread::newLogAvailable, this, [&](const bool containKeyword, const QString log){
        m_logModel->appendLog(log);
        if(containKeyword)
        {
            m_resultModel->appendLog(log);
        }
    });
    QObject::connect(m_logLoaderThread, &LogLoaderThread::loadFinish, this, [&](){ emit loadFinish(); });
    //QObject::connect(m_thread, &QThread::finished, m_logLoaderThread, &QObject::deleteLater);
    //QObject::connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);

    // 配置文件
    QtConcurrent::run([&](){
        QThread::msleep(500);

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
            //insertKeyword(-1, keywords[i], randomColorRGB_Safe().name());
        }
    });
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
        m_searchTarget[m_searchTarget.size()] = { keyword, finalColor };
        qDebug() << "LogSearcher::insertKeyword [ Add ]" << m_searchTarget;
        emit addKeywordFinish(keyword.isEmpty() ? "     " : keyword, finalColor);
    }
    else
    {
        // 修改
        m_searchTarget[index] = { keyword, finalColor };
        qDebug() << "LogSearcher::insertKeyword [ Modify ]" << m_searchTarget;
    }

    refreshSettings__();
}

void LogSearcher::removeKeyword(const int index)
{
    m_searchTarget.erase(std::next(m_searchTarget.begin(), index));
    refreshSettings__();

    emit removeKeywordFinish(index);

    qDebug() << "LogSearcher::removeKeyword" << m_searchTarget;
}

void LogSearcher::search(const QString& filePath)
{
    // remove "file:///"
    QString convertedFilepath = filePath;
    const QString prefix = "file:///";
    if(convertedFilepath.contains(prefix))
    {
        convertedFilepath = convertedFilepath.mid(prefix.size());
    }

    // 设置查询属性
    m_logLoaderThread->setTargetLog(convertedFilepath);
    QMap<QString, QString> kcm;
    for(auto iter = m_searchTarget.begin(); iter != m_searchTarget.end(); ++iter)
    {
        kcm[iter.value().first] = iter.value().second;
    }
    m_logLoaderThread->setTargetKeywordAndColor(kcm);

    // 开始查询
    emit loadStart();
    m_thread->start();
}

void LogSearcher::setSearchModel(LogModel* model1, LogModel* model2)
{
    m_logModel = model1;
    m_resultModel = model2;
}

void LogSearcher::refreshSettings__()
{
    std::async(std::launch::async, [&](){
        QString keywords = "";
        for(auto iter = m_searchTarget.begin(); iter != m_searchTarget.end(); ++iter)
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
