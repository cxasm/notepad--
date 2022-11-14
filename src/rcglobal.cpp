#include "rcglobal.h"
#include <QSettings>
#include <QFileInfo>
#include <QDir>

QString s_userLangDirPath;

QString getUserLangDirPath()
{
	if (s_userLangDirPath.isEmpty())
	{
		QString settingDir = QString("notepad/userlang/test");
		QSettings qs(QSettings::IniFormat, QSettings::UserScope, settingDir);
		QString qsSavePath = qs.fileName();
		QFileInfo fi(qsSavePath);
		s_userLangDirPath = fi.dir().absolutePath();
		//保存起来userLangPath，避免后续每次都要去查找。注意这个目录是在notepad/userlang这个级别
	}
	return s_userLangDirPath;
}
