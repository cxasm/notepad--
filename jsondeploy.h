#pragma once

#include <QJsonObject>

static QString SKIN_KEY = "skinid";
static QString LANGS_KEY = "langs";
static QString AUTOWARP_KEY = "warp";
static QString INDENT_KEY = "indent";
static QString MAX_BIG_TEXT = "maxtsize";
static QString SOFT_STATUS = "rstatus";
static QString SOFT_KEY = "rkey";
static QString RESTORE_CLOSE_FILE = "restore"; //恢复关闭时打开的文件
//static QString TXT_FONT = "txtfont";
//static QString PRO_LANG_FONT = "langfont";
static QString PRO_DIR = "prodir";//放置配置文件的路径

class JsonDeploy 
{
//public:
//	JsonDeploy(QObject *parent);
//	virtual ~JsonDeploy();
//
//private:
//	JsonDeploy(const JsonDeploy& other) = delete;
//	JsonDeploy &operator=(const JsonDeploy &other) = delete;


public:
	static QString selectDataPath();

	static void checkNoExistAdd(QString key, QJsonValue & value);

	static void init();

	static QString getKeyValueFromSets(QString key);

	static bool updataKeyValueFromSets(QString key, QString  value);

	static void addKeyValueToSets(QString  key, QString  value);

	static QString getKeyValueFromLongSets(QString key);

	static bool updataKeyValueFromLongSets(QString key, QString& value);

	static void addKeyValueToLongSets(QString key, QString value);

	static int getKeyValueFromNumSets(const QString key);

	static bool updataKeyValueFromNumSets(const QString key, int value);

	static void addKeyValueToNumSets(QString key, int value);

	static void close();

	static bool isDbExist()
	{
		return s_isExistDb;
	}

private:

	static bool s_isExistDb;
	static bool s_isContentChanged;
	static int s_reference;

	static QJsonObject* s_jsonObj;
};
