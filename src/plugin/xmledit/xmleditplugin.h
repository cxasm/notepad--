#pragma once

#include <QObject>
#include <QPointer>
#include <QAction>
#include <QTimer>
#include <functional>

#include "pluginGl.h"

class QsciScintilla;
class QMenu;
class QTabWidget;
class XmlTagHighlighter;

// XML 编辑插件主协调类
class XmlEditPlugin : public QObject
{
    Q_OBJECT

public:
    explicit XmlEditPlugin(QWidget* pNotepad,
                           std::function<QsciScintilla*(QWidget*)> getCurEdit,
                           NDD_PROC_DATA* pProcData,
                           QObject* parent = nullptr);
    ~XmlEditPlugin();

    // 清理资源（断开所有信号连接）
    void cleanup();

private slots:
    // 菜单动作
    void onActionJumpToMatch();
    void onActionAutoCloseToggled(bool checked);
    void onActionAutoRenameToggled(bool checked);
    void onActionHighlightToggled(bool checked);

    // 编辑器事件
    void onCharAdded(int charadded);
    void onCursorPositionChanged(int line, int index);
    void onTextModified(int pos, int mtype, const char* text, int len,
                        int added, int line, int foldNow, int foldPrev,
                        int token, int annotationLinesAdded);

    // Tab 切换
    void onTabChanged(int index);

    // 防抖定时器触发实际重命名
    void performAutoRename();

private:
    // 安装/卸载编辑器事件监听
    void installEditorListeners(QsciScintilla* editor);
    void uninstallEditorListeners(QsciScintilla* editor);

    // 更新当前编辑器引用
    void updateCurrentEditor();

    // 创建菜单
    void createMenu(NDD_PROC_DATA* pProcData);

    // 设置 Tab 切换监听
    void setupTabMonitoring();

    // 输入 '>' 时自动插入对应的 </tag>
    void tryAutoCloseTag(QsciScintilla* editor, int cursorPos);

    // 输入 '<' 后继续输入字符时，显示已有标签名的自动补全
    void tryShowTagCompletion(QsciScintilla* editor, int cursorPos);

    // 扫描 pos 向前找最近的 '<'，返回位置（用于判断是否在标签内）
    int findTagStartAt(QsciScintilla* editor, int pos);

    // 检查 pos 是否在 tagStart 处 '<' 开始的标签名内
    bool isPosInTagName(QsciScintilla* editor, int tagStart, int pos);

    // 获取编辑器全文（缓存优化）
    QString getEditorText(QsciScintilla* editor);

    // 清除上一个文档的缓存
    void clearTextCache();

    // --- 成员变量 ---
    QWidget* m_pNotepad;
    QMenu* m_rootMenu;
    std::function<QsciScintilla*(QWidget*)> m_getCurEdit;

    QPointer<QsciScintilla> m_currentEditor;

    XmlTagHighlighter* m_highlighter;
    QTimer* m_renameDebounceTimer;

    // 功能开关
    bool m_autoCloseEnabled;
    bool m_autoRenameEnabled;
    bool m_highlightEnabled;

    // 菜单动作
    QAction* m_actionJumpToMatch;
    QAction* m_actionAutoClose;
    QAction* m_actionAutoRename;
    QAction* m_actionHighlightPair;

    // 文本缓存
    QString m_cachedText;
    QPointer<QsciScintilla> m_cachedEditor;

    // 跳转时用于闪烁目标标签的定时器
    QTimer* m_jumpFlashTimer;
    int m_jumpFlashCount;
    int m_jumpTargetStart;
    int m_jumpTargetEnd;

    // 初始化重试定时器（编辑器未就绪时延迟重试）
    QTimer* m_initRetryTimer;
    int m_initRetryCount;

    // 正在编辑的标签 '<' 位置（-1 表示未在标签名内编辑）
    int m_editingTagStart;

    // 防止自动重命名递归触发
    bool m_isRenaming;
};
