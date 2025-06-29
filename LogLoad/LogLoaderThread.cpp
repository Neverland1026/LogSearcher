#include "LogLoaderThread.h"
#include <windows.h>
#include <QCoreApplication>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QDir>
#include "LogAnalysis/LogUtils.h"

void LogLoaderThread::analyze(const QString& filePath)
{
    LogUtils::SplitFileAllLines().resize(0);
    LogUtils::KeyLineInfos().resize(0);
    LogUtils::HighlightLines().clear();

    m_elapsedTimer.start();
    mapFile__(filePath);
    process__();
    qDebug() << "LogLoaderThread::analyze finish, cost" << m_elapsedTimer.elapsed() << "ms";

    emit operateFinish();
}

void LogLoaderThread::refilterSearchResult()
{
    if(LogUtils::SplitFileAllLines().isEmpty())
        return;

    LogUtils::KeyLineInfos().resize(0);

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

    emit operateFinish();
}

void LogLoaderThread::recolorful(const int toBeRecolorfulIndex, const bool ignoreKeyword)
{
    if(toBeRecolorfulIndex < 0 || LogUtils::KeyLineInfos().empty())
        return;

#pragma omp parallel for
    for(int i = 0; i < LogUtils::KeyLineInfos().size(); ++i)
    {
        const auto& lineInfo = LogUtils::KeyLineInfos()[i];
        if(lineInfo.keywordIndex == toBeRecolorfulIndex)
        {
            emit updateSingleLine(lineInfo.lineIndex, i, lineInfo.colorful(ignoreKeyword), ignoreKeyword);
        }
    }

    emit operateFinish();
}

void LogLoaderThread::remove(const int toBeRemovedIndex)
{
    if(toBeRemovedIndex < 0  || LogUtils::KeyLineInfos().empty())
        return;

    // 并行一定会有问题，先注释
    //#pragma omp parallel for
    for(int i = LogUtils::KeyLineInfos().size() - 1; i >= 0; --i)
    {
        /*const*/ auto& lineInfo = LogUtils::KeyLineInfos()[i];
        if(lineInfo.keywordIndex == toBeRemovedIndex)
        {
            lineInfo.keywordIndex = lineInfo.beginPos = lineInfo.endPos = -1;
            emit removeSingleLine(lineInfo.lineIndex, i, lineInfo.colorful());
            LogUtils::KeyLineInfos().removeAt(i);
        }
    }

    emit operateFinish();
}

void LogLoaderThread::mapFile__(const QString& filePath)
{
    auto fail__ = [&]()
    {
        emit openFileFailed();
        return;
    };

    QString transformedPath = filePath;
    const bool is_ZST = (QFileInfo(transformedPath).suffix().toLower() == FILE_SUUIFX_ZST);
    if(is_ZST)
    {
        // ./7z.exe x FileName.log.zst -oNewFolder
        const QString TimeStamp = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");

        QProcess process;
        QString program = QCoreApplication::applicationDirPath()  + "/7z.exe";
        QStringList arguments;
        arguments << "x"
                  << transformedPath
                  << "-o" + TimeStamp;
        qDebug() << "Unzip *.zst begin [" << program << arguments.join(" ") << "]";
        process.start(program, arguments);

        if (!process.waitForFinished(20000))
        {
            qDebug() << "Unzip *.zst failed [ timeout ]";
            process.kill();
            return fail__();
        }
        else
        {
            const QString output = process.readAllStandardOutput();
            if(!output.contains("Everything is Ok"))
            {
                return fail__();
            }

            const QString fileName = QFileInfo(filePath).fileName();
            transformedPath = QCoreApplication::applicationDirPath() + "/" + TimeStamp + "/" + fileName.left(fileName.length() - 4);
            qDebug() << "Unzip *.zst succeed [" << transformedPath << "[";
        }
    }

    QFile file(transformedPath);
    if (!file.open(QIODevice::ReadOnly))
        return fail__();

    const qint64 fileSize = file.size();
    uchar *memory = file.map(0, fileSize);
    if (!memory)
        return fail__();

    const char *data = reinterpret_cast<const char*>(memory);
    if(data)
    {
        // 直接赋值会导致 '\0' 截断
        /*Qstirng fileContent = QString(data);*/

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

    if(is_ZST)
    {
        QDir dir(QFileInfo(transformedPath).absoluteDir());
        qDebug() << "Unzip *.zst finish and removeRecursively [" << QFileInfo(transformedPath).absoluteDir() << "]";
        dir.removeRecursively();
    }

    if(!data)
        return fail__();
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
}
