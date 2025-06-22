#include "LogLoaderThread.h"
#include <windows.h>
#include <QThread>
#include <QDebug>
#include <QFile>
#include "LogAnalysis/LogUtils.h"

void LogLoaderThread::analyze()
{
    m_fileContent.resize(0);
    m_fileAllLines.resize(0);
    m_lineInfos.resize(0);

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
    for(int lineIndex = 0; lineIndex < qstrlist.size(); ++lineIndex)
    {
        m_fileAllLines.emplace_back(lineIndex, qstrlist[lineIndex]);
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
    for(int lineIndex = 0; lineIndex < m_fileAllLines.size(); ++lineIndex)
    {
        int beginPos;
        int endPos;
        QColor color;
        const bool&& containKeyword = LogUtils::ConvertHTML(m_fileAllLines[lineIndex].second, beginPos, endPos, color);

#pragma omp ordered
        {
            QString dstLine = m_fileAllLines[lineIndex].second;
            if(containKeyword)
            {
                m_lineInfos.emplace_back(lineIndex, m_fileAllLines[lineIndex].second, beginPos, endPos, color);
                dstLine.insert(endPos, QString("%1").arg("</b></font>"));
                dstLine.insert(beginPos, QString("<font color='%1'><b>").arg(color.name()));
                dstLine = QString("<font color='#000000'>%1</font>").arg(dstLine);
            }

            emit newLogAvailable(containKeyword, lineIndex, dstLine);
        }
    }

    // 解析结束
    emit loadFinish();
}
