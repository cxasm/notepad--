#pragma once

// 安全 Scintilla 调用封装
// 从宿主 qmyedit_qt5.dll 获取两个 SendScintilla 重载：
// 1. (uint, ulong, long)   — 用于整数参数
// 2. (uint, ulong, void*)  — 用于指针参数 (lParam 是 64-bit 指针)

#include <Windows.h>
#include <qsciscintilla.h>
#include <Scintilla.h>

// 获取函数指针的辅助模板
template<typename T>
static T getHostExport(const char* name)
{
    static T fn = nullptr;
    static bool tried = false;
    if (!tried) {
        tried = true;
        HMODULE hMod = GetModuleHandleW(L"qmyedit_qt5.dll");
        if (hMod)
            fn = reinterpret_cast<T>(GetProcAddress(hMod, name));
    }
    return fn;
}

// 整数参数版本: long SendScintilla(uint, ulong, long)
using SciIntFn = long (*)(const void*, unsigned int, unsigned long, long);

// 指针参数版本: long SendScintilla(uint, ulong, void*)
using SciPtrFn = long (*)(const void*, unsigned int, unsigned long, void*);

inline sptr_t sciDirectCall(QsciScintilla* editor, unsigned int msg,
                            uptr_t wParam = 0, sptr_t lParam = 0)
{
    if (!editor) return 0;
    SciIntFn fn = getHostExport<SciIntFn>(
        "?SendScintilla@QsciScintillaBase@@QEBAJIKJ@Z");
    if (!fn) return 0;
    const void* base = static_cast<const QsciScintillaBase*>(editor);
    __try {
        return fn(base, msg,
                  static_cast<unsigned long>(wParam),
                  static_cast<long>(lParam));
    } __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

// 指针参数版本（SCI_GETTEXTRANGE, SCI_INSERTTEXT 等需要传指针）
inline sptr_t sciDirectCallPtr(QsciScintilla* editor, unsigned int msg,
                               uptr_t wParam, void* lParam)
{
    if (!editor) return 0;
    SciPtrFn fn = getHostExport<SciPtrFn>(
        "?SendScintilla@QsciScintillaBase@@QEBAJIKPEAX@Z");
    if (!fn) return 0;
    const void* base = static_cast<const QsciScintillaBase*>(editor);
    __try {
        return fn(base, msg, static_cast<unsigned long>(wParam), lParam);
    } __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

inline QString sciGetText(QsciScintilla* editor)
{
    if (!editor) return QString();
    const int docLen = static_cast<int>(sciDirectCall(editor, SCI_GETLENGTH));
    if (docLen <= 0) return QString();
    QByteArray raw;
    raw.resize(docLen + 1);
    Sci_TextRange tr;
    tr.chrg.cpMin = 0;
    tr.chrg.cpMax = docLen;
    tr.lpstrText = raw.data();
    sciDirectCallPtr(editor, SCI_GETTEXTRANGE, 0, &tr);
    return QString::fromLatin1(raw.constData(), docLen);
}
