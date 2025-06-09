#include "LogLoaderThread.h"
#include <windows.h>
#include <QThread>
#include <QDebug>
#include <QFile>

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
        {
            line = QString("<font color='#FFFFFF'>%1</font>").arg(line);
            return false;
        }

        line.insert(insertIndex + keyword.size(), QString("%1").arg("</font>"));
        line.insert(insertIndex, QString("<font color='%1'>").arg(color));
        line = QString("<font color='#FFFFFF'>%1</font>").arg(line);

        return true;
    };

    // 日志解析
    for(int i = 0; i < lines.size(); ++i)
    {
        const bool&& containKeyword = colorful__(lines[i]);
        emit newLogAvailable(containKeyword, lines[i]);

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
