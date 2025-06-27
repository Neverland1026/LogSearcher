#include "LogLoaderThread.h"
#include <windows.h>
#include <QThread>
#include <QDebug>
#include <QFile>
#include "LogAnalysis/LogUtils.h"

void LogLoaderThread::analyze()
{
    LogUtils::SplitFileAllLines().resize(0);
    LogUtils::KeyLineInfos().resize(0);

    mapFile__();
    process__();
}

void LogLoaderThread::recolorful()
{
    if(m_recolorfulInfo.first < 0)
        return;

    for(int i  = 0; i < LogUtils::KeyLineInfos().size(); ++i)
    {
        const auto& lineInfo = LogUtils::KeyLineInfos()[i];
        if(lineInfo.keywordIndex == m_recolorfulInfo.first)
        {
            emit updateSingleLineColor(lineInfo.lineIndex, i, lineInfo.colorful(m_recolorfulInfo.second), m_recolorfulInfo.second);
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
                // 直接赋值会导致 '\0' 截断
                /*m_fileContent = QString(data);*/

                // 直接赋值给 QString 会导致 '\0' 截断，QByteArray::split() 会正确处理内部的 '\0'
                QByteArray byteArray(data, fileSize);  // 明确指定长度，避免 \0 截断
                const QList<QByteArray> splitByteArray = byteArray.split('\n');
                for(int lineIndex = 0; lineIndex < splitByteArray.size(); ++lineIndex)
                {
                    LogUtils::SplitFileAllLines().emplace_back(lineIndex, splitByteArray[lineIndex]);
                }
            }

            file.unmap(memory);
            file.close();
        } while(0);
    };

    read_method2__(m_logPath);
}

void LogLoaderThread::process__()
{
    if(LogUtils::SplitFileAllLines().isEmpty())
        return;

    // 推测行号宽度
    int count = 0;
    int totalCount = LogUtils::SplitFileAllLines().size();
    while (totalCount != 0) {
        count++;
        totalCount /= 10;
    }
    emit lineNumWidth(count);

    // 日志解析
#pragma omp parallel for
    for(int lineIndex = 0; lineIndex < LogUtils::SplitFileAllLines().size(); ++lineIndex)
    {
        LogUtils::LineInfo lineInfo;
        lineInfo.lineIndex = lineIndex;
        lineInfo.line = LogUtils::SplitFileAllLines()[lineIndex].second;
        const bool&& containKeyword = LogUtils::ConvertHTML(LogUtils::SplitFileAllLines()[lineIndex].second,
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
