#include "xmltagmatcher.h"

QHash<int, XmlTagInfo> XmlTagMatcher::computeTagPairs(const QString& text,
                                                       const QVector<XmlTagInfo>& allTags)
{
    QHash<int, XmlTagInfo> pairs;
    // 栈：存储 (标签名, 在 allTags 中的索引)
    QVector<QPair<QString, int>> tagStack;

    for (int i = 0; i < allTags.size(); ++i) {
        const XmlTagInfo& tag = allTags.at(i);

        if (tag.type == XmlTagType::XTT_Open) {
            tagStack.append(qMakePair(tag.name, i));
        }
        else if (tag.type == XmlTagType::XTT_Close) {
            // 从栈顶向下搜索同名开标签
            for (int j = tagStack.size() - 1; j >= 0; --j) {
                if (tagStack[j].first == tag.name) {
                    // 匹配成功
                    int openIdx = tagStack[j].second;
                    pairs.insert(allTags[openIdx].startPos, tag);
                    pairs.insert(tag.startPos, allTags[openIdx]);

                    // 移除该开标签，后面的 oprhan 标签保留（视为未闭合）
                    tagStack.remove(j);
                    break;
                }
            }
            // 如果没找到匹配，这是一个 orphan 闭标签，忽略
        }
        // 自闭合标签不参与配对
    }

    return pairs;
}

XmlTagInfo XmlTagMatcher::findMatchingTag(const QString& text, int position)
{
    if (text.isEmpty() || position < 0) {
        return XmlTagInfo();
    }

    // 1. 在 position 处解析标签
    XmlTagInfo tag = XmlTagParser::parseTagAt(text, position);

    // 2. 如果 position 不在标签上，尝试查找包围它的开标签
    if (!tag.isValid() || tag.type == XmlTagType::XTT_None) {
        tag = getEnclosingOpenTag(text, position);
    }

    // 3. 自闭合标签没有配对
    if (!tag.isValid() || tag.type == XmlTagType::XTT_SelfClosing
        || tag.type == XmlTagType::XTT_None) {
        return XmlTagInfo();
    }

    // 4. 解析全部标签并计算配对
    QVector<XmlTagInfo> allTags = XmlTagParser::parseAllTags(text);
    QHash<int, XmlTagInfo> pairs = computeTagPairs(text, allTags);

    auto it = pairs.constFind(tag.startPos);
    if (it != pairs.constEnd()) {
        return it.value();
    }

    return XmlTagInfo();
}

QVector<XmlTagInfo> XmlTagMatcher::getUnmatchedOpenTags(const QString& text, int cursorPos)
{
    QVector<XmlTagInfo> unmatched;
    if (text.isEmpty()) return unmatched;

    QVector<XmlTagInfo> allTags = XmlTagParser::parseAllTags(text);

    // 只处理光标之前的标签
    QVector<QPair<QString, XmlTagInfo>> tagStack; // (name, tagInfo)

    for (int i = 0; i < allTags.size(); ++i) {
        const XmlTagInfo& tag = allTags.at(i);

        // 只处理光标之前的标签
        if (tag.endPos > cursorPos) break;

        if (tag.type == XmlTagType::XTT_Open) {
            tagStack.append(qMakePair(tag.name, tag));
        }
        else if (tag.type == XmlTagType::XTT_Close) {
            // 从栈顶向下搜索匹配
            for (int j = tagStack.size() - 1; j >= 0; --j) {
                if (tagStack[j].first == tag.name) {
                    tagStack.remove(j);
                    break;
                }
            }
        }
        // 自闭合跳过
    }

    // 栈中剩下的就是未闭合的开标签
    for (const auto& item : tagStack) {
        unmatched.append(item.second);
    }

    return unmatched;
}

XmlTagInfo XmlTagMatcher::getEnclosingOpenTag(const QString& text, int cursorPos)
{
    if (text.isEmpty() || cursorPos < 0) return XmlTagInfo();

    QVector<XmlTagInfo> allTags = XmlTagParser::parseAllTags(text);

    // 使用栈跟踪，找出包含 cursorPos 的最内层开标签
    QVector<QPair<QString, XmlTagInfo>> tagStack;

    for (int i = 0; i < allTags.size(); ++i) {
        const XmlTagInfo& tag = allTags.at(i);

        // 只处理标签开始位置在光标之前的标签
        if (tag.startPos > cursorPos) break;

        if (tag.type == XmlTagType::XTT_Open) {
            tagStack.append(qMakePair(tag.name, tag));
        }
        else if (tag.type == XmlTagType::XTT_Close) {
            // 检查该闭标签是否在光标之后（标签对包围了光标）
            if (tag.endPos > cursorPos) {
                // 闭标签在光标之后 → 标签对包围了光标
                // 检查栈顶是否有匹配
                for (int j = tagStack.size() - 1; j >= 0; --j) {
                    if (tagStack[j].first == tag.name) {
                        // 如果栈顶匹配且包含光标，返回该开标签
                        if (tagStack[j].second.startPos < cursorPos) {
                            return tagStack[j].second;
                        }
                        tagStack.remove(j);
                        break;
                    }
                }
            }
            else {
                // 闭标签在光标之前 → 正常出栈
                for (int j = tagStack.size() - 1; j >= 0; --j) {
                    if (tagStack[j].first == tag.name) {
                        tagStack.remove(j);
                        break;
                    }
                }
            }
        }
    }

    // 返回最内层的开标签（栈顶）
    if (!tagStack.isEmpty()) {
        return tagStack.last().second;
    }

    return XmlTagInfo();
}

int XmlTagMatcher::findTagIndexAtPos(const QVector<XmlTagInfo>& allTags, int position)
{
    for (int i = 0; i < allTags.size(); ++i) {
        const XmlTagInfo& tag = allTags.at(i);
        if (position >= tag.startPos && position <= tag.endPos) {
            return i;
        }
    }
    return -1;
}

// 基于结构（嵌套层级）查找匹配标签，不依赖标签名相同
// 这样即使用户修改了标签名，也能找到对应的匹配标签
XmlTagInfo XmlTagMatcher::findMatchingTagByStructure(const QString& text,
                                                      int cursorPos)
{
    if (text.isEmpty() || cursorPos < 0) return XmlTagInfo();

    // 查找光标所在的标签
    XmlTagInfo cursorTag = XmlTagParser::parseTagAt(text, cursorPos);
    if (!cursorTag.isValid()) {
        cursorTag = getEnclosingOpenTag(text, cursorPos);
    }
    if (!cursorTag.isValid()) return XmlTagInfo();

    if (cursorTag.type != XmlTagType::XTT_Open && cursorTag.type != XmlTagType::XTT_Close) {
        return XmlTagInfo();
    }

    // 解析所有标签
    QVector<XmlTagInfo> allTags = XmlTagParser::parseAllTags(text);

    // 找到光标标签在 allTags 中的索引
    int cursorIdx = findTagIndexAtPos(allTags, cursorTag.startPos);
    if (cursorIdx < 0) return XmlTagInfo();

    if (cursorTag.type == XmlTagType::XTT_Open) {
        // 向后搜索匹配的闭标签（按嵌套层级，不按名称）
        int depth = 1; // 当前开标签贡献 1 层
        for (int i = cursorIdx + 1; i < allTags.size(); ++i) {
            const XmlTagInfo& tag = allTags.at(i);
            if (tag.type == XmlTagType::XTT_Open) {
                depth++;
            } else if (tag.type == XmlTagType::XTT_Close) {
                depth--;
                if (depth == 0) {
                    return tag; // 找到匹配的闭标签
                }
            }
            // 自闭合标签不影响层级
        }
    } else {
        // 向前搜索匹配的开标签（按嵌套层级，不按名称）
        int depth = 1; // 当前闭标签贡献 1 层
        for (int i = cursorIdx - 1; i >= 0; --i) {
            const XmlTagInfo& tag = allTags.at(i);
            if (tag.type == XmlTagType::XTT_Close) {
                depth++;
            } else if (tag.type == XmlTagType::XTT_Open) {
                depth--;
                if (depth == 0) {
                    return tag; // 找到匹配的开标签
                }
            }
        }
    }

    return XmlTagInfo();
}
