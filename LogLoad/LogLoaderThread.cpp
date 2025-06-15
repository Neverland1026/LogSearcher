#include "LogLoaderThread.h"
#include <windows.h>
#include <QThread>
#include <QDebug>
#include <QFile>
#include "LogAnalysis/LogUtils.h"

void LogLoaderThread::analyze()
{
    mapFile__();
    process__();
}

void LogLoaderThread::mapFile__()
{
    auto read_method2__ = [this](const QString& filePath)
    {
        do
        {
            QFile file(filePath);
            if (!file.open(QIODevice::ReadOnly))
                break;

            const qint64 fileSize = file.size();
            uchar *memory = file.map(0, fileSize);
            if (!memory)
                break;

            const char *data = reinterpret_cast<const char*>(memory);
            if(data)
            {
                m_fileContent = QString(data);
            }

            file.unmap(memory);
            file.close();
        } while(0);
    };

    read_method2__(m_logPath);
}

void LogLoaderThread::process__()
{
    if(m_fileContent.isEmpty())
        return;

    // 按照换行符分隔
    const QStringList&& qstrlist = m_fileContent.split("\r\n");
    for(int i = 0; i < qstrlist.size(); ++i)
    {
        m_fileAllLines.emplace_back(i, qstrlist[i]);
    }

    // 推测行号宽度
    int count = 0;
    int totalCount = m_fileAllLines.size();
    while (totalCount != 0) {
        count++;
        totalCount /= 10;
    }
    emit lineNumWidth(count);

// 日志解析
#pragma omp parallel for
    for(int i = 0; i < m_fileAllLines.size(); ++i)
    {
        QString dstLine;
        const bool&& containKeyword = LogUtils::ConvertHTML(m_fileAllLines[i].second, dstLine);

#pragma omp ordered
        {
            emit newLogAvailable(containKeyword, i, dstLine);
        }
    }

    // 解析结束
    emit loadFinish();
}
