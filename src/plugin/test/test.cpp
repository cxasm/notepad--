#include <qobject.h>
#include <qstring.h>
#include <pluginGl.h>
#include <functional>
#include <qsciscintilla.h>
#include <QAction>

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
	NDD_EXPORT int NDD_PROC_MAIN(QWidget* pNotepad, const QString& strFileName, std::function<QsciScintilla* ()>getCurEdit, NDD_PROC_DATA* procData);

#ifdef __cplusplus
	}
#endif

static NDD_PROC_DATA s_procData;
static QWidget* s_pMainNotepad = nullptr;
std::function<QsciScintilla* ()> s_getCurEdit;

bool NDD_PROC_IDENTIFY(NDD_PROC_DATA* pProcData)
{
	if(pProcData == NULL)
	{
		return false;
	}
	pProcData->m_strPlugName = QObject::tr("Test Plug");
	pProcData->m_strComment = QObject::tr("char to lower.");

	pProcData->m_version = QString("v1.0");
	pProcData->m_auther = QString("zuowei.yin");

	pProcData->m_menuType = 1;
	return true;
}

//则点击菜单栏按钮时，会自动调用到该插件的入口点函数。
//pNotepad:就是CCNotepad的主界面指针
//strFileName:当前插件DLL的全路径，如果不关心，则可以不使用
//getCurEdit:从NDD主程序传递过来的仿函数，通过该函数获取当前编辑框操作对象QsciScintilla
//pProcData:如果pProcData->m_menuType = 0 ,则该指针为空；如果pProcData->m_menuType = 1，则该指针有值。目前需要关心s_procData.m_rootMenu
//开发者可以在该菜单下面，自行创建二级菜单
int NDD_PROC_MAIN(QWidget* pNotepad, const QString& strFileName, std::function<QsciScintilla* ()>getCurEdit, NDD_PROC_DATA* pProcData)
{

	//务必拷贝一份pProcData，在外面会释放。
	if (pProcData != nullptr)
	{
		s_procData = *pProcData;
	}
	else
	{
		return -1;
	}

	s_pMainNotepad = pNotepad;
	s_getCurEdit = getCurEdit;

	//如果pProcData->m_menuType = 1;是自己要创建二级菜单的场景。则通过s_procData.m_rootMenu 获取该插件的菜单根节点。
	//插件开发者自行在s_procData.m_rootMenu下添加新的二级菜单项目

	QAction* pAction = new QAction(s_procData.m_strPlugName, s_procData.m_rootMenu);
	s_procData.m_rootMenu->addAction(pAction);
	pAction->setText("Secondary Menu Example");

	return 0;
}
