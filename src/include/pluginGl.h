#pragma once
#include <QString>

#define NDD_EXPORTDLL

#if defined(Q_OS_WIN)
#if defined(NDD_EXPORTDLL)
#define NDD_EXPORT __declspec(dllexport)
#else
#define NDD_EXPORT __declspec(dllimport)
#endif
#endif

struct ndd_proc_data
{
	QString m_strPlugName; //插件名称 必选
	QString m_strFilePath; //lib 插件的全局路径。必选
	QString m_strComment; //插件说明
	QString m_version; //版本号码。可选
	QString m_auther;//作者名称。可选
};


typedef struct ndd_proc_data NDD_PROC_DATA;

typedef bool (*NDD_PROC_IDENTIFY_CALLBACK)(NDD_PROC_DATA* pProcData);
typedef void (*NDD_PROC_FOUND_CALLBACK)(NDD_PROC_DATA* pProcData, void* pUserData);
