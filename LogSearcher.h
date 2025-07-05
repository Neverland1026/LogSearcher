#ifndef LOGSEARCHER_H
#define LOGSEARCHER_H

#include "qdir.h"
#include <QObject>
#include <QMap>
#include <QtConcurrent>
#include <QFileSystemWatcher>
#include <QWindow>
#include <QFileInfo>
#include <QDebug>
#include <QVector>
#include <QElapsedTimer>
#include <QFileSystemWatcher>
#include <thread>

#include "LogModel/LogModel.h"
#include "LogLoad/LogLoaderThread.h"

class LogSearcher : public QObject
{
    Q_OBJECT

public:

    Q_PROPERTY(bool topMOST READ topMOST WRITE setTopMOST NOTIFY topMOSTChanged);
    bool topMOST() const { return m_topMOST; }
    void setTopMOST(bool topMOST) { if(m_topMOST != topMOST) { m_topMOST = topMOST; emit topMOSTChanged(m_topMOST); } }

    explicit LogSearcher(QObject *parent = nullptr);
    ~LogSearcher();

    // 设置 WId
    void setWId(WId winid);

    // 设置日志对象
    Q_INVOKABLE void setSearchModel(LogModel* model1, LogModel* model2, LogModel* model3);

    // 初始化
    Q_INVOKABLE void init();

    // 添加关键字
    Q_INVOKABLE void insertKeyword(const int index, const QString& keyword, const QString& color);

    // 删除关键字
    Q_INVOKABLE void removeKeyword(const int index);

    // 打开日志
    Q_INVOKABLE void openLog(const QString& filePath, const bool repeatOpen = false);
    Q_INVOKABLE void openLatestIndexLog(const int latestIndex);

    // 重新过滤关键字
    Q_INVOKABLE void refilterSearchResult();

    // 关键字重着色
    Q_INVOKABLE void recolorfulKeyword(const int index, const bool ignoreKeyword);

    // 查找指定关键字
    Q_INVOKABLE void find(const QString& targetKeyword,
                          const bool caseSensitivity = true,
                          const bool wholeWordWrap = true);

    // 某字段是否为关键字
    Q_INVOKABLE bool isKeyword(const QString& word);

    // 查询某个关键字在指定行的索引
    Q_INVOKABLE QList<int> getKeywordPos(const int lineIndex, const QString& word);

    // 置顶切换
    Q_INVOKABLE void toggleTOPMOST();

    // 切换行高亮
    Q_INVOKABLE bool isHighlight(const int index);
    Q_INVOKABLE void toggleHighlight(const int index);

protected:

    // 查找
    using LineNumber_Line_Pair = QPair<int, QString>;

    // 配置文件绝对路径
    inline QString getSettingsAbsolutePath__() const { return m_rootPath + "/" + m_settingsName; }

    // 更新配置文件
    void refreshSettings__();

signals:

    void topMOSTChanged(bool);

    // 新增关键字
    void addKeywordFinish(const QString keyword, const QString color);

    // 删除关键字完成
    void removeKeywordFinish(const int index);

    // 行号显示宽度
    void lineNumWidth(int width);

    // 加载开始
    void loadStart();

    // 结束加载
    void loadFinish(const QString& logPath, const bool exist);

    // 查找结束
    void findFinish(const QString& targetKeyword, const int findCount, const int findTimeCost);

    // 日志有变化
    void logContentModified();

private:

    // 窗口 WId
    WId m_winId;

    // exe 运行目录
    QString m_rootPath = "";

    // 配置文件路径
    const QString m_settingsName = "settings.ini";

    // 日志对象模型
    LogModel* m_logModel = nullptr;
    LogModel* m_summaryModel = nullptr;
    LogModel* m_findModel = nullptr;

    // 当前的目标日志
    QString m_focusedLog = "";

    // 日志加载线程
    QThread* m_thread = nullptr;
    LogLoaderThread* m_logLoaderThread = nullptr;

    // 在日志中查找关键字
    QElapsedTimer m_elapsedTimer;
    QFutureWatcher<LineNumber_Line_Pair> watcher;

    // 文件监视器
    QFileSystemWatcher m_fileSystemWatcher;

    // 软件置顶
    bool m_topMOST = false;

};

#endif // LOGSEARCHER_H
