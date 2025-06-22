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

bool LogUtils::ConvertHTML(const QString& normalLine,
                           int& beginIndex,
                           int& endIndex,
                           QColor& color,
                           const QString& specifiedFindTarget /*= ""*/)
{
    QMap<QString, QString> formatedSearchTarget = m_formatedSearchTarget;
    if(!specifiedFindTarget.isEmpty())
    {
        formatedSearchTarget = { { specifiedFindTarget, "#FF0000" } };
    }

    for(auto iter = formatedSearchTarget.begin(); iter != formatedSearchTarget.end(); ++iter)
    {
        beginIndex = normalLine.indexOf(iter.key());
        if(beginIndex >= 0)
        {
            endIndex = beginIndex + iter.key().size();
            color = iter.value();
            return true;
        }
    }

    beginIndex = endIndex = -1;

    return false;
}

bool LogUtils::ConvertHTML(const QString& normalLine,
                           const QString& specifiedFindTarget,
                           QString& htmlLine)
{
    htmlLine = normalLine;

    int beginPos;
    int endPos;
    QColor color;
    const bool&& containKeyword = LogUtils::ConvertHTML(htmlLine, beginPos, endPos, color, specifiedFindTarget);

    if(containKeyword)
    {
        htmlLine.insert(endPos, QString("%1").arg("</b></font>"));
        htmlLine.insert(beginPos, QString("<font color='%1'><b>").arg("#FF0000"));
        htmlLine = QString("<font color='#000000'>%1</font>").arg(htmlLine);
    }
    else
    {
        htmlLine = QString("<font color='#000000'>%1</font>").arg(htmlLine);
    }

    return containKeyword;
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
