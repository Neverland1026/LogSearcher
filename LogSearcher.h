#ifndef LOGSEARCHER_H
#define LOGSEARCHER_H

#include "qdir.h"
#include <QObject>
#include <QMap>
#include <QFileSystemWatcher>
#include <QWindow>
#include <QFileInfo>
#include <QDebug>
#include <QVector>
#include <thread>

#include "LogModel.h"

class LogSearcher : public QObject
{
    Q_OBJECT

public:

    explicit LogSearcher(QObject *parent = nullptr);
    ~LogSearcher();

    // 设置 WId
    void setWId(WId winid);

    // 添加关键字
    Q_INVOKABLE void insertKeyword(const int index, const QString& keyword, const QString& color);

    // 删除关键字
    Q_INVOKABLE void removeKeyword(const int index);

    // 开始查找
    Q_INVOKABLE void search(const QString& filePath);

    // 显隐前缀
    Q_INVOKABLE void togglePrefix();

    // 刷新
    Q_INVOKABLE void refresh();

    // 设置日志对象
    void setLogModel(LogModel* model1, LogModel* model2);

protected:

    // 映射文件内容
    void mapFile__(const QString& filePath);

    // 后处理
    void process__();

    // 跟新配置文件
    void refreshSettings__();

signals:

    // 新增关键字
    void addKeywordFinish(const QString keyword, const QString color);

    // 删除关键字完成
    void removeKeywordFinish(const int index);

private:

    // 窗口 WId
    WId m_winId;

    // 配置文件路径
    const QString m_settingsPath = "./settings.ini";

    // 查询关键字、关键字索引及对应前景色
    QMap<int, QPair<QString, QString>> m_searchTarget = {};

    // 文本内容
    QString m_fileContent;

    // 每行的信息
    struct LineInfo
    {
        using Keyword_Pos_Pair = QPair<QString, int>;

        LogSearcher* parent = nullptr;
        int lineNum = -1;
        QString line = "";
        QVector<Keyword_Pos_Pair> containedKeywords = {};

        LineInfo() = default;
        LineInfo(LogSearcher* parent, const int lineNum = -1)
        {
            this->parent = parent;
            this->lineNum = lineNum;
            this->line = "";
            this->containedKeywords.resize(0);
        }

        inline bool existKeyword() const
        {
            return !containedKeywords.empty();
        }

        QString colorful() const
        {
            if(existKeyword())
            {
                // m_searchTarget 简化
                QMap<QString, QString> kc;
                for(auto iter = parent->m_searchTarget.begin(); iter != parent->m_searchTarget.end(); ++iter)
                {
                    kc[iter.value().first] = iter.value().second;
                }

                auto newLine = this->line;
                qDebug() << "newLine  ==> " << newLine;
                Keyword_Pos_Pair kp = containedKeywords.first();
                if(parent->m_skipPrefix)
                {
                    newLine = newLine.mid(kp.second);
                    qDebug() << "newLine  ==========> " << newLine;
                }
                newLine.insert(parent->m_skipPrefix ? 0 : kp.second, QString("<font color='%1'>").arg(kc[kp.first]));
                newLine.insert((parent->m_skipPrefix ? 0 : kp.second) + 22 + kp.first.size(), QString("%1").arg("</font>"));
                newLine = QString("<font color='#FFFFFF'>") + newLine + QString("</font>");

                return newLine;
            }

            return this->line;
        }
    };
    QVector<LineInfo> m_allLineInfo;

    // 裁剪前缀
    bool m_skipPrefix = false;

    // 日志对象模型
    LogModel* m_logModel1 = nullptr;
    LogModel* m_logModel2 = nullptr;

};

#endif // LOGSEARCHER_H
