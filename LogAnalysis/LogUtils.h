#ifndef LOGUTILS_H
#define LOGUTILS_H

#include <QMap>
#include <QColor>

class LogUtils
{
public:

    // 关键字对象
    static QMap<int, QPair<QString, QString>>& Keywords();

    // 格式化关键字对象
    static QMap<QString, QString> FormatedKeywordMap();

    // 将普通字符串转转化为HTML字符串
    static bool ConvertHTML(const QString& normalLine, QString& htmlLine, const QString findTarget = "");

    // 生成随机色
    static QColor GenerateRandomColorRGB_Safe();

private:

    // 查询关键字、关键字索引及对应前景色
    static QMap<int, QPair<QString, QString>> m_searchTarget;

    // 格式转换
    static QMap<QString, QString> m_formatedSearchTarget;

};

#endif // LOGUTILS_H
