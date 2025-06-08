#include "LogLoaderThread.h"
#include <windows.h>
#include <QThread>
#include <QDebug>

void LogLoaderThread::analyze()
{
    mapFile__();
    process__();
}

void LogLoaderThread::mapFile__()
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
        } while(0);
    };

    read__(m_logPath.toStdWString().c_str());
}

void LogLoaderThread::process__()
{
    if(m_fileContent.isEmpty())
        return;

    // 按照换行符分隔
    /*const*/ QStringList lines = m_fileContent.split("\r\n");

    // 推测行号宽度
    int count = 0;
    int totalCount = lines.size();
    while (totalCount != 0) {
        count++;
        totalCount /= 10;
    }
    emit lineNumWidth(count);

    // 颜色处理
    auto colorful__ = [&](QString& line, const bool skip = false)
    {
        QString keyword = "";
        QString color = "";
        int insertIndex = -1;
        for(auto iter = m_keywordAndColorMap.begin(); iter != m_keywordAndColorMap.end(); ++iter)
        {
            insertIndex = line.indexOf(iter.key());
            if(insertIndex >= 0)
            {
                keyword = iter.key();
                color = iter.value();
                break;
            }
        }

        if(insertIndex < 0)
            return QString("<font color='#FFFFFF'>%1</font>").arg(line);

        auto newLine = line;
        newLine.insert(insertIndex + keyword.size(), QString("%1").arg("</font>"));
        newLine.insert(insertIndex, QString("<font color='%1'>").arg(color));
        newLine = QString("<font color='#FFFFFF'>%1</font>").arg(newLine);

        static int loop = 0;
        if(loop++ < 6)
        {
            qDebug() << "_____" << newLine;
        }

        return newLine;
    };

    // 日志解析
    for(int i = 0; i < lines.size(); ++i)
    {
        emit newLogAvailable(colorful__(lines[i]));

        if(0 == i % 100 || (lines.size() - 1 == i))
        {
            emit progressChanged((float)(i) / lines.size());
            QThread::msleep(5);
        }

        if(lines.size() - 1 == i)
        {
            emit progressChanged(-1);
        }
    }
}
