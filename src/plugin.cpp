#include "plugin.h"
#include <QLibrary>
#include <QDir>
#include <QMenu>
#include <QAction>


bool loadApplication(const QString& strFileName, NDD_PROC_DATA* pProcData)
{
	QLibrary lib(strFileName);
	NDD_PROC_IDENTIFY_CALLBACK procCallBack;

	procCallBack = (NDD_PROC_IDENTIFY_CALLBACK)lib.resolve("NDD_PROC_IDENTIFY");

	if (procCallBack == NULL)
	{
		return false;
	}

	if (!procCallBack(pProcData))
	{
		return false;
	}
	pProcData->m_strFilePath = strFileName;
	return true;
}




int loadProc(const QString& strDirOut, std::function<void(NDD_PROC_DATA&, QMenu*)> funcallback, QMenu* pUserData)
{
	int nReturn = 0;
	QStringList list;

	QDir dir;
	dir.setPath(strDirOut);

	QString strDir, strName;
	QStringList strFilter;

	strDir = dir.absolutePath();
	strDir += QDir::separator();
#if  defined(Q_OS_WIN)
	strFilter << "*.dll";
#else
	strFilter << "lib*.so";
#endif
	list = dir.entryList(strFilter, QDir::Files | QDir::Readable, QDir::Name);
	QStringList::Iterator it = list.begin();

	for (; it != list.end(); ++it)
	{
		NDD_PROC_DATA procData;
		strName = *it;
		strName = strDir + strName;

		if (!loadApplication(strName, &procData))
		{
			continue;
		}

		funcallback(procData, pUserData);
		
		nReturn++;
	}

	return nReturn;
}

