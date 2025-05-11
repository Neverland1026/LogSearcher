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

void LogSearcher::search(const QString& filePath)
{
    // remove "file:///"
    QString convertedFilepath = filePath;
    const QString prefix = "file:///";
    if(convertedFilepath.contains(prefix))
    {
        convertedFilepath = convertedFilepath.mid(prefix.size());
    }

    mapFile__(convertedFilepath);
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

            // 获取文件大小
            LARGE_INTEGER fileSize;
            if (!GetFileSizeEx(hFile, &fileSize)) {
                CloseHandle(hFile);
                break;
            }

            // 创建文件映射对象
            HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
            if (hMapFile == NULL)
            {
                CloseHandle(hFile);
                break;
            }

            // 映射视图到内存
            LPVOID lpBaseAddress = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
            if (lpBaseAddress == NULL)
            {
                CloseHandle(hMapFile);
                CloseHandle(hFile);
                break;
            }

            // 读取文件内容（假设文件为文本文件，且内容较短）
            // 注意：这里直接将内存地址转换为char*指针，并输出字符串内容
            // 在实际应用中，应根据文件内容格式进行相应处理
            char* fileContent = static_cast<char*>(lpBaseAddress);
            /*m_fileContent = QString::fromUtf8(static_cast<char*>(lpBaseAddress), static_cast<int>(fileSize.QuadPart));*/
            QString tmp(fileContent);
            process__(tmp);

            // 取消映射并释放资源
            UnmapViewOfFile(lpBaseAddress);
            CloseHandle(hMapFile);
            CloseHandle(hFile);
        } while(0);
    };

    auto ret = QtConcurrent::run([&, filePath](){ read__(filePath.toStdWString().c_str()); });
}

void LogSearcher::process__(const QString& fileContent)
{
    if(fileContent.isEmpty())
        return;

    const QStringList lines = fileContent.split("\n");
    for(const auto/*&*/ line : lines)
    {
        emit appendContent(line);
    }
}
