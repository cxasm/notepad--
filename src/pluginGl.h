#pragma once
#include <QString>
#include <QMenu>

struct ndd_proc_data
{
	QString m_strPlugName; //插件名称 必选。插件里面需填写
	QString m_strFilePath; //lib 插件的全局路径。必选。插件内部不用填写,主程序传递下来
	QString m_strComment; //插件说明；插件里面需填写
	QString m_version; //版本号码。可选；插件里面需填写
	QString m_auther;//作者名称。可选；插件里面需填写
	int m_menuType;//菜单类型。0：不使用二级菜单 1：创建二级菜单；插件里面需填写
	QMenu* m_rootMenu;//如果m_menuType = 1，给出二级根菜单的地址。其他值nullptr 插件内部不用填写，主程序传递下来

	ndd_proc_data(): m_rootMenu(nullptr), m_menuType(0)
	{

	}
};

//#define NDD_PROC_IDENTIFY ("nddProc")

typedef struct ndd_proc_data NDD_PROC_DATA;

typedef bool (*NDD_PROC_IDENTIFY_CALLBACK)(NDD_PROC_DATA* pProcData);
typedef void (*NDD_PROC_FOUND_CALLBACK)(NDD_PROC_DATA* pProcData, void* pUserData);
