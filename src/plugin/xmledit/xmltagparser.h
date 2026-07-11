#pragma once

#include <QString>
#include <QVector>
#include <QRegularExpression>

// XML 标签类型枚举（定义在结构体外，避免 MSVC 解析问题）
enum class XmlTagType {
    XTT_Open,        // <name ...>
    XTT_Close,       // </name>
    XTT_SelfClosing, // <name ... />
    XTT_None         // 无效/非标签
};

// XML 标签信息结构体
struct XmlTagInfo {
    XmlTagType type = XmlTagType::XTT_None;
    QString name;     // 标签名，如 "xs:element"
    int startPos = -1;     // '<' 在文档中的字符位置 (0-based)
    int endPos = -1;       // '>' 在文档中的字符位置
    int nameStartPos = -1; // 标签名起始字符位置
    int nameEndPos = -1;   // 标签名结束字符位置

    bool isValid() const { return type != XmlTagType::XTT_None && startPos >= 0; }
};

// XML 标签解析器（纯算法类，无 Qt 继承）
class XmlTagParser
{
public:
    // 正则：匹配开标签、闭标签、自闭合标签
    // 组1: '/' (闭标签标志)
    // 组2: 标签名 (支持命名空间: a-z, A-Z, 0-9, _, :, ., -)
    // 组3: 属性部分 (非贪婪)
    // 组4: '/' (自闭合标志，在 > 前面)
    static QRegularExpression tagRegex();

    // 从 position 位置开始向前/后搜索最近的标签并解析
    // position 是文档中的字符位置
    static XmlTagInfo parseTagAt(const QString& text, int position);

    // 解析文档中的所有 XML 标签
    static QVector<XmlTagInfo> parseAllTags(const QString& text);

    // 判断光标位置是否在标签名内
    static bool isCursorInTagName(const XmlTagInfo& tag, int cursorPos);

private:
    static XmlTagInfo parseTagFromMatch(const QRegularExpressionMatch& match);
};
