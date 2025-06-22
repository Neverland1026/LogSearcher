#ifndef LOGLOADERTHREAD_H
#define LOGLOADERTHREAD_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>
#include <QColor>

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

    // 返回日志引用
    inline const QVector<QPair<int, QString>>& getAllLines() const { return m_fileAllLines; }

signals:

    void lineNumWidth(int width);

    void newLogAvailable(const bool containKeyword, const int lineIndex, const QString log);

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
    QString m_fileContent = {};

    // 分割后的文本內容
    QVector<QPair<int, QString>> m_fileAllLines = {};

    // 检索到关键字的行信息
    struct LineInfo
    {
        int lineIndex = -1;
        QString line = "";
        int beginPos = -1;
        int endPos = -1;
        QString color = "#FFFFFF";

        LineInfo()
        {
            this->lineIndex = -1;
            this->line = "";
            this->beginPos = -1;
            this->endPos = -1;
            this->color = "#000000";
        }
        LineInfo(const int lineIndex, const QString& line, const int beginPos, const int endPos, const QString& color)
        {
            this->lineIndex = lineIndex;
            this->line = line;
            this->beginPos = beginPos;
            this->endPos = endPos;
            this->color = color;
        }
        QString colorful()
        {
            if(beginPos < 0)
                return QString("<font color='#000000'>%1</font>").arg(this->line);

            QString dstLine = this->line;
            dstLine.insert(endPos, QString("%1").arg("</b></font>"));
            dstLine.insert(beginPos, QString("<font color='%1'><b>").arg(color));
            dstLine = QString("<font color='#000000'>%1</font>").arg(dstLine);

            return dstLine;
        }
    };
    QVector<LineInfo> m_keyLineInfos = {};

};

#endif // LOGLOADERTHREAD_H
