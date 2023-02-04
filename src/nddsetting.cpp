#include "nddsetting.h"
#include <QObject>
#include <QtDebug>
#include <QCoreApplication>
#include <QDate>
#include <QFile>
#include <QStorageInfo>
#include <QSettings>

static short version_num = 29; //1.22.0 是29
//配置文件是否存在。如果初始化失败，则不存在
bool NddSetting::s_isExistDb = false;
int NddSetting::s_reference = 0;
bool NddSetting::s_isContentChanged = false;

QSettings* NddSetting::s_nddSet = nullptr;

QSettings* NddSetting::s_winPosSet = nullptr;

//如果key不存在，则新增key-value。存在：返回true
bool NddSetting::checkNoExistAdd(QString key, QVariant& value)
{
	//每次新加字段后，这里要检查一下，不存在则新增
	if (!s_nddSet->contains(key))
	{
		s_nddSet->setValue(key, value);

		if (!s_isContentChanged)
		{
			s_isContentChanged = true;
		}
		return false;
	}
	return true;
}

//20220402这里隐含了一个前置条件：数据库句柄是在主线程创建的，最好不要在子线程中调用。
//避免因为跨线程访问和多线程冲突访问引发的闪退问题。所以最好配置文件的读写都在主线程中进行
void NddSetting::init()
{
	++s_reference;

	//如果已经初始化过了，则直接返回
	if (s_isExistDb)
	{
		return;
	}
	QString settingDir = QString("notepad/nddsets");
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, settingDir);
	QString qsSetPath = qs.fileName();

	s_nddSet = new QSettings(QSettings::IniFormat, QSettings::UserScope, settingDir);
	s_nddSet->setIniCodec("UTF-8");
	bool initOk = true;

	auto initNddSet = []() {
		QString key = "signdate";
		//QString date = QDate::currentDate().toString(QString("yyyy/M/d"));
		//不写今天的时间，否则第一次运行，无条件不会发送心跳。
		//直接写一个过去的时间，让第一次运行，总是要签到
		addKeyValueToSets(key, "2022/2/20");

		QString str;

		//tab的长度，默认为4
		addKeyValueToNumSets("tablens", 4);
		//space replace tab空格替换tab，默认0, 1为true,0为false
		addKeyValueToNumSets("tabnouse", 0);

		addKeyValueToSets("mac", "0");
		addKeyValueToNumSets("padtimes", 0);
		addKeyValueToNumSets("serverip", 0);

		//是否高亮不同处背景
		addKeyValueToNumSets("hexhigh", 1);

		addKeyValueToNumSets(VERSION, version_num);

		//皮肤id
        addKeyValueToNumSets(SKIN_KEY, 0);

		//语言index 0:自动选择 1:中文 2 英文
		addKeyValueToNumSets(LANGS_KEY, 0);

		//开启自动换行
		addKeyValueToNumSets(AUTOWARP_KEY, 0);

		//开启自动缩进
		addKeyValueToNumSets(INDENT_KEY, 0);

		//显示空白
		addKeyValueToNumSets(SHOWSPACE_KEY, 0);

		//最大文本文件的门限。默认100M.(50-300)
		addKeyValueToNumSets(MAX_BIG_TEXT, 100);

		addKeyValueToSets(SOFT_KEY, "0");

		addKeyValueToSets(RESTORE_CLOSE_FILE, "1");

		//0 24 1 36 2 48
		addKeyValueToNumSets(ICON_SIZE, 1);

		addKeyValueToNumSets(ZOOMVALUE, 100);
	
		addKeyValueToNumSets(FINDRESULTPOS, Qt::BottomDockWidgetArea);

		addKeyValueToNumSets(FILELISTPOS, Qt::LeftDockWidgetArea);

		//默认0不显示
		addKeyValueToNumSets(FILELISTSHOW, 0);

		//默认显示工具栏
		addKeyValueToNumSets(TOOLBARSHOW, 1);

		//打开网页，默认不勾选，资源耗费多
		addKeyValueToNumSets(SHOWWEBADDR, 0);

		//查找结果框的默认字体大小
		addKeyValueToNumSets(FIND_RESULT_FONT_SIZE, 14);
	};

	if (!s_nddSet->contains(VERSION))
	{
		//不存在走初始化流程
		initNddSet();
		s_isContentChanged = true;
	}
	else
	{
		do {
			{
				QVariant v(VERSION);
				if (checkNoExistAdd(VERSION, v))
				{
					//如果存在，而且版本是最新,不需要走后面自动检查流程了
					int curVersion = s_nddSet->value(VERSION).toInt();
					if (curVersion == version_num)
					{
						break;
					}
					else if (curVersion < version_num)
					{
						//更新版本到最新
						updataKeyValueFromNumSets(VERSION, version_num);
					}
				}
			}
			//每次新加字段后，这里要检查一下，不存在则新增
			{
				QVariant v(1);
				checkNoExistAdd(SKIN_KEY, v);
			}

			{
				QVariant langs(0);
				checkNoExistAdd(LANGS_KEY, langs);
			}

			{
				QVariant v(0);
				checkNoExistAdd(AUTOWARP_KEY, v);
			}

			{
				QVariant v(0);
				checkNoExistAdd(INDENT_KEY, v);
			}

			{
				QVariant v(0);
				checkNoExistAdd(SHOWSPACE_KEY, v);
			}

			
			{
				QVariant v(100);
				checkNoExistAdd(MAX_BIG_TEXT, v);
			}

			{
				QVariant v(0);
				checkNoExistAdd(SOFT_STATUS, v);
			}

			{
				QVariant v("0");
				checkNoExistAdd(SOFT_KEY, v);
			}

			{
				QVariant v(1);
				checkNoExistAdd(RESTORE_CLOSE_FILE, v);
			}

			{
				QVariant v(1);
				checkNoExistAdd(ICON_SIZE, v);
			}

			{
				QVariant v(100);
				checkNoExistAdd(ZOOMVALUE, v);
			}
			{
				QVariant v(Qt::BottomDockWidgetArea);
				checkNoExistAdd(FINDRESULTPOS, v);
			}

			{
				QVariant v(Qt::LeftDockWidgetArea);
				checkNoExistAdd(FILELISTPOS, v);
			}
			{
				QVariant v(0);
				checkNoExistAdd(FILELISTSHOW, v);
			}
			{
				QVariant v(1);
				checkNoExistAdd(TOOLBARSHOW, v);
			}
			{
				QVariant v(0);
				checkNoExistAdd(SHOWWEBADDR, v);
			}
			{
				QVariant v(14);
				checkNoExistAdd(FIND_RESULT_FONT_SIZE, v);
			}
		} while (false);

	}

	s_isExistDb = initOk;

}



//写一个总的获取配置的接口，避免以后每个字段都需要写一个读写接口
QString NddSetting::getKeyValueFromSets(QString key)
{
	return s_nddSet->value(key,"").toString();
}

bool NddSetting::updataKeyValueFromSets(QString key, QString value)
{
	s_nddSet->setValue(key,value);
	s_isContentChanged = true;
	return true;
}

//写一个总的获取配置的接口，避免以后每个字段都需要写一个读写接口
QByteArray NddSetting::getKeyByteArrayValue(QString key)
{
	return s_nddSet->value(key, "").toByteArray();
}

void NddSetting::updataKeyByteArrayValue(QString key, QByteArray& value)
{
	s_nddSet->setValue(key, QVariant(value));
	s_isContentChanged = true;
}

//第一次加一条记录，用于初始化
void NddSetting::addKeyValueToSets(QString key, QString value)
{
	s_nddSet->setValue(key, QVariant(value));
}


//写一个总的获取配置的接口，避免以后每个字段都需要写一个读写接口.0做默认值，外部最好不用0做初始化值
int NddSetting::getKeyValueFromNumSets(const QString key)
{
	QVariant v = s_nddSet->value(key, QVariant(0));
	return v.toInt();
}

bool NddSetting::updataKeyValueFromNumSets(const QString key, int value)
{
	s_nddSet->setValue(key, QVariant(value));
	return true;
}


//第一次加一条记录，用于初始化
void NddSetting::addKeyValueToNumSets(QString key, int value)
{
	s_nddSet->setValue(key, QVariant(value));
}


void NddSetting::close()
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
				s_nddSet->sync();
				delete s_nddSet;
				s_nddSet = nullptr;
				s_isContentChanged = false;	
			}

			//在这里保存一下子窗口的位置。不排除有可能子窗口还在，主窗口已经退出的情况，不过问题不大。
			if (s_winPosSet != nullptr)
			{
				s_winPosSet->sync();
				s_winPosSet = nullptr;
		}
	}
}
}

//子窗口的位置，单独放在一个winpos.ini文件中，而且启动程序时，不需要读取，可避免启动时拖慢速度
QByteArray NddSetting::getWinPos(QString key)
{
	winPosInit();
	return s_winPosSet->value(key, "").toByteArray();
}

void NddSetting::updataWinPos(QString key, QByteArray& value)
{
	winPosInit();
	s_winPosSet->setValue(key, QVariant(value));
}

void NddSetting::winPosInit()
{
	if (s_winPosSet == nullptr)
	{
		QString settingDir = QString("notepad/delayset");
		QSettings qs(QSettings::IniFormat, QSettings::UserScope, settingDir);
		QString qsSetPath = qs.fileName();

		s_winPosSet = new QSettings(QSettings::IniFormat, QSettings::UserScope, settingDir);
		s_winPosSet->setIniCodec("UTF-8");
	}
}
