#include "rcglobal.h"
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QProcess>

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

const int M_SIZE = 1024 * 1024;
const int G_SIZE = 1024 * 1024 * 1024;

//把字节大小文件，转换为M 或 G 单位
QString tranFileSize(qint64 fileSize)
{
	float num = 0.0f;
	QString unit;

	if (fileSize >= G_SIZE)
	{
		num = double(fileSize) / G_SIZE;
		unit = "GB";
	}
	else if (fileSize >= M_SIZE)
	{
		num = double(fileSize) / M_SIZE;
		unit = "MB";
	}
	else if (fileSize > 1024)
	{
		num = float(fileSize) / 1024;
		unit = "KB";
	}
	else
	{
		return QString("%1").arg(fileSize);
	}
	
	return QString("%1 %2").arg(num, 0, 'f' , 2).arg(unit);
}

void showFileInExplorer(QString path)
{
	QString cmd;

#ifdef _WIN32
	path = path.replace("/", "\\");
	cmd = QString("explorer.exe /select,%1").arg(path);
#endif

#ifdef ubu
	path = path.replace("\\", "/");
	cmd = QString("nautilus %1").arg(path);
#endif

#ifdef uos
	path = path.replace("\\", "/");
	cmd = QString("dde-file-manager %1").arg(path);
#endif 

#if defined(Q_OS_MAC)
	path = path.replace("\\", "/");
	cmd = QString("open -R %1").arg(path);
#endif

	QProcess process;
	process.startDetached(cmd);
}
