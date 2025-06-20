#include "LogUtils.h"
#include <QRandomGenerator>

QMap<int, QPair<QString, QString>> LogUtils::m_searchTarget = {};
QMap<QString, QString> LogUtils::m_formatedSearchTarget = {};

QMap<int, QPair<QString, QString>>& LogUtils::Keywords()
{
    return m_searchTarget;
}

QMap<QString, QString> LogUtils::FormatedKeywordMap()
{
    m_formatedSearchTarget.clear();
    for(auto iter = m_searchTarget.begin(); iter != m_searchTarget.end(); ++iter)
    {
        if(!iter.value().first.isEmpty())
        {
            m_formatedSearchTarget[iter.value().first] = iter.value().second;
        }
    }

    return m_formatedSearchTarget;
}

bool LogUtils::ConvertHTML(const QString& normalLine, QString& htmlLine, const QString findTarget /*= ""*/)
{
    FormatedKeywordMap();

    htmlLine = normalLine;

    QString keyword = "";
    QString color = "";
    int insertIndex = -1;

    if(findTarget.isEmpty())
    {
        for(auto iter = m_formatedSearchTarget.begin(); iter != m_formatedSearchTarget.end(); ++iter)
        {
            insertIndex = htmlLine.indexOf(iter.key());
            if(insertIndex >= 0)
            {
                keyword = iter.key();
                color = iter.value();
                break;
            }
        }
    }
    else
    {
        insertIndex = htmlLine.indexOf(findTarget);
        keyword = findTarget;
        color = "#FF0000";
    }

    if(insertIndex < 0)
    {
        htmlLine = QString("<font color='#000000'>%1</font>").arg(htmlLine);
        return false;
    }

    htmlLine.insert(insertIndex + keyword.size(), QString("%1").arg("</b></font>"));
    htmlLine.insert(insertIndex, QString("<font color='%1'><b>").arg(color));
    htmlLine = QString("<font color='#000000'>%1</font>").arg(htmlLine);

    return true;
}

QColor LogUtils::GenerateRandomColorRGB_Safe()
{
    QRandomGenerator* random = QRandomGenerator::global();

    // 确保至少一个通道接近255，其他通道较低
    int mainChannel = random->bounded(3);
    int colors[3] = {0};

    colors[mainChannel] = random->bounded(56) + 200;  // 200-255

    // 其他两个通道在0-100之间
    for(int i = 0; i < 3; i++) {
        if(i != mainChannel) {
            colors[i] = random->bounded(101);
        }
    }

    return QColor(colors[0], colors[1], colors[2]);
}
