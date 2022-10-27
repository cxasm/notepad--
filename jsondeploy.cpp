#include "jsondeploy.h"
#include <QObject>
#include <QJsonObject>
#include <QtDebug>
#include <QCoreApplication>
#include <QDate>
#include <QFile>
#include <QStorageInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSettings>

//替换JsonDeploy.h的类。不使用数据库，直接写一个文件就行。

static int version_num = 24;

//数据库文件是否存在。如果初始化失败，则不存在
bool JsonDeploy::s_isExistDb = false;
int JsonDeploy::s_reference = 0;
bool JsonDeploy::s_isContentChanged = false;

QJsonObject* JsonDeploy::s_jsonObj = nullptr;


//因为WIN10的C盘没有写权限，我们选择一个可以写的目录
QString JsonDeploy::selectDataPath()
{
#if defined(Q_OS_WIN)

	QString settingDir = QString("notepad/test");
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, settingDir);
	QString qsSavePath = qs.fileName();
	QFileInfo fi(qsSavePath);
	QString dbPath = fi.dir().absolutePath();

	QString ret;
	bool exist = false;

	QDir dir(dbPath);

	if (!dir.exists())
	{
		if (dir.mkpath(dir.absolutePath()))
		{
			ret = dbPath;
			exist = true;
		}
	}
	else
	{
		ret = dbPath;
		exist = true;
	}
	
	//理论上一定有，如果没有，则下面多半也会错误的
	if (!exist)
	{
		{
			QString name = QDir::home().dirName();
			if (name.isEmpty())
			{
				return ret;
			}

			QString dbPath = QString("c:/Users/%1/.notepad").arg(name);
			QDir dir(dbPath);

			if (!dir.exists())
			{
				if (dir.mkpath(dir.absolutePath()))
				{
					ret = dbPath;
				}
			}
			else
			{
				ret = dbPath;
			}
		}
	}

#if 0
	QStringList volumesList;
	volumesList << "E:/" << "D:/";

	QString existVolume;
	QString ret;
	bool exist = false;

	for (QString path : volumesList)
	{
		QDir dir(path);
		if (dir.exists())
		{
			existVolume = path;
			exist = true;
			break;
		}
	}

	qDebug() << existVolume;

	if (exist)
	{
		QString dbPath = existVolume;
		dbPath.append("Program Files/Notepad");

		QDir dir(dbPath);

		if (!dir.exists())
		{
			if (dir.mkpath(dir.absolutePath()))
			{
				ret = dbPath;
			}
		}
		else
		{
			ret = dbPath;
		}
	}
	else
	{
		QString name = QDir::home().dirName();
		if (name.isEmpty())
		{
			return ret;
		}
		//如果没有d e盘，则创建在c盘的用户目录下面

		QString dbPath = QString("c:/Users/%1/.notepad").arg(name);
		QDir dir(dbPath);

		if (!dir.exists())
		{
			if (dir.mkpath(dir.absolutePath()))
			{
				ret = dbPath;
			}
		}
		else
		{
			ret = dbPath;
		}
	}
#endif

#elif defined(Q_OS_MAC)

	QString ret;
	QString name = QDir::home().dirName();
	if (name.isEmpty())
	{
		return ret;
	}

	QString dbPath = QString("/Users/%1/Applications/com.hmja.notepad").arg(name);

	QDir dir(dbPath);

	if (!dir.exists())
	{
		if (dir.mkpath(dir.absolutePath()))
		{
			ret = dbPath;
		}
	}
	else
	{
		ret = dbPath;
	}
#else
	QString ret;
	QString name = QDir::home().dirName();
	if (name.isEmpty())
	{
		return ret;
	}

    QString dbPath = QString("/home/%1/.config/com.hmja.notepad").arg(name);

	QDir dir(dbPath);

	if (!dir.exists())
	{
		if (dir.mkpath(dir.absolutePath()))
		{
			ret = dbPath;
		}
	}
	else
	{
		ret = dbPath;
	}
#endif
	return ret;
}

//如果key不存在，则新增key-value
void JsonDeploy::checkNoExistAdd(QString key, QJsonValue& value)
{
	//每次新加字段后，这里要检查一下，不存在则新增
	QJsonValue v = s_jsonObj->value(key);
	if (v.isUndefined())
	{
		s_jsonObj->insert(key, value);
	}
}
//20220402这里隐含了一个前置条件：数据库句柄是在主线程创建的，最好不要在子线程中调用。
//避免因为跨线程访问和多线程冲突访问引发的闪退问题。所以最好数据库的读写都在主线程中进行
void JsonDeploy::init()
{
	++s_reference;

	//如果已经初始化过了，则直接返回
	if (s_isExistDb)
	{
		return;
	}

	s_jsonObj = new QJsonObject();

	bool initOk = true;

	QString dbDir = selectDataPath();

	QString dbPath;

	if (dbDir.isEmpty())
	{
		dbPath = "options.json";
	}
	else
	{
		dbPath = QString("%1/options.json").arg(dbDir);
	}

	s_jsonObj->insert(PRO_DIR, dbDir);
	s_jsonObj->insert("dbpath", dbPath);

	//打开json文件解析
	QFile file(dbPath);

	auto initJosnObj = []() {
		QString key = "signdate";
		//QString date = QDate::currentDate().toString(QString("yyyy/M/d"));
		//不写今天的时间，否则第一次运行，无条件不会发送心跳。
		//直接写一个过去的时间，让第一次运行，总是要签到
		addKeyValueToSets(key, "2022/2/20");
#if 0
#if defined(Q_OS_WIN)
		//addKeyValueToSets(TXT_FONT, QString(u8"宋体,14,-1,5,50,0,0,0,0,0,常规"));
		//addKeyValueToSets(PRO_LANG_FONT, QString("Courier New,14,-1,5,50,0,0,0,0,0,Regular"));
#elif defined(Q_OS_MAC)
        addKeyValueToSets(TXT_FONT, QString(u8"STSong,18,-1,5,50,0,0,0,0,0,Regular"));
		addKeyValueToSets(PRO_LANG_FONT, QString("Menlo,14,-1,5,50,0,0,0,0,0,Regular"));
#else
        addKeyValueToSets(TXT_FONT, QString(u8"CESI宋体-GB2312,14,-1,5,50,0,0,0,0,0,Regular"));
		addKeyValueToSets(PRO_LANG_FONT, QString("Bitstream Vera Sans,11,-1,5,50,0,0,0,0,0,Regular"));
#endif
#endif
		QString str;

		addKeyValueToLongSets("recentopenfile", str);

		//tab的长度，默认为4
		addKeyValueToNumSets("tablens", 4);
		//space replace tab空格替换tab，默认1为true,0为false
		addKeyValueToNumSets("tabnouse", 1);

		addKeyValueToSets("mac", "0");
		addKeyValueToNumSets("padtimes", 0);
		addKeyValueToNumSets("serverip", 0);

		//是否高亮不同处背景
		addKeyValueToNumSets("hexhigh", 1);

		//是否高亮不同处背景
		addKeyValueToNumSets("version", version_num);


		//皮肤id
		addKeyValueToNumSets(SKIN_KEY, 1);

		//语言index 0:自动选择 1:中文 2 英文
		addKeyValueToNumSets(LANGS_KEY, 0);

		//开启自动换行
		addKeyValueToNumSets(AUTOWARP_KEY, 0);

		//开启自动缩进
		addKeyValueToNumSets(INDENT_KEY, 0);

		//最大文本文件的门限。默认100M.(50-300)
		addKeyValueToNumSets(MAX_BIG_TEXT, 100);

		//当前软件状态 0 试用版 1 注册版 2 注册码已过期
		addKeyValueToNumSets(SOFT_STATUS, 0);

		addKeyValueToSets(SOFT_KEY, "0");

		addKeyValueToSets(RESTORE_CLOSE_FILE, "1");
		
	};

	//不存在则创建。可能是第一次初始化
	if (!file.exists())
	{
		if (!file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::NewOnly))
		{
			//如果失败，则是权限不够
			initOk = false;
			qDebug() << "Error: Failed to create json set file.";
		}
		else
		{
			initJosnObj();

			QJsonDocument doc(*s_jsonObj);
			QByteArray bytes = doc.toJson(); //是uft8编码的
			file.write(bytes);			
			file.close();
		}
	}
	else
	{
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			//如果失败，则是权限不够
			initOk = false;
			qDebug() << "Error: Failed to open json set file.";
		}
		else
		{
			//加载json配置到内存
			QByteArray bytes = file.readAll();

			QJsonParseError parseError;
			QJsonDocument doc = QJsonDocument::fromJson(bytes, &parseError);

			if (QJsonParseError::NoError == parseError.error)
			{
				*s_jsonObj = QJsonObject(doc.object());

				//每次新加字段后，这里要检查一下，不存在则新增
				{
				QJsonValue v(1);
				checkNoExistAdd(SKIN_KEY, v);
				}

				{
				QJsonValue langs(0);
				checkNoExistAdd(LANGS_KEY, langs);
			}

				{
					QJsonValue v(0);
					checkNoExistAdd(AUTOWARP_KEY, v);
				}

				{
					QJsonValue v(0);
					checkNoExistAdd(INDENT_KEY, v);
				}

				{
					QJsonValue v(100);
					checkNoExistAdd(MAX_BIG_TEXT, v);
			}

				{
					QJsonValue v(0);
					checkNoExistAdd(SOFT_STATUS, v);
			}

				{
					QJsonValue v("0");
					checkNoExistAdd(SOFT_KEY, v);
				}
#if 0
				{
					#if defined(Q_OS_WIN)
						QJsonValue v("Courier New,14,-1,5,50,0,0,0,0,0,Regular");
					#elif defined(Q_OS_MAC)
						QJsonValue v("Menlo,14,-1,5,50,0,0,0,0,0,Regular");
					#else
						QJsonValue v("Bitstream Vera Sans,11,-1,5,50,0,0,0,0,0,Regular");
					#endif
					checkNoExistAdd(PRO_LANG_FONT, v);
			}
#endif
				{
					QJsonValue v(dbDir);
					checkNoExistAdd(PRO_DIR, v);
			}

				{
					QJsonValue v(1);
					checkNoExistAdd(RESTORE_CLOSE_FILE, v);
			}
				
			}
			else
			{
				//读取错误，直接走初始化流程
				initJosnObj();

				//无条件设置为修改过，关闭的时候有机会触发保存配置
				s_isContentChanged = true;
			}
		}
	}

	s_isExistDb = initOk;

}


//写一个总的获取配置的接口，避免以后每个字段都需要写一个读写接口
QString JsonDeploy::getKeyValueFromSets(QString key)
{
	QString ret;
	QJsonValue v = s_jsonObj->value(key);
	if (!v.isUndefined())
	{
		ret = v.toString();
	}
	return ret;
}

bool JsonDeploy::updataKeyValueFromSets(QString key, QString value)
{
	QJsonObject::iterator it = s_jsonObj->find(key);
	if (it == s_jsonObj->end())
	{
		qDebug() << "updataKeyValueFromSets faile";
		return false;
	}

	if (*it != value)
	{
		*it = value;
		s_isContentChanged = true;
	}
	return true;
}


//第一次加一条记录，用于初始化
void JsonDeploy::addKeyValueToSets(QString key, QString value)
{
	s_jsonObj->insert(key, QJsonValue(value));
}

//写一个总的获取配置的接口，避免以后每个字段都需要写一个读写接口
QString JsonDeploy::getKeyValueFromLongSets(QString key)
{
	QString ret;
	QJsonValue v = s_jsonObj->value(key);
	if (!v.isUndefined())
	{
		ret = v.toString();
	}
	return ret;
}

bool JsonDeploy::updataKeyValueFromLongSets(QString key, QString& value)
{
	if (value.size() > 10240)
	{
		return false;
	}

	QJsonObject::iterator it = s_jsonObj->find(key);
	if (it == s_jsonObj->end())
	{
		qDebug() << "updataKeyValueFromSets faile";
		return false;
	}
	if (*it != value)
	{
		*it = value;
		s_isContentChanged = true;
	}
	return true;
}


//第一次加一条记录，用于初始化
void JsonDeploy::addKeyValueToLongSets(QString key, QString value)
{
	if (value.size() > 10240)
	{
		return;
	}

	s_jsonObj->insert(key, QJsonValue(value));
}

//写一个总的获取配置的接口，避免以后每个字段都需要写一个读写接口.0做默认值，最后不用0做值
int JsonDeploy::getKeyValueFromNumSets(const QString key)
{
	int ret = 0;

	QJsonValue v = s_jsonObj->value(key);
	if (!v.isUndefined())
	{
		ret = v.toInt();
	}
	return ret;
}

bool JsonDeploy::updataKeyValueFromNumSets(const QString key, int value)
{

	QJsonObject::iterator it = s_jsonObj->find(key);
	if (it == s_jsonObj->end())
	{
		qDebug() << "updataKeyValueFromNumSets faile";
		return false;
	}
	if (*it != value)
	{
		*it = value;
		s_isContentChanged = true;
	}
	return true;
}


//第一次加一条记录，用于初始化
void JsonDeploy::addKeyValueToNumSets(QString key, int value)
{
	s_jsonObj->insert(key, QJsonValue(value));
}


void JsonDeploy::close()
{
	if (s_reference > 0)
	{
		--s_reference;

		if (s_reference == 0)
		{
			s_isExistDb = false;

			//做一次真正的保存
			if (s_isContentChanged)
			{
				QJsonValue v = s_jsonObj->value("dbpath");
				if (v.isString())
				{
					QString dbPath = v.toString();
					QFile file(dbPath);
					if (!file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
					{
						//如果失败，则是权限不够
						qDebug() << "Error: Failed to save json set file.";
						return;
					}

					QJsonDocument doc(*s_jsonObj);
					QByteArray bytes = doc.toJson(); //是uft8编码的
					file.write(bytes);
					file.close();
					s_isContentChanged = false;
				}
			}
		}
	}
}

