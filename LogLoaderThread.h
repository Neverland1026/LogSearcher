#ifndef LOGLOADERTHREAD_H
#define LOGLOADERTHREAD_H

#include <QObject>
#include <QString>

class LogLoaderThread : public QObject
{
    Q_OBJECT

public slots:

    // 设置日志路径
    inline void setTargetLog(const QString& filePath) { m_logPath = filePath; };

    // 解析日志
    void analyze();

signals:

    void newLogAvailable(const QString newlog);

    void progressChanged(int value);

protected:

    // 映射文件内容
    void mapFile__();

    // 后处理
    void process__();

private:

    // 日志路径
    QString m_logPath;

    // 文本内容
    QString m_fileContent;

};

#endif // LOGLOADERTHREAD_H
