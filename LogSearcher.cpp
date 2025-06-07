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
    , m_fileContent("")
{
    // 配置文件
    QTimer::singleShot(500, this, [&]() {
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
    });
}

LogSearcher::~LogSearcher()
{
}

void LogSearcher::setWId(WId winid)
{
    m_winId = winid;

    ::SetWindowPos((HWND)(/*this->winId()*/m_winId),
                   HWND_TOPMOST, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
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
    m_fileContent.clear();
    m_allLineInfo.resize(0);

    // remove "file:///"
    QString convertedFilepath = filePath;
    const QString prefix = "file:///";
    if(convertedFilepath.contains(prefix))
    {
        convertedFilepath = convertedFilepath.mid(prefix.size());
    }

    mapFile__(convertedFilepath);
}

void LogSearcher::togglePrefix()
{
    m_skipPrefix = !m_skipPrefix;
    refresh();
}

void LogSearcher::refresh()
{
    if(m_allLineInfo.isEmpty())
        return;

    QString colorfulLog = "";

    for(const auto& li : m_allLineInfo)
    {
        if(li.existKeyword())
        {
            li.parent->m_skipPrefix = this->m_skipPrefix;
            colorfulLog += li.colorful() + "<br>";
        }
    }

    if(!colorfulLog.isEmpty())
    {

    }
}

void LogSearcher::setLogModel(LogModel* model1, LogModel* model2)
{
    m_logModel1 = model1;
    m_logModel2 = model2;
}

void LogSearcher::mapFile__(const QString& filePath)
{
    auto read__ = [this](const LPCWSTR& filePath)
    {
        do
        {
            // 打开文件
            HANDLE hFile = CreateFile(filePath,
                                      GENERIC_READ,
                                      FILE_SHARE_READ,
                                      NULL,
                                      OPEN_EXISTING,
                                      FILE_ATTRIBUTE_NORMAL,
                                      NULL);
            if (hFile == INVALID_HANDLE_VALUE)
            {
                break;
            }

            // 创建文件映射对象
            HANDLE hMapFile = CreateFileMapping(hFile,
                                                NULL,
                                                PAGE_READONLY,
                                                0,
                                                0,
                                                NULL);
            if (hMapFile == NULL)
            {
                CloseHandle(hFile);
                break;
            }

            // 映射视图到内存
            LPVOID lpBaseAddress = MapViewOfFile(hMapFile,
                                                 FILE_MAP_READ,
                                                 0,
                                                 0,
                                                 0);
            if (lpBaseAddress == NULL)
            {
                CloseHandle(hMapFile);
                CloseHandle(hFile);
                break;
            }

            // 读取文件内容（假设文件为文本文件，且内容较短）
            // 注意：这里直接将内存地址转换为char*指针，并输出字符串内容
            // 在实际应用中，应根据文件内容格式进行相应处理
            const char* pData = static_cast<const char*>(lpBaseAddress);
            if(pData)
            {
                m_fileContent = QString(pData);
            }

            // 取消映射并释放资源
            UnmapViewOfFile(lpBaseAddress);
            CloseHandle(hMapFile);
            CloseHandle(hFile);

            // 后处理
            process__();
        } while(0);
    };

    auto ret = std::async([&, filePath](){ read__(filePath.toStdWString().c_str()); });
}

void LogSearcher::process__()
{
    if(m_fileContent.isEmpty())
        return;

    QString colorfulLog = "";

    const QStringList lines = m_fileContent.split("\r\n");
    for(int i = 0; i < lines.size(); ++i)
    {
        LineInfo li(this, i);
        li.line = lines[i];
        for(auto iter = m_searchTarget.begin(); iter != m_searchTarget.end(); ++iter)
        {
            int pos = lines[i].indexOf(iter.value().first);
            if(pos >= 0)
            {
                li.containedKeywords.push_back({ iter.value().first, pos });
            }
        }

        if(li.existKeyword())
        {
            colorfulLog += li.colorful() + "<br>";
        }

        m_allLineInfo.push_back(std::move(li));
    }

    if(!colorfulLog.isEmpty())
    {

    }
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
