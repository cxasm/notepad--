#pragma once
#include <QSettings>
#include <QVariant>

static QString SKIN_KEY = "skinid";
static QString LANGS_KEY = "langs";
static QString AUTOWARP_KEY = "warp";
static QString INDENT_KEY = "indent";
static QString SHOWSPACE_KEY = "blank";
static QString MAX_BIG_TEXT = "maxtsize";
static QString SOFT_STATUS = "rstatus";
static QString SOFT_KEY = "rkey";
static QString RESTORE_CLOSE_FILE = "restore"; //恢复关闭时打开的文件
static QString PRO_DIR = "prodir";//放置配置文件的路径
//static QString RESTORE_SIZE = "rsize";//保存关闭时的大小
static QString WIN_POS = "pos";//保存关闭是的大小
static QString ICON_SIZE = "iconsize";//图标大小
static QString ZOOMVALUE = "zoom"; //放大倍数
static QString VERSION = "version";//当前版本
static QString FINDRESULTPOS = "findpos";//查找窗口悬浮的位置
static QString FILELISTPOS = "filepos";//文件列表悬浮框的位置
static QString FILELISTSHOW = "showfilelist"; //文件列表框是否显示
static QString TOOLBARSHOW = "showbar"; //是否显示工具栏
static QString FINDWINSIZE = "findwinsize";//保存查找框的大小。150%放大时界面永久。保留避免每次手动调整
static QString SHOWWEBADDR = "showweb";//高亮web地址，双击网页打开
static QString FIND_RESULT_FONT_SIZE = "frfs";//查找结果的默认字体大小，默认为14


//下面这个是winpos.ini中的key，避免单个文件太大，拖慢启动速度
static QString BATCH_FIND_REPLACE_POS = "bfpos";//批量查找替换窗口的大小

class NddSetting
{
public:
	static bool checkNoExistAdd(QString key, QVariant & value);

	static void init();

	static QString getKeyValueFromSets(QString key);

	static bool updataKeyValueFromSets(QString key, QString  value);

	static QByteArray getKeyByteArrayValue(QString key);

	static void updataKeyByteArrayValue(QString key, QByteArray & value);

	static void addKeyValueToSets(QString  key, QString  value);

	static int getKeyValueFromNumSets(const QString key);

	static bool updataKeyValueFromNumSets(const QString key, int value);

	static void addKeyValueToNumSets(QString key, int value);

	static void close();

	static QByteArray getWinPos(QString key);

	static void updataWinPos(QString key, QByteArray& value);

	

	static bool isDbExist()
	{
		return s_isExistDb;
	}
private:
	static void winPosInit();
private:

	static bool s_isExistDb;
	static bool s_isContentChanged;
	static int s_reference;

	static QSettings* s_nddSet;

	static QSettings* s_winPosSet;
};
