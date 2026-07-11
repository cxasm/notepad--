#include <qobject.h>
#include <qstring.h>
#include <pluginGl.h>
#include <functional>
#include <qsciscintilla.h>
#include "xmleditplugin.h"
#ifdef WIN32
#include <Windows.h>
#endif

#define NDD_EXPORTDLL

#if defined(Q_OS_WIN)
    #if defined(NDD_EXPORTDLL)
        #define NDD_EXPORT __declspec(dllexport)
    #else
        #define NDD_EXPORT __declspec(dllimport)
    #endif
#else
    #define NDD_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
    extern "C" {
#endif

    NDD_EXPORT bool NDD_PROC_IDENTIFY(NDD_PROC_DATA* pProcData);
    NDD_EXPORT int NDD_PROC_MAIN(QWidget* pNotepad, const QString& strFileName,
        std::function<QsciScintilla* (QWidget*)>getCurEdit,
        std::function<bool(QWidget*, int, void*)> pluginCallBack,
        NDD_PROC_DATA* procData);

#ifdef __cplusplus
    }
#endif

static NDD_PROC_DATA s_procData;
static XmlEditPlugin* s_pluginInstance = nullptr;

std::function<QsciScintilla* (QWidget*)> s_getCurEdit;
std::function<bool(QWidget*, int, void*)> s_invokeMainFun;

// 插件识别回调：主程序扫描插件目录时调用，获取插件元信息
bool NDD_PROC_IDENTIFY(NDD_PROC_DATA* pProcData)
{
    if (pProcData == NULL)
    {
        return false;
    }

    pProcData->m_strPlugName = QObject::tr("Easy XML");
    pProcData->m_strComment = QObject::tr("Easy XML: tag auto-close, auto-rename, jump to match, highlight pair");
    pProcData->m_version = QString("v1.0");
    pProcData->m_auther = QString("ndd-community");
    pProcData->m_menuType = 1; // 使用二级菜单

    return true;
}

// 插件入口回调：主程序加载插件后调用
int NDD_PROC_MAIN(QWidget* pNotepad, const QString& strFileName,
    std::function<QsciScintilla*(QWidget*)>getCurEdit,
    std::function<bool(QWidget*, int, void*)> pluginCallBack,
    NDD_PROC_DATA* pProcData)
{
    Q_UNUSED(strFileName);

    if (pProcData == nullptr)
    {
        return -1;
    }

    s_getCurEdit = getCurEdit;
    s_invokeMainFun = pluginCallBack;

    // 保存 procData 副本（外部会释放）
    s_procData = *pProcData;

    // 创建插件实例（生命周期由 pNotepad 管理）
    s_pluginInstance = new XmlEditPlugin(pNotepad, getCurEdit, pProcData);
    s_pluginInstance->setObjectName(QStringLiteral("nddplg")); // 主程序清理时通过此名称查找

    return 0;
}

#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
    Q_UNUSED(hInst);

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if (NULL == lpvReserved)
        {
            // 进程退出时做全局回收
            if (s_pluginInstance != nullptr)
            {
                s_pluginInstance->cleanup();
                delete s_pluginInstance;
                s_pluginInstance = nullptr;
            }
        }
        break;
    }
    return TRUE;
}
#else
__attribute__((destructor))
void onDllUnload(void)
{
    if (s_pluginInstance != nullptr)
    {
        s_pluginInstance->cleanup();
        delete s_pluginInstance;
        s_pluginInstance = nullptr;
    }
}
#endif
