#pragma once

#include "xmltagparser.h"
#include <QString>
#include <QVector>
#include <QHash>

// XML 标签配对匹配器（栈算法）
class XmlTagMatcher
{
public:
    // 查找与 position 处标签配对的标签
    static XmlTagInfo findMatchingTag(const QString& text, int position);

    // 基于结构（嵌套层级）查找匹配标签，不依赖标签名相同
    // 用于自动重命名：用户修改标签名后仍能找到对应标签
    static XmlTagInfo findMatchingTagByStructure(const QString& text, int cursorPos);

    // 获取光标位置之前所有未闭合的开标签列表
    static QVector<XmlTagInfo> getUnmatchedOpenTags(const QString& text, int cursorPos);

    // 获取包围给定位置的最内层开标签
    static XmlTagInfo getEnclosingOpenTag(const QString& text, int cursorPos);

private:
    // 核心栈算法：处理所有标签，返回 tag.startPos → 配对 tag 的映射
    static QHash<int, XmlTagInfo> computeTagPairs(const QString& text,
                                                   const QVector<XmlTagInfo>& allTags);

    // 在 allTags 中查找包含 position 的标签
    static int findTagIndexAtPos(const QVector<XmlTagInfo>& allTags, int position);
};
