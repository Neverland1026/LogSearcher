#ifndef LOGLOADERTHREAD_H
#define LOGLOADERTHREAD_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>
#include <QElapsedTimer>

class LogLoaderThread : public QObject
{
    Q_OBJECT

public slots:

    // 解析日志
    void analyze(const QString& filePath);

    // 重新过滤关键字
    void refilterSearchResult();

    // 关键字重着色
    void recolorful(const int toBeRecolorfulIndex, const bool ignoreKeyword);

    // 删除关键字
    void remove(const int toBeRemovedIndex);

signals:

    void lineMaximumWidth(int width);

    void lineNumWidth(int width);

    void newLogAvailable(const bool containKeyword, const int lineIndex, const QString log);

    void updateSingleLine(const int lineIndex, const int summaryLineIndex, const QString log, const bool ignoreKeyword);

    void removeSingleLine(const int lineIndex, const int summaryLineIndex, const QString log);

    void openFileFailed();

    void operateFinish();

protected:

    // 映射文件内容
    void mapFile__(const QString& filePath);

    // 后处理
    void process__();

private:

    // 计时器
    QElapsedTimer m_elapsedTimer;

};

#endif // LOGLOADERTHREAD_H
