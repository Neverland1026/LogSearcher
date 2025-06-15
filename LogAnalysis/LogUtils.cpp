#include "LogUtils.h"
#include <random>

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
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    const int random_number_1 = dis(gen);
    const int random_number_2 = dis(gen);
    const int random_number_3 = dis(gen);

    QColor color;
    do {
        color = QColor(random_number_1 % 256, random_number_2 % 256, random_number_3 % 256);
    } while (color.red() > 230 && color.green() > 230 && color.blue() > 230);

    return color;
}

