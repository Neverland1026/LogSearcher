#include "LogLoaderThread.h"
#include <windows.h>

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

    const QStringList lines = m_fileContent.split("\r\n");
    for(int i = 0; i < lines.size(); ++i)
    {
        emit newLogAvailable(QString("<font color='#FFFFFF'>%1</font>").arg(lines[i]));

        emit progressChanged(std::ceil((float)(i + 2) / lines.size() * 100));

        //        LineInfo li(this, i);
        //        li.line = lines[i];
        //        for(auto iter = m_searchTarget.begin(); iter != m_searchTarget.end(); ++iter)
        //        {
        //            int pos = lines[i].indexOf(iter.value().first);
        //            if(pos >= 0)
        //            {
        //                li.containedKeywords.push_back({ iter.value().first, pos });
        //            }
        //        }

        //        emit appendLog(li.colorful());



    }
}
