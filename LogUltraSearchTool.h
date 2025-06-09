#ifndef LOGULTRASEARCHTOOL_H
#define LOGULTRASEARCHTOOL_H

#include <QFile>
#include <QtConcurrent>
#include <QThreadPool>
#include <QMutex>
#include <vector>
#include <algorithm>

class LogUltraSearchTool {
    struct IndexCache {
        QDateTime lastModified;
        QMap<QString, QVector<qint64>> keywordPositions;
    };

    static QMap<QString, IndexCache> globalCache;
    static QMutex cacheMutex;

public:
    static QStringList searchUltra(const QString &filePath, const QString &keyword,
                                   bool useCache = true, int threadCount = QThread::idealThreadCount()) {
        // 检查缓存
        if (useCache) {
            QMutexLocker locker(&cacheMutex);
            auto it = globalCache.find(filePath);
            if (it != globalCache.end()) {
                QFileInfo info(filePath);
                if (it->lastModified == info.lastModified()) {
                    if (it->keywordPositions.contains(keyword)) {
                        return readLinesByPositions(filePath, it->keywordPositions[keyword]);
                    }
                }
            }
        }

        // 内存映射文件
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) return QStringList();

        qint64 fileSize = file.size();
        uchar *memory = file.map(0, fileSize);
        if (!memory) return QStringList();

        const char *data = reinterpret_cast<const char*>(memory);
        QStringList results;
        QMutex resultMutex;

        // 使用Boyer-Moore预处理
        QByteArray pattern = keyword.toUtf8();
        std::vector<int> badChar(256, -1);
        for (size_t i = 0; i < pattern.size(); ++i)
            badChar[static_cast<uchar>(pattern[i])] = i;

        // 分割文件块
        auto chunks = splitFile(fileSize, threadCount, data);

        // 并行搜索
        QtConcurrent::blockingMap(chunks, [&](const auto &chunk) {
            QVector<qint64> localPositions;
            const qint64 chunkEnd = chunk.first + chunk.second;

            // Boyer-Moore搜索实现
            qint64 n = chunk.second;
            qint64 m = pattern.size();
            qint64 s = 0;

            while (s <= n - m) {
                qint64 j = m - 1;
                while (j >= 0 && pattern[j] == data[chunk.first + s + j])
                    j--;

                if (j < 0) {
                    localPositions.append(chunk.first + s);
                    s += (s + m < n) ? m - badChar[data[chunk.first + s + m]] : 1;
                } else {
                    s += std::max(1, j - badChar[data[chunk.first + s + j]]);
                }
            }

            // 合并结果
            if (!localPositions.isEmpty()) {
                QMutexLocker locker(&resultMutex);
                for (qint64 pos : localPositions) {
                    results.append(extractLine(data, pos, fileSize));
                }
            }
        });

        // 更新缓存
        if (useCache) {
            QMutexLocker locker(&cacheMutex);
            QFileInfo info(filePath);
            globalCache[filePath].lastModified = info.lastModified();
            globalCache[filePath].keywordPositions[keyword] = extractAllPositions(results, data, fileSize);
        }

        file.unmap(memory);
        file.close();
        return results;
    }

private:
    static QVector<QPair<qint64, qint64>> splitFile(qint64 fileSize, int threadCount, const char *data) {
        QVector<QPair<qint64, qint64>> chunks;
        qint64 chunkSize = fileSize / threadCount;

        for (int i = 0; i < threadCount; ++i) {
            qint64 start = i * chunkSize;
            qint64 end = (i == threadCount - 1) ? fileSize : (i + 1) * chunkSize;

            // 确保每个块从行首开始
            if (i > 0) {
                while (start < end && data[start] != '\n') ++start;
                if (start < end) ++start;
            }
            chunks.append({start, end - start});
        }
        return chunks;
    }

    static QString extractLine(const char *data, qint64 pos, qint64 fileSize) {
        qint64 lineStart = pos;
        while (lineStart > 0 && data[lineStart - 1] != '\n') lineStart--;

        qint64 lineEnd = pos;
        while (lineEnd < fileSize && data[lineEnd] != '\n') lineEnd++;

        return QString::fromUtf8(data + lineStart, lineEnd - lineStart);
    }

    static QVector<qint64> extractAllPositions(const QStringList &results, const char *data, qint64 fileSize) {
        QVector<qint64> positions;
        for (const QString &line : results) {
            QByteArray utf8 = line.toUtf8();
            const char *needle = utf8.constData();
            qint64 len = utf8.size();

            // 简单线性搜索定位行位置
            for (qint64 i = 0; i <= fileSize - len; ++i) {
                if (std::equal(needle, needle + len, data + i)) {
                    positions.append(i);
                    break;
                }
            }
        }
        return positions;
    }

    static QStringList readLinesByPositions(const QString &filePath, const QVector<qint64> &positions) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) return QStringList();

        QStringList results;
        uchar *memory = file.map(0, file.size());
        if (!memory) return QStringList();

        const char *data = reinterpret_cast<const char*>(memory);
        qint64 fileSize = file.size();

        for (qint64 pos : positions) {
            results.append(extractLine(data, pos, fileSize));
        }

        file.unmap(memory);
        file.close();
        return results;
    }
};

QMap<QString, LogUltraSearchTool::IndexCache> LogUltraSearchTool::globalCache;
QMutex LogUltraSearchTool::cacheMutex;

#endif // LOGULTRASEARCHTOOL_H
