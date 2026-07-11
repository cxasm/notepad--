#include "xmltaghighlighter.h"
#include "xmltagparser.h"
#include "xmltagmatcher.h"

#include <qsciscintilla.h>
#include <Scintilla.h>
#include "scidirectcall.h"

// indicator 编号：使用较高编号避免与主程序冲突（主程序使用 0-8 和容器分配）
static const int XML_TAG_HIGHLIGHT_INDIC = 20;

XmlTagHighlighter::XmlTagHighlighter(QObject* parent)
    : QObject(parent)
    , m_indicatorNumber(XML_TAG_HIGHLIGHT_INDIC)
    , m_highlightColor(100, 149, 237, 80) // 半透明蓝色 (cornflower blue)
{
}

XmlTagHighlighter::~XmlTagHighlighter()
{
}

void XmlTagHighlighter::initIndicator(QsciScintilla* editor)
{
    if (!editor) return;

    // 使用直接 Scintilla API 设置 indicator 颜色和样式
    const int rgb = (m_highlightColor.red())
                  | (m_highlightColor.green() << 8)
                  | (m_highlightColor.blue() << 16);
    sciDirectCall(editor, SCI_INDICSETFORE, m_indicatorNumber, rgb);
    sciDirectCall(editor, SCI_INDICSETALPHA, m_indicatorNumber, m_highlightColor.alpha());

    // 通过 Scintilla API 设置 indicator 样式为 INDIC_STRAIGHTBOX (8)
    sciDirectCall(editor, SCI_INDICSETSTYLE, m_indicatorNumber, 8);   // INDIC_STRAIGHTBOX
    sciDirectCall(editor, SCI_INDICSETALPHA, m_indicatorNumber, 80);  // 半透明度
    sciDirectCall(editor, SCI_INDICSETOUTLINEALPHA, m_indicatorNumber, 120); // 边框不透明度
    sciDirectCall(editor, SCI_INDICSETUNDER, m_indicatorNumber, 1);   // 绘制在文本下方
}

void XmlTagHighlighter::highlightPair(QsciScintilla* editor,
                                       const XmlTagInfo& tag1,
                                       const XmlTagInfo& tag2)
{
    if (!editor) return;

    clearHighlight(editor);

    fillTagIndicator(editor, tag1);
    fillTagIndicator(editor, tag2);

    m_lastEditor = editor;
    m_lastTag1 = tag1;
    m_lastTag2 = tag2;
}

void XmlTagHighlighter::clearHighlight(QsciScintilla* editor)
{
    if (!editor) return;

    if (!m_lastEditor.isNull() && m_lastEditor.data() == editor) {
        if (m_lastTag1.isValid())
            clearTagIndicator(editor, m_lastTag1);
        if (m_lastTag2.isValid())
            clearTagIndicator(editor, m_lastTag2);
    }

    m_lastTag1 = XmlTagInfo();
    m_lastTag2 = XmlTagInfo();
}

bool XmlTagHighlighter::tryHighlightAtCursor(QsciScintilla* editor)
{
    if (!editor) return false;

    int cursorPos = static_cast<int>(sciDirectCall(editor, SCI_GETCURRENTPOS));
    QString text = sciGetText(editor);

    XmlTagInfo tag = XmlTagParser::parseTagAt(text, cursorPos);
    if (!tag.isValid() || tag.type == XmlTagType::XTT_SelfClosing) {
        clearHighlight(editor);
        return false;
    }

    XmlTagInfo matching = XmlTagMatcher::findMatchingTag(text, cursorPos);
    if (!matching.isValid()) {
        clearHighlight(editor);
        return false;
    }

    highlightPair(editor, tag, matching);
    return true;
}

void XmlTagHighlighter::fillTagIndicator(QsciScintilla* editor, const XmlTagInfo& tag)
{
    if (!editor || !tag.isValid()) return;

    // 高亮整个标签（从 '<' 到 '>'）
    sciDirectCall(editor, SCI_SETINDICATORCURRENT, m_indicatorNumber);
    sciDirectCall(editor, SCI_INDICATORFILLRANGE, tag.startPos, tag.endPos - tag.startPos);
}

void XmlTagHighlighter::clearTagIndicator(QsciScintilla* editor, const XmlTagInfo& tag)
{
    if (!editor || !tag.isValid()) return;

    sciDirectCall(editor, SCI_SETINDICATORCURRENT, m_indicatorNumber);
    sciDirectCall(editor, SCI_INDICATORCLEARRANGE, tag.startPos, tag.endPos - tag.startPos);
}
