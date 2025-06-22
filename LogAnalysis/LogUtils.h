#ifndef LOGUTILS_H
#define LOGUTILS_H

#include <QMap>
#include <QColor>

class LogUtils
{
public:

    // 检索到关键字的行信息
    struct LineInfo
    {
        int lineIndex = -1;
        QString line = "";
        int keywordIndex = -1;
        int beginPos = -1;
        int endPos = -1;

        LineInfo()
        {
            this->lineIndex = -1;
            this->line = "";
            this->keywordIndex = this->beginPos = this->endPos = -1;
        }
        LineInfo(const int lineIndex, const QString& line, const int keywordIndex, const int beginPos, const int endPos)
        {
            this->lineIndex = lineIndex;
            this->line = line;
            this->keywordIndex = keywordIndex;
            this->beginPos = beginPos;
            this->endPos = endPos;
        }
        QString colorful()
        {
            if(beginPos < 0)
                return QString("<font color='#000000'>%1</font>").arg(this->line);

            QString dstLine = this->line;
            dstLine.insert(endPos, QString("%1").arg("</b></font>"));
            dstLine.insert(beginPos, QString("<font color='%1'><b>").arg(LogUtils::Keywords()[this->keywordIndex].second));
            dstLine = QString("<font color='#000000'>%1</font>").arg(dstLine);

            return dstLine;
        }
    };

    // 关键字对象
    static QMap<int, QPair<QString, QString>>& Keywords();

    // 格式化关键字对象
    static void FormatedKeywordMap();

    // 将普通字符串转转化为HTML字符串
    static bool ConvertHTML(const QString& normalLine,
                            int& keywordIndex,
                            int& beginIndex,
                            int& endIndex,
                            const QString& specifiedFindTarget = "");
    static bool ConvertHTML(const QString& normalLine,
                            const QString& specifiedFindTarget,
                            QString& htmlLine);

    // 获取检索到关键字的行信息
    static QVector<LogUtils::LineInfo>& KeyLineInfos();

    // 生成随机色
    static QColor GenerateRandomColorRGB_Safe();

private:

    // 查询关键字、关键字索引及对应前景色
    static QMap<int, QPair<QString, QString>> m_searchTarget;

    // 格式转换
    static QMap<QString, QPair<int, QString>> m_transformedSearchTarget;

    // 检索到关键字的行信息
    static QVector<LineInfo> m_keyLineInfos;

};

#endif // LOGUTILS_H
