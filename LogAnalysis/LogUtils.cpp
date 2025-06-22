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
    static QVector<QColor> colors = {};
    if(colors.isEmpty())
    {
        const float goldenRatio = 0.618033988749895f; // 黄金比例
        float hue = QRandomGenerator::global()->generateDouble(); // 随机起点

        for(int i = 0; i < 50; i++) {
            hue += goldenRatio;
            hue = std::fmod(hue, 1.0f);
            colors.append(QColor::fromHsvF(
                hue,                                                      // 色相
                0.7f + QRandomGenerator::global()->generateDouble()*0.3f, // 饱和度70-100%
                0.8f + QRandomGenerator::global()->generateDouble()*0.2f  // 亮度80-100%
                ));
        }
    }

    static int generateCount = 0;

    return colors[generateCount++ % colors.size()];
}
