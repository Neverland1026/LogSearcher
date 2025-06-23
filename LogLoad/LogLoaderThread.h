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
    inline void setRecolorfulKeywordIndex(const int index) { m_recolorfulKeywordIndex = index; };

    // 解析日志
    void analyze();

    // 关键字重着色
    void recolorful();

signals:

    void lineNumWidth(int width);

    void newLogAvailable(const bool containKeyword, const int lineIndex, const QString log);

    void loadFinish();

    void updateSingleLineColor(const int lineIndex, const int summaryLineIndex, const QString log);

protected:

    // 映射文件内容
    void mapFile__();

    // 后处理
    void process__();

private:

    // 日志路径
    QString m_logPath;

    // 重新着色的关键字索引
    int m_recolorfulKeywordIndex = -1;

    // 文本内容
    QString m_fileContent = {};

};

#endif // LOGLOADERTHREAD_H
