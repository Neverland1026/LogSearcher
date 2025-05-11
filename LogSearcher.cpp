#include "LogSearcher.h"
#include <windows.h>

int LogSearcher::mousePosX() const
{
    return m_mousePosX;
}

void LogSearcher::setMousePosX(int mousePosX)
{
    if (mousePosX != m_mousePosX) {
        m_mousePosX = mousePosX;
        emit mousePosXChanged(m_mousePosX);
    }
}

int LogSearcher::mousePosY() const
{
    return m_mousePosY;
}

void LogSearcher::setMousePosY(int mousePosY)
{
    if (mousePosY != m_mousePosY) {
        m_mousePosY = mousePosY;
        emit mousePosYChanged(m_mousePosY);
    }
}

LogSearcher::LogSearcher(QObject *parent /*= nullptr*/)
    : QObject{parent}
    , m_mousePosX(0)
    , m_mousePosY(0)
{

}

LogSearcher::~LogSearcher()
{
}

void LogSearcher::setWId(WId winid)
{
    m_winId = winid;

    ::SetWindowPos((HWND)(/*this->winId()*/m_winId),
                   HWND_TOPMOST, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}
