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

#include "LogModel/LogModel.h"
#include "LogLoad/LogLoaderThread.h"

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

    // 设置日志对象
    void setSearchModel(LogModel* model1, LogModel* model2);

protected:

    // 跟新配置文件
    void refreshSettings__();

signals:

    // 新增关键字
    void addKeywordFinish(const QString keyword, const QString color);

    // 删除关键字完成
    void removeKeywordFinish(const int index);

    // 行号显示宽度
    void lineNumWidth(int width);

    // 进度更新
    void loadFinish();

private:

    // 窗口 WId
    WId m_winId;

    // 配置文件路径
    const QString m_settingsPath = "./settings.ini";

    // 查询关键字、关键字索引及对应前景色
    QMap<int, QPair<QString, QString>> m_searchTarget = {};

    // 日志对象模型
    LogModel* m_logModel = nullptr;
    LogModel* m_resultModel = nullptr;

    // 日志加载线程
    QThread* m_thread = nullptr;
    LogLoaderThread* m_logLoaderThread = nullptr;

};

#endif // LOGSEARCHER_H
