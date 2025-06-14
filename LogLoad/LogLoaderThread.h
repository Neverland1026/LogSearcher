#ifndef LOGLOADERTHREAD_H
#define LOGLOADERTHREAD_H

#include <QObject>
#include <QString>
#include <QMap>

class LogLoaderThread : public QObject
{
    Q_OBJECT

public slots:

    // 设置日志路径
    inline void setTargetLog(const QString& filePath) { m_logPath = filePath; };

    // 设置关键字及对应颜色
    inline void setTargetKeywordAndColor(const QMap<QString, QString>& kcm) { m_keywordAndColorMap = kcm; };

    // 解析日志
    void analyze();

signals:

    void lineNumWidth(int width);

    void newLogAvailable(const bool containKeyword, const QString log);

    void loadFinish();

protected:

    // 映射文件内容
    void mapFile__();

    // 后处理
    void process__();

private:

    // 日志路径
    QString m_logPath;

    // 关键字及对应颜色
    QMap<QString, QString> m_keywordAndColorMap = {};

    // 文本内容
    QString m_fileContent;

};

#endif // LOGLOADERTHREAD_H
