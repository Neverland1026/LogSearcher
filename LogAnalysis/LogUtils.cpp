#include "LogUtils.h"
#include <QRandomGenerator>

QMap<int, QPair<QString, QString>> LogUtils::m_searchTarget = {};
QMap<QString, QPair<int, QString>> LogUtils::m_transformedSearchTarget = {};
QVector<LogUtils::LineInfo> LogUtils::m_keyLineInfos = {};

QMap<int, QPair<QString, QString>>& LogUtils::Keywords()
{
    return m_searchTarget;
}

void LogUtils::FormatedKeywordMap()
{
    m_transformedSearchTarget.clear();
    for(auto iter = m_searchTarget.begin(); iter != m_searchTarget.end(); ++iter)
    {
        m_transformedSearchTarget[iter.value().first] = { iter.key(), iter.value().second };
    }
}

QVector<LogUtils::LineInfo>& LogUtils::KeyLineInfos()
{
    return m_keyLineInfos;
}

bool LogUtils::ConvertHTML(const QString& normalLine,
                           int& keywordIndex,
                           int& beginIndex,
                           int& endIndex,
                           const QString& specifiedFindTarget /*= ""*/)
{
    auto transformedSearchTarget = m_transformedSearchTarget;
    if(!specifiedFindTarget.isEmpty())
    {
        transformedSearchTarget = { { specifiedFindTarget, { -1, "#FF0000" } } };
    }

    for(auto iter = transformedSearchTarget.begin(); iter != transformedSearchTarget.end(); ++iter)
    {
        if(iter.key().isEmpty())
            continue;

        beginIndex = normalLine.indexOf(iter.key());
        if(beginIndex >= 0)
        {
            keywordIndex = iter.value().first;
            endIndex = beginIndex + iter.key().size();
            return true;
        }
    }

    beginIndex = endIndex = keywordIndex = -1;

    return false;
}

bool LogUtils::ConvertHTML(const QString& normalLine,
                           const QString& specifiedFindTarget,
                           QString& htmlLine)
{
    htmlLine = normalLine;

    int keywordIndex;
    int beginPos;
    int endPos;
    const bool&& containKeyword = LogUtils::ConvertHTML(htmlLine, keywordIndex, beginPos, endPos, specifiedFindTarget);

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
