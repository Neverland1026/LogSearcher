#ifndef LOGLOADERTHREAD_H
#define LOGLOADERTHREAD_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>

class LogLoaderThread : public QObject
{
    Q_OBJECT

public slots:

    // 设置日志路径
    inline void setTargetLog(const QString& filePath) { m_logPath = filePath; };

    // 设置重新着色的关键字索引
    inline void setRecolorfulInfo(const int index, const bool ignoreKeyword) { m_recolorfulInfo = { index, ignoreKeyword }; };

    // 要被删除的关键字索引
    inline void setRemoveKeywordIndex(const int index) { m_toBeRemovedIndex = index; }

    // 解析日志
    void analyze();

    // 关键字重着色
    void recolorful();

    // 删除关键字
    void remove();

signals:

    void lineNumWidth(int width);

    void newLogAvailable(const bool containKeyword, const int lineIndex, const QString log);

    void loadFinish();

    void updateSingleLine(const int lineIndex, const int summaryLineIndex, const QString log, const bool ignoreKeyword);

    void removeSingleLine(const int lineIndex, const int summaryLineIndex, const QString log);

protected:

    // 映射文件内容
    void mapFile__();

    // 后处理
    void process__();

private:

    // 日志路径
    QString m_logPath;

    // 将要重新着色的关键字索引
    QPair<int, bool> m_recolorfulInfo = { -1, false };

    // 将要删除的关键字索引
    int m_toBeRemovedIndex = -1;

};

#endif // LOGLOADERTHREAD_H
