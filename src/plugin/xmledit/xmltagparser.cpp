#include "xmltagparser.h"

QRegularExpression XmlTagParser::tagRegex()
{
    // 匹配 XML 标签的正则表达式
    // <                    -- 开尖括号
    // (\/?)                -- 组1: 可选 '/' (闭标签标志)
    // \s*                  -- 可选空白
    // ([\w:.\-]+)          -- 组2: 标签名 (字母数字下划线, 命名空间 : ., 连字符 -)
    // ([^>]*?)             -- 组3: 属性部分 (非贪婪匹配)
    // (\/?)                -- 组4: 可选 '/' (自闭合标志, 在 > 前)
    // \s*>                 -- 可选空白后跟 >
    static QRegularExpression regex(
        QStringLiteral("<(\\/?)\\s*([\\w:.\\-]+)([^>]*?)(\\/?)\\s*>"),
        QRegularExpression::MultilineOption
    );
    regex.optimize();
    return regex;
}

XmlTagInfo XmlTagParser::parseTagFromMatch(const QRegularExpressionMatch& match)
{
    XmlTagInfo info;
    if (!match.hasMatch()) {
        return info;
    }

    QString slash1 = match.captured(1);   // '/' for close tags
    info.name = match.captured(2);         // tag name
    // captured(3) is attributes (not used for basic matching)
    QString slash2 = match.captured(4);   // '/' for self-closing

    info.startPos = match.capturedStart(0);
    info.endPos = match.capturedEnd(0);
    info.nameStartPos = match.capturedStart(2);
    info.nameEndPos = match.capturedEnd(2);

    if (!slash1.isEmpty() && slash1 == QStringLiteral("/")) {
        info.type = XmlTagType::XTT_Close;
    } else if (!slash2.isEmpty() && slash2 == QStringLiteral("/")) {
        info.type = XmlTagType::XTT_SelfClosing;
    } else {
        info.type = XmlTagType::XTT_Open;
    }

    return info;
}

XmlTagInfo XmlTagParser::parseTagAt(const QString& text, int position)
{
    if (text.isEmpty() || position < 0 || position > text.length()) {
        return XmlTagInfo();
    }

    QRegularExpression regex = tagRegex();
    QRegularExpressionMatchIterator it = regex.globalMatch(text);

    XmlTagInfo bestTag;
    int bestDist = INT_MAX;

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        int tagStart = match.capturedStart(0);
        int tagEnd = match.capturedEnd(0);

        // 检查 position 是否在此标签范围内
        if (position >= tagStart && position <= tagEnd) {
            return parseTagFromMatch(match);
        }

        // 如果 position 不在任何标签内，找最近的标签
        int dist;
        if (position < tagStart) {
            dist = tagStart - position; // 在标签前面
        } else {
            dist = position - tagEnd;   // 在标签后面
        }

        if (dist < bestDist) {
            bestDist = dist;
            bestTag = parseTagFromMatch(match);
        }
    }

    return bestTag;
}

QVector<XmlTagInfo> XmlTagParser::parseAllTags(const QString& text)
{
    QVector<XmlTagInfo> tags;
    if (text.isEmpty()) {
        return tags;
    }

    QRegularExpression regex = tagRegex();
    QRegularExpressionMatchIterator it = regex.globalMatch(text);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        XmlTagInfo info = parseTagFromMatch(match);

        // 跳过：
        // 1. 以 <? 开头的 XML 声明/处理指令
        // 2. 以 <! 开头的 DOCTYPE/CDATA/注释
        if (info.isValid()) {
            int checkPos = info.startPos;
            if (checkPos + 1 < text.length()) {
                QChar nextChar = text.at(checkPos + 1);
                if (nextChar == QLatin1Char('?') || nextChar == QLatin1Char('!')) {
                    continue; // 跳过处理指令和声明
                }
            }
            tags.append(info);
        }
    }

    return tags;
}

bool XmlTagParser::isCursorInTagName(const XmlTagInfo& tag, int cursorPos)
{
    if (!tag.isValid()) return false;
    return (cursorPos >= tag.nameStartPos && cursorPos <= tag.nameEndPos);
}
