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
    LogUtils::KeyLineInfos().resize(0);

    mapFile__();
    process__();
}

void LogLoaderThread::recolorful()
{
    if(m_recolorfulKeywordIndex < 0)
        return;

    for(int i  = 0; i < LogUtils::KeyLineInfos().size(); ++i)
    {
        const auto& lineInfo = LogUtils::KeyLineInfos()[i];
        if(lineInfo.keywordIndex == m_recolorfulKeywordIndex)
        {
            emit updateSingleLineColor(lineInfo.lineIndex, i, lineInfo.colorful());
        }
    }
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
        LogUtils::LineInfo lineInfo;
        lineInfo.lineIndex = lineIndex;
        lineInfo.line = m_fileAllLines[lineIndex].second;
        const bool&& containKeyword = LogUtils::ConvertHTML(m_fileAllLines[lineIndex].second,
                                                            lineInfo.keywordIndex,
                                                            lineInfo.beginPos,
                                                            lineInfo.endPos);

#pragma omp ordered
        {
            if(lineInfo.beginPos >= 0)
            {
                LogUtils::KeyLineInfos().emplace_back(lineInfo.lineIndex,
                                                      lineInfo.line,
                                                      lineInfo.keywordIndex,
                                                      lineInfo.beginPos,
                                                      lineInfo.endPos);
            }

            emit newLogAvailable(containKeyword, lineIndex, lineInfo.colorful());
        }
    }

    // 解析结束
    emit loadFinish();
}
