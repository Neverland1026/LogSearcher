#include "LogSearcher.h"
#include <QtConcurrent>
#include <windows.h>

int LogSearcher::mousePosX() const
{
    return m_mousePosX;
}

void LogSearcher::setMousePosX(int mousePosX)
{
    if (mousePosX != m_mousePosX) {
        m_mousePosX = mousePosX;
        emit mousePosXChanged(m_mousePosX);
    }
}

int LogSearcher::mousePosY() const
{
    return m_mousePosY;
}

void LogSearcher::setMousePosY(int mousePosY)
{
    if (mousePosY != m_mousePosY) {
        m_mousePosY = mousePosY;
        emit mousePosYChanged(m_mousePosY);
    }
}

LogSearcher::LogSearcher(QObject *parent /*= nullptr*/)
    : QObject{parent}
    , m_mousePosX(0)
    , m_mousePosY(0)
    , m_fileContent("")
{
    QObject::connect(this, &LogSearcher::dataReady, [&](const QString data) {
        emit appendContent(data);
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
    m_searchTarget[index] = { keyword, color };
    qDebug() << "LogSearcher::insertKeyword" << m_searchTarget;
}

void LogSearcher::removeKeyword(const int index)
{
    m_searchTarget.erase(std::next(m_searchTarget.begin(), index));
    qDebug() << "LogSearcher::removeKeyword" << m_searchTarget;
    emit removeKeywordFinish(index);
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

void LogSearcher::refresh()
{
    if(m_allLineInfo.isEmpty())
        return;

    QString colorfulLog = "";

    for(const auto& li : m_allLineInfo)
    {
        if(li.existKeyword())
        {
            colorfulLog += li.colorful() + "\n";
        }
    }

    if(!colorfulLog.isEmpty())
    {
        emit dataReady(colorfulLog);
    }
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
            colorfulLog += li.colorful() + "\n";
        }

        m_allLineInfo.push_back(std::move(li));
    }

    if(!colorfulLog.isEmpty())
    {
        emit dataReady(colorfulLog);
    }
}
