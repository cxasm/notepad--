#include "xmleditplugin.h"
#include "xmltagparser.h"
#include "xmltagmatcher.h"
#include "xmltaghighlighter.h"

#include <qsciscintilla.h>
#include <Scintilla.h>
// Scintilla.h 中的 SCN_* 宏与 QScintilla 信号名冲突，需要 #undef 以使用新式 connect
#undef SCN_CHARADDED
#undef SCN_MODIFIED
#include <QMenu>
#include <QTabWidget>
#include <QKeySequence>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSet>
#include "scidirectcall.h"

// 调试日志（开发时取消注释，发布时注释掉）
//#define XMLEDIT_DEBUG_LOG

#ifdef XMLEDIT_DEBUG_LOG
static QFile s_logFile;
static void debugLog(const QString& msg)
{
    if (!s_logFile.isOpen()) {
        s_logFile.setFileName(QStringLiteral("xmledit_debug.log"));
        s_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }
    if (s_logFile.isOpen()) {
        QTextStream ts(&s_logFile);
        ts << QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss.zzz"))
           << QStringLiteral(" [") << msg << QStringLiteral("]\n");
        ts.flush();
    }
}
#else
#define debugLog(msg) ((void)0)
#endif

// 大文件阈值（字节）：超过此大小的文档不进行全文档标签解析
static const int LARGE_FILE_THRESHOLD = 10 * 1024 * 1024; // 10MB

// 检查编辑器是否只读
static bool isEditorReadOnly(QsciScintilla* editor)
{
    if (!editor) return true;
    return sciDirectCall(editor, SCI_GETREADONLY) != 0;
}

XmlEditPlugin::XmlEditPlugin(QWidget* pNotepad,
                               std::function<QsciScintilla*(QWidget*)> getCurEdit,
                               NDD_PROC_DATA* pProcData,
                               QObject* parent)
    : QObject(parent)
    , m_pNotepad(pNotepad)
    , m_rootMenu(nullptr)
    , m_getCurEdit(getCurEdit)
    , m_currentEditor(nullptr)
    , m_highlighter(nullptr)
    , m_renameDebounceTimer(nullptr)
    , m_autoCloseEnabled(true)
    , m_autoRenameEnabled(true)
    , m_highlightEnabled(true)
    , m_actionJumpToMatch(nullptr)
    , m_actionAutoClose(nullptr)
    , m_actionAutoRename(nullptr)
    , m_actionHighlightPair(nullptr)
    , m_jumpFlashTimer(nullptr)
    , m_jumpFlashCount(0)
    , m_jumpTargetStart(-1)
    , m_jumpTargetEnd(-1)
    , m_initRetryTimer(nullptr)
    , m_initRetryCount(0)
    , m_editingTagStart(-1)
    , m_isRenaming(false)
{
    debugLog(QStringLiteral("XmlEditPlugin constructor"));

    // 创建高亮器
    m_highlighter = new XmlTagHighlighter(this);

    // 创建防抖定时器（300ms 单次触发）
    m_renameDebounceTimer = new QTimer(this);
    m_renameDebounceTimer->setSingleShot(true);
    m_renameDebounceTimer->setInterval(300);
    connect(m_renameDebounceTimer, &QTimer::timeout,
            this, &XmlEditPlugin::performAutoRename);

    // 创建跳转闪烁定时器（80ms 间隔，闪烁2次）
    m_jumpFlashTimer = new QTimer(this);
    m_jumpFlashTimer->setSingleShot(false);
    m_jumpFlashTimer->setInterval(80);
    connect(m_jumpFlashTimer, &QTimer::timeout, this, [this]() {
        if (!m_currentEditor) {
            m_jumpFlashTimer->stop();
            return;
        }
        m_jumpFlashCount++;
        if (m_jumpFlashCount > 4) { // 选中-取消-选中-取消 = 4 次
            m_jumpFlashTimer->stop();
            sciDirectCall(m_currentEditor, SCI_SETSELECTIONSTART, m_jumpTargetStart);
            sciDirectCall(m_currentEditor, SCI_SETSELECTIONEND, m_jumpTargetStart);
            return;
        }
        if (m_jumpFlashCount % 2 == 1) {
            // 选中目标标签
            sciDirectCall(m_currentEditor, SCI_SETSELECTIONSTART, m_jumpTargetStart);
            sciDirectCall(m_currentEditor, SCI_SETSELECTIONEND, m_jumpTargetEnd);
        } else {
            // 取消选中
            sciDirectCall(m_currentEditor, SCI_SETSELECTIONSTART, m_jumpTargetStart);
            sciDirectCall(m_currentEditor, SCI_SETSELECTIONEND, m_jumpTargetStart);
        }
    });

    // 创建菜单
    createMenu(pProcData);

    // 创建初始化重试定时器：编辑器可能尚未就绪，延迟重试安装监听器
    m_initRetryTimer = new QTimer(this);
    m_initRetryTimer->setSingleShot(true);
    m_initRetryTimer->setInterval(200);
    connect(m_initRetryTimer, &QTimer::timeout, this, [this]() {
        // 如果已有编辑器且监听器已安装，退出重试
        if (!m_currentEditor.isNull()) {
            return;
        }
        updateCurrentEditor();
        if (m_currentEditor) {
            installEditorListeners(m_currentEditor);
            return;
        }
        // 最多重试 10 次（2秒），之后停止
        if (++m_initRetryCount < 10) {
            m_initRetryTimer->start();
        }
    });

    // 设置 Tab 切换监听
    setupTabMonitoring();
}

XmlEditPlugin::~XmlEditPlugin()
{
    cleanup();
}

void XmlEditPlugin::cleanup()
{
    if (m_renameDebounceTimer) {
        m_renameDebounceTimer->stop();
    }
    if (m_jumpFlashTimer) {
        m_jumpFlashTimer->stop();
    }
    if (m_initRetryTimer) {
        m_initRetryTimer->stop();
    }
    if (!m_currentEditor.isNull()) {
        uninstallEditorListeners(m_currentEditor.data());
    }
    if (m_highlighter && !m_currentEditor.isNull()) {
        m_highlighter->clearHighlight(m_currentEditor.data());
    }
}

void XmlEditPlugin::createMenu(NDD_PROC_DATA* pProcData)
{
    if (!pProcData || !pProcData->m_rootMenu) return;

    m_rootMenu = pProcData->m_rootMenu;

    // 1. 跳转到匹配标签
    m_actionJumpToMatch = new QAction(tr("Jump to Matching Tag"), m_rootMenu);
    m_actionJumpToMatch->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+J")));
    m_actionJumpToMatch->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(m_actionJumpToMatch, &QAction::triggered,
            this, &XmlEditPlugin::onActionJumpToMatch);
    m_rootMenu->addAction(m_actionJumpToMatch);

    m_rootMenu->addSeparator();

    // 2. 标签自动闭合（可勾选）
    m_actionAutoClose = new QAction(tr("Auto Close Tag"), m_rootMenu);
    m_actionAutoClose->setCheckable(true);
    m_actionAutoClose->setChecked(m_autoCloseEnabled);
    m_actionAutoClose->setToolTip(tr("Automatically complete closing tag when typing </"));
    connect(m_actionAutoClose, &QAction::toggled,
            this, &XmlEditPlugin::onActionAutoCloseToggled);
    m_rootMenu->addAction(m_actionAutoClose);

    // 3. 标签自动重命名（可勾选）
    m_actionAutoRename = new QAction(tr("Auto Rename Tag"), m_rootMenu);
    m_actionAutoRename->setCheckable(true);
    m_actionAutoRename->setChecked(m_autoRenameEnabled);
    m_actionAutoRename->setToolTip(tr("Synchronously update matching tag when renaming"));
    connect(m_actionAutoRename, &QAction::toggled,
            this, &XmlEditPlugin::onActionAutoRenameToggled);
    m_rootMenu->addAction(m_actionAutoRename);

    // 4. 匹配标签高亮（可勾选）
    m_actionHighlightPair = new QAction(tr("Highlight Matching Tag"), m_rootMenu);
    m_actionHighlightPair->setCheckable(true);
    m_actionHighlightPair->setChecked(m_highlightEnabled);
    m_actionHighlightPair->setToolTip(tr("Highlight matching tag pair when cursor is on a tag"));
    connect(m_actionHighlightPair, &QAction::toggled,
            this, &XmlEditPlugin::onActionHighlightToggled);
    m_rootMenu->addAction(m_actionHighlightPair);
}

void XmlEditPlugin::setupTabMonitoring()
{
    debugLog(QStringLiteral("setupTabMonitoring"));
    // 通过 objectName 查找 QTabWidget
    QTabWidget* tabWidget = m_pNotepad->findChild<QTabWidget*>(QStringLiteral("editTabWidget"));
    if (tabWidget) {
        debugLog(QStringLiteral("found editTabWidget, currentIndex=%1").arg(tabWidget->currentIndex()));
        connect(tabWidget, &QTabWidget::currentChanged,
                this, &XmlEditPlugin::onTabChanged, Qt::UniqueConnection);
        // 初始化当前 Tab
        onTabChanged(tabWidget->currentIndex());
    } else {
        // 回退方案：直接获取当前编辑器
        updateCurrentEditor();
        if (m_currentEditor) {
            installEditorListeners(m_currentEditor);
        }
    }

    // 如果未找到编辑器，启动延迟重试
    if (m_currentEditor.isNull() && m_initRetryTimer) {
        m_initRetryTimer->start();
    }
}

void XmlEditPlugin::updateCurrentEditor()
{
    if (!m_pNotepad) return;
    m_currentEditor = m_getCurEdit(m_pNotepad);
    debugLog(QStringLiteral("updateCurrentEditor: editor=%1").arg(
        m_currentEditor ? QStringLiteral("valid") : QStringLiteral("NULL")));
}

void XmlEditPlugin::installEditorListeners(QsciScintilla* editor)
{
    if (!editor) return;

    debugLog(QStringLiteral("installEditorListeners: editor=%1").arg(
        reinterpret_cast<quintptr>(editor), 0, 16));

    // SCN_CHARADDED: 字符输入通知（用于自动闭合检测）
    // 使用新式 connect 确保编译期类型检查
    connect(editor, QOverload<int>::of(&QsciScintilla::SCN_CHARADDED),
            this, &XmlEditPlugin::onCharAdded, Qt::UniqueConnection);

    // cursorPositionChanged: 光标移动（用于高亮更新）
    connect(editor, &QsciScintilla::cursorPositionChanged,
            this, &XmlEditPlugin::onCursorPositionChanged, Qt::UniqueConnection);

    // SCN_MODIFIED: 文本修改通知（用于自动重命名防抖触发）
    connect(editor, &QsciScintilla::SCN_MODIFIED,
            this, &XmlEditPlugin::onTextModified, Qt::UniqueConnection);

    // 初始化 indicator
    m_highlighter->initIndicator(editor);

    m_currentEditor = editor;
    m_cachedEditor = nullptr;
    m_cachedText.clear();
}

void XmlEditPlugin::uninstallEditorListeners(QsciScintilla* editor)
{
    if (!editor) return;

    disconnect(editor, QOverload<int>::of(&QsciScintilla::SCN_CHARADDED),
               this, &XmlEditPlugin::onCharAdded);
    disconnect(editor, &QsciScintilla::cursorPositionChanged,
               this, &XmlEditPlugin::onCursorPositionChanged);
    disconnect(editor, &QsciScintilla::SCN_MODIFIED,
               this, &XmlEditPlugin::onTextModified);

    m_highlighter->clearHighlight(editor);
}

// --- Tab 切换 ---

void XmlEditPlugin::onTabChanged(int index)
{
    Q_UNUSED(index);
    debugLog(QStringLiteral("onTabChanged: index=%1").arg(index));

    // 卸载旧编辑器的事件监听
    if (!m_currentEditor.isNull()) {
        uninstallEditorListeners(m_currentEditor.data());
    }

    // 安装新编辑器的事件监听
    updateCurrentEditor();
    if (m_currentEditor) {
        installEditorListeners(m_currentEditor);

        // 刷新高亮
        if (m_highlightEnabled) {
            m_highlighter->tryHighlightAtCursor(m_currentEditor);
        }
    }
}

// --- 菜单动作 ---

void XmlEditPlugin::onActionJumpToMatch()
{
    if (m_currentEditor.isNull()) {
        updateCurrentEditor();
    }
    if (m_currentEditor.isNull()) return;

    QString text = getEditorText(m_currentEditor);
    if (text.isEmpty()) return;

    int cursorPos = static_cast<int>(sciDirectCall(m_currentEditor, SCI_GETCURRENTPOS));

    XmlTagInfo matching = XmlTagMatcher::findMatchingTag(text, cursorPos);
    if (!matching.isValid()) return;

    // 跳转到匹配标签的起始位置
    sciDirectCall(m_currentEditor, SCI_GOTOPOS, matching.startPos);

    // 确保目标行可见
    int targetLine = static_cast<int>(sciDirectCall(m_currentEditor, 
        SCI_LINEFROMPOSITION, matching.startPos));
    sciDirectCall(m_currentEditor, SCI_ENSUREVISIBLE, targetLine);

    // 闪烁效果：短暂选中目标标签
    m_jumpTargetStart = matching.startPos;
    m_jumpTargetEnd = matching.endPos;
    m_jumpFlashCount = 0;
    m_jumpFlashTimer->start();
}

void XmlEditPlugin::onActionAutoCloseToggled(bool checked)
{
    m_autoCloseEnabled = checked;
}

void XmlEditPlugin::onActionAutoRenameToggled(bool checked)
{
    m_autoRenameEnabled = checked;
    if (!checked) {
        m_renameDebounceTimer->stop();
    }
}

void XmlEditPlugin::onActionHighlightToggled(bool checked)
{
    m_highlightEnabled = checked;
    if (!checked) {
        if (!m_currentEditor.isNull()) {
            m_highlighter->clearHighlight(m_currentEditor);
        }
    } else {
        if (!m_currentEditor.isNull()) {
            m_highlighter->tryHighlightAtCursor(m_currentEditor);
        }
    }
}

// --- 编辑器事件 ---

void XmlEditPlugin::onCharAdded(int charadded)
{
    debugLog(QStringLiteral("onCharAdded: char='%1' (0x%2) autoClose=%3")
        .arg(QChar(charadded)).arg(charadded, 0, 16).arg(m_autoCloseEnabled));

    if (!m_autoCloseEnabled) return;

    if (m_currentEditor.isNull()) {
        updateCurrentEditor();
    }
    if (m_currentEditor.isNull()) return;

    if (isEditorReadOnly(m_currentEditor)) return;

    int cursorPos = static_cast<int>(sciDirectCall(m_currentEditor, SCI_GETCURRENTPOS));

    if (charadded == '>') {
        // 输入 '>' → 检查是否是开标签闭合，自动插入对应的 </tag>
        tryAutoCloseTag(m_currentEditor, cursorPos);
    } else if (charadded == '<') {
        // 输入 '<' → 开始跟踪，准备显示自动补全
        // 不做任何事，等用户继续输入字母后触发补全
    } else if (charadded >= 0x20 && charadded <= 0x7E) {
        // 输入可打印字符 → 检查是否在 '<' 之后，显示标签名补全
        tryShowTagCompletion(m_currentEditor, cursorPos);
    }
}

void XmlEditPlugin::onCursorPositionChanged(int line, int index)
{
    Q_UNUSED(line);
    Q_UNUSED(index);

    if (!m_highlightEnabled) return;
    if (m_currentEditor.isNull()) return;

    m_highlighter->tryHighlightAtCursor(m_currentEditor);
}

void XmlEditPlugin::onTextModified(int pos, int mtype, const char* text, int len,
                                    int added, int line, int foldNow, int foldPrev,
                                    int token, int annotationLinesAdded)
{
    Q_UNUSED(text);
    Q_UNUSED(len);
    Q_UNUSED(line);
    Q_UNUSED(foldNow);
    Q_UNUSED(foldPrev);
    Q_UNUSED(token);
    Q_UNUSED(annotationLinesAdded);

    if (!m_autoRenameEnabled) return;
    if ((mtype & 0x03) == 0) return;
    if (mtype & 0x400) return;
    if (mtype & 0x800) return;
    if (m_isRenaming) return;

    if (m_currentEditor.isNull()) updateCurrentEditor();
    if (m_currentEditor.isNull() || isEditorReadOnly(m_currentEditor)) return;

    if (static_cast<int>(sciDirectCall(m_currentEditor, SCI_GETLENGTH)) > LARGE_FILE_THRESHOLD) return;

    // 直接扫描：从 pos 向前找 '<'，判断是否在标签名内
    int tagStart = findTagStartAt(m_currentEditor, pos);
    if (tagStart >= 0 && isPosInTagName(m_currentEditor, tagStart, pos)) {
        m_editingTagStart = tagStart;
        m_renameDebounceTimer->start(50);
        debugLog(QStringLiteral("autoRename: detected edit in tag at %1").arg(tagStart));
    }
}

// 从 pos 向前扫描找最近的 '<'，返回位置，找不到返回 -1
int XmlEditPlugin::findTagStartAt(QsciScintilla* editor, int pos)
{
    for (int i = pos; i >= 0 && i >= pos - 200; --i) {
        char ch = static_cast<char>(sciDirectCall(editor, SCI_GETCHARAT, i));
        if (ch == '>') return -1;  // 在标签外
        if (ch == '<') return i;
        if (ch == '\n' || ch == '\r') return -1;
    }
    return -1;
}

// 检查 pos 是否在标签名内（< 之后、空格或 > 之前）
bool XmlEditPlugin::isPosInTagName(QsciScintilla* editor, int tagStart, int pos)
{
    // < 之后第一个字符不能是 / 或 !
    char firstCh = static_cast<char>(sciDirectCall(editor, SCI_GETCHARAT, tagStart + 1));
    if (firstCh == '/' || firstCh == '!' || firstCh == '?') return false;

    // pos 必须在 < 之后且在标签名结束之前（空格、>、/、换行之前）
    for (int i = tagStart + 1; i <= pos; ++i) {
        char ch = static_cast<char>(sciDirectCall(editor, SCI_GETCHARAT, i));
        if (ch == ' ' || ch == '>' || ch == '/' || ch == '\t' || ch == '\r' || ch == '\n')
            return false;
    }
    return true;
}

// --- 自动重命名 ---

void XmlEditPlugin::performAutoRename()
{
    if (!m_autoRenameEnabled || m_isRenaming) { debugLog(QStringLiteral("rn: disabled or renaming")); return; }
    if (m_currentEditor.isNull()) { updateCurrentEditor(); }
    if (m_currentEditor.isNull() || isEditorReadOnly(m_currentEditor)) { debugLog(QStringLiteral("rn: no editor/readonly")); return; }

    int tagStart = m_editingTagStart;
    m_editingTagStart = -1;
    if (tagStart < 0) { debugLog(QStringLiteral("rn: no tagStart")); return; }

    if (static_cast<char>(sciDirectCall(m_currentEditor, SCI_GETCHARAT, tagStart)) != '<') {
        int curPos = static_cast<int>(sciDirectCall(m_currentEditor, SCI_GETCURRENTPOS));
        tagStart = findTagStartAt(m_currentEditor, curPos);
        if (tagStart < 0) { debugLog(QStringLiteral("rn: tagStart not <")); return; }
    }

    m_cachedEditor = nullptr;
    m_cachedText.clear();
    QString docText = getEditorText(m_currentEditor);
    if (docText.isEmpty()) { debugLog(QStringLiteral("rn: empty text")); return; }

    QVector<XmlTagInfo> allTags = XmlTagParser::parseAllTags(docText);

    int tagIdx = -1;
    for (int i = 0; i < allTags.size(); ++i) {
        if (allTags[i].startPos == tagStart) { tagIdx = i; break; }
    }
    if (tagIdx < 0) {
        int bestDist = INT_MAX;
        for (int i = 0; i < allTags.size(); ++i) {
            int d = qAbs(allTags[i].startPos - tagStart);
            if (d < bestDist && d < 10) { bestDist = d; tagIdx = i; }
        }
    }
    if (tagIdx < 0) { debugLog(QStringLiteral("rn: tagStart=%1 not in allTags(%2)").arg(tagStart).arg(allTags.size())); return; }

    const XmlTagInfo& tag = allTags[tagIdx];
    if (tag.type == XmlTagType::XTT_SelfClosing || tag.type == XmlTagType::XTT_None) { debugLog(QStringLiteral("rn: selfclosing/none")); return; }

    debugLog(QStringLiteral("rename: tag='%1' pos=%2 type=%3").arg(tag.name).arg(tag.startPos).arg(static_cast<int>(tag.type)));

    XmlTagInfo matching;
    if (tag.type == XmlTagType::XTT_Open) {
        int depth = 1;
        for (int i = tagIdx + 1; i < allTags.size(); ++i) {
            if (allTags[i].type == XmlTagType::XTT_Open) depth++;
            else if (allTags[i].type == XmlTagType::XTT_Close) { depth--; if (depth == 0) { matching = allTags[i]; break; } }
        }
    } else {
        int depth = 1;
        for (int i = tagIdx - 1; i >= 0; --i) {
            if (allTags[i].type == XmlTagType::XTT_Close) depth++;
            else if (allTags[i].type == XmlTagType::XTT_Open) { depth--; if (depth == 0) { matching = allTags[i]; break; } }
        }
    }

    if (!matching.isValid()) { debugLog(QStringLiteral("rename: no match for '%1'").arg(tag.name)); return; }
    if (tag.name == matching.name) { debugLog(QStringLiteral("rename: same name '%1'").arg(tag.name)); return; }

    debugLog(QStringLiteral("rename: '%1' -> '%2'").arg(matching.name).arg(tag.name));

    m_isRenaming = true;
    QByteArray utf8Name = tag.name.toUtf8();

    // 保存当前光标和可视区域
    int curPos = static_cast<int>(sciDirectCall(m_currentEditor, SCI_GETCURRENTPOS));
    int firstLine = static_cast<int>(sciDirectCall(m_currentEditor, SCI_GETFIRSTVISIBLELINE));

    sciDirectCall(m_currentEditor, SCI_BEGINUNDOACTION);
    sciDirectCall(m_currentEditor, SCI_SETSELECTIONSTART, matching.nameStartPos);
    sciDirectCall(m_currentEditor, SCI_SETSELECTIONEND, matching.nameEndPos);
    sciDirectCallPtr(m_currentEditor, SCI_REPLACESEL, 0, (void*)utf8Name.constData());
    sciDirectCall(m_currentEditor, SCI_ENDUNDOACTION);

    // 恢复光标位置和视图
    sciDirectCall(m_currentEditor, SCI_GOTOPOS, curPos);
    sciDirectCall(m_currentEditor, SCI_SETFIRSTVISIBLELINE, firstLine);

    m_isRenaming = false;
}

// --- 自动闭合：输入 '>' 时自动插入对应的 </tag> ---

void XmlEditPlugin::tryAutoCloseTag(QsciScintilla* editor, int cursorPos)
{
    if (!editor) return;
    if (isEditorReadOnly(editor)) return;

    // 需要至少 3 个字符: <x>
    if (cursorPos < 3) return;

    debugLog(QStringLiteral("tryAutoCloseTag: cursorPos=%1").arg(cursorPos));

    // 向前搜索匹配的 '<'
    int searchStart = cursorPos - 2; // 跳过刚输入的 '>'
    int tagStart = -1;

    for (int i = searchStart; i >= 0; --i) {
        char ch = static_cast<char>(sciDirectCall(editor, SCI_GETCHARAT, i));
        if (ch == '>') {
            // 遇到 '>' 说明前面有另一个标签，停止搜索
            return;
        }
        if (ch == '<') {
            tagStart = i;
            break;
        }
        if (ch == '\n' || ch == '\r') {
            // 遇到换行，停止搜索（标签不应跨行）
            return;
        }
    }

    if (tagStart < 0) return;

    // 读取 '<' 到 '>' 之间的内容（不含 '<' 和刚输入的 '>'）
    int tagContentLen = cursorPos - 1 - tagStart - 1;
    if (tagContentLen <= 0) return;

    QByteArray tagContent;
    tagContent.resize(tagContentLen + 1);
    Sci_TextRange tr;
    tr.chrg.cpMin = tagStart + 1;
    tr.chrg.cpMax = cursorPos - 1;
    tr.lpstrText = tagContent.data();
    sciDirectCallPtr(editor, SCI_GETTEXTRANGE, 0, &tr);
    QString content = QString::fromUtf8(tagContent.data());

    debugLog(QStringLiteral("tryAutoCloseTag: content='%1'").arg(content));

    // 排除特殊标签：</xxx>, <?xxx, <!--, <![CDATA[, <xxx/>
    if (content.isEmpty()) return;
    if (content.startsWith('/')) return;        // </xxx>
    if (content.startsWith('?')) return;        // <?xml?>
    if (content.startsWith('!')) return;         // <!-- -->, <![CDATA[
    if (content.endsWith('/')) return;          // <xxx/>

    // 提取标签名（到第一个空格或结束）
    QString tagName = content.split(' ').first().split('\t').first();
    if (tagName.isEmpty()) return;

    // 过滤无效字符
    for (const QChar& c : tagName) {
        if (!c.isLetterOrNumber() && c != ':' && c != '_' && c != '-' && c != '.') {
            return;
        }
    }

    debugLog(QStringLiteral("tryAutoCloseTag: inserting </%1>").arg(tagName));

    // 插入闭标签并定位光标到标签之间
    QString closingTag = QStringLiteral("</%1>").arg(tagName);
    QByteArray utf8Close = closingTag.toUtf8();

    // 防止自动闭合触发的 SCN_MODIFIED 干扰重命名
    m_isRenaming = true;
    sciDirectCall(editor, SCI_BEGINUNDOACTION);
    sciDirectCallPtr(editor, SCI_INSERTTEXT, cursorPos,
                           (void*)utf8Close.constData());
    // 光标回到 > 之前（不动视图）
    sciDirectCall(editor, SCI_GOTOPOS, cursorPos);
    sciDirectCall(editor, SCI_ENDUNDOACTION);
    // 恢复光标到插入的闭标签之后（用户下次输入的位置）
    sciDirectCall(editor, SCI_GOTOPOS, cursorPos + utf8Close.length());
    m_isRenaming = false;
}

// --- 标签名自动补全：在 '<' 后输入字符时显示已有标签名建议 ---

void XmlEditPlugin::tryShowTagCompletion(QsciScintilla* editor, int cursorPos)
{
    if (!editor) return;
    if (isEditorReadOnly(editor)) return;

    // 大文件不做全文档补全
    int docLength = static_cast<int>(sciDirectCall(editor, SCI_GETLENGTH));
    if (docLength > LARGE_FILE_THRESHOLD) return;

    // 向前搜索最近的 '<'，确认光标处于标签名位置
    int tagStart = -1;
    for (int i = cursorPos - 1; i >= 0; --i) {
        char ch = static_cast<char>(sciDirectCall(editor, SCI_GETCHARAT, i));
        if (ch == '>') return;      // 不在标签内
        if (ch == '<') {
            tagStart = i;
            break;
        }
        if (ch == '\n' || ch == '\r') return;
        if (ch == ' ' && i == tagStart + 1) return; // < 后直接是空格
    }
    if (tagStart < 0) return;

    // 读取 '<' 到光标之间的部分标签名
    int partialLen = cursorPos - tagStart - 1;
    if (partialLen < 1) return; // 只有 '<'，还没输入字母

    QByteArray partialData;
    partialData.resize(partialLen + 1);
    Sci_TextRange tr;
    tr.chrg.cpMin = tagStart + 1;
    tr.chrg.cpMax = cursorPos;
    tr.lpstrText = partialData.data();
    sciDirectCallPtr(editor, SCI_GETTEXTRANGE, 0, &tr);
    QString partial = QString::fromUtf8(partialData.data());

    // 过滤：只对字母开头的内容做补全
    if (partial.isEmpty()) return;
    if (!partial.at(0).isLetter() && partial.at(0) != '_') return;

    // 检查是否已有 '/' 或 '!'（闭标签或注释）
    if (partial.startsWith('/') || partial.startsWith('!') || partial.startsWith('?')) return;

    debugLog(QStringLiteral("tryShowTagCompletion: partial='%1'").arg(partial));

    // 收集文档中所有唯一标签名
    QString text = getEditorText(editor);
    QVector<XmlTagInfo> allTags = XmlTagParser::parseAllTags(text);

    QSet<QString> tagNames;
    for (const XmlTagInfo& tag : allTags) {
        if (!tag.name.isEmpty() && tag.name.startsWith(partial, Qt::CaseInsensitive)) {
            tagNames.insert(tag.name);
        }
    }

    if (tagNames.isEmpty()) return;

    // 构建补全列表（按字母排序），用空格分隔
    QStringList sortedNames = tagNames.values();
    sortedNames.sort(Qt::CaseInsensitive);

    QString list = sortedNames.join(' ');
    QByteArray utf8List = list.toUtf8();

    debugLog(QStringLiteral("tryShowTagCompletion: showing %1 suggestions").arg(sortedNames.size()));

    // 显示自动补全弹窗：lenEntered = partialLen 表示替换已键入的部分
    sciDirectCall(editor, SCI_AUTOCSETSEPARATOR, ' ');
    sciDirectCall(editor, SCI_AUTOCSHOW, partialLen,
                           reinterpret_cast<sptr_t>(utf8List.constData()));
}

// --- 文本缓存 ---

QString XmlEditPlugin::getEditorText(QsciScintilla* editor)
{
    if (!editor) return QString();

    int docLen = static_cast<int>(sciDirectCall(editor, SCI_GETLENGTH));

    // 缓存命中检查
    if (m_cachedEditor == editor && !m_cachedText.isEmpty()) {
        if (docLen == static_cast<int>(m_cachedText.length())) {
            return m_cachedText;
        }
    }

    // 用 SCI_GETTEXTRANGE 读取原始字节，以 Latin-1 转 QString
    // 这样保证每个字节 = 一个 QChar，Scintilla 字节位置与 QString 位置一致
    QByteArray raw;
    raw.resize(docLen + 1);
    Sci_TextRange tr;
    tr.chrg.cpMin = 0;
    tr.chrg.cpMax = docLen;
    tr.lpstrText = raw.data();
    sciDirectCallPtr(editor, SCI_GETTEXTRANGE, 0, &tr);

    QString text = QString::fromLatin1(raw.data(), docLen);
    m_cachedEditor = editor;
    m_cachedText = text;
    return text;
}

void XmlEditPlugin::clearTextCache()
{
    m_cachedEditor = nullptr;
    m_cachedText.clear();
}
