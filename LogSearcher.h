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

class LogSearcher : public QObject
{
    Q_OBJECT

public:

    Q_PROPERTY(int mousePosX READ mousePosX WRITE setMousePosX NOTIFY mousePosXChanged);
    Q_PROPERTY(int mousePosY READ mousePosY WRITE setMousePosY NOTIFY mousePosYChanged);

    int mousePosX() const;
    void setMousePosX(int mousePosX);

    int mousePosY() const;
    void setMousePosY(int mousePosY);

    explicit LogSearcher(QObject *parent = nullptr);
    ~LogSearcher();

    // 设置 WId
    void setWId(WId winid);

protected:



signals:

    void mousePosXChanged(int);
    void mousePosYChanged(int);

private:

    // 鼠标相对于单个视图窗口在左上角的位置
    int m_mousePosX, m_mousePosY;

private:

    // 窗口 WId
    WId m_winId;

};

#endif // LOGSEARCHER_H
