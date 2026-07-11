#pragma once

#include <QObject>
#include <QColor>
#include <QPointer>

#include "xmltagparser.h"

class QsciScintilla;

// 基于 Scintilla Indicator 的标签对高亮
class XmlTagHighlighter : public QObject
{
    Q_OBJECT

public:
    explicit XmlTagHighlighter(QObject* parent = nullptr);
    ~XmlTagHighlighter();

    // 在编辑器上初始化 indicator（只需调用一次）
    void initIndicator(QsciScintilla* editor);

    // 高亮显示一对标签
    void highlightPair(QsciScintilla* editor,
                       const XmlTagInfo& tag1,
                       const XmlTagInfo& tag2);

    // 清除编辑器上的高亮
    void clearHighlight(QsciScintilla* editor);

    // 尝试在光标位置查找并高亮匹配标签对
    // 返回 true 表示找到并高亮了匹配对
    bool tryHighlightAtCursor(QsciScintilla* editor);

    // 获取 indicator 编号
    int indicatorNumber() const { return m_indicatorNumber; }

private:
    // 在编辑器上填充 indicator 范围
    void fillTagIndicator(QsciScintilla* editor, const XmlTagInfo& tag);

    // 清除编辑器上指定标签范围的 indicator
    void clearTagIndicator(QsciScintilla* editor, const XmlTagInfo& tag);

    int m_indicatorNumber;
    QColor m_highlightColor;
    QPointer<QsciScintilla> m_lastEditor;
    XmlTagInfo m_lastTag1;
    XmlTagInfo m_lastTag2;
};
