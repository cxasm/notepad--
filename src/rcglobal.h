#pragma once

#include <QString>
#include <QVector>
#include <QTreeWidgetItem>
#include <qscilexer.h>

//#define TEST_PRE

#ifdef TEST_PRE
static const char* VersionStr = u8"(内部测试非稳定) v1.22.1";
#else

static const char* VersionStr = "v1.22.2";
#endif // TEST_PRE


#define NO_PLUGIN 1

#define CMP_CODE_NOEQUAL

enum RC_DIRECTION
{
	RC_LEFT = 0,
	RC_RIGHT,
};

enum RC_CMP_RESULT
{
	RC_RESULT_EQUAL=0,
	RC_RESULT_NOEQUAL,
};

enum RC_FILE_TYPE
{
	RC_FILE = 1000,
	RC_DIR,
	RC_PAD_FILE
};

enum RC_ITEM_STATUS
{
	RC_COLLAPSED = 0,
	RC_EXPANDED,
};

/* 是放置在block中的userState，-1是保留行，因为-1是默认没有时的值，标识新插入行 */
enum RC_LINE_FORM
{
	PAD_LINE = -2,//对齐行 
	//-1没有使用QTextBlock::userState()的默认值
	UNKNOWN_LINE =0, //未知就是没有换行符号。当做没有
	UNIX_LINE,
	DOS_LINE,
	MAC_LINE,
};


enum CODE_ID {
	UNKOWN = -2,//其实应该是ANSI中的非GBK编码。暂时不考虑其它国家语言编码，则直接按照ASCII进行字节处理
	ANSI = -1,
	UTF8_NOBOM,//如果是这种，其实需要确定到底椒UTF8 还是ANSI
	UTF8_BOM, //UTF8_WITH BOM
	UNICODE_LE,
	UNICODE_BE,
	GBK,
	//增加国际化的几种语言
	EUC_JP,//日本鬼子
	Shift_JIS,//日文另外一种
	EUC_KR,//韩国
	KOI8_R,//俄罗斯
	TSCII,//泰国
	TIS_620,//泰文
	BIG5,//繁体中文
	CODE_END //最后一个标志,在UI上是显示一个UNKNOWN，这是一个特殊
};


typedef struct fileAttriNode_ {
	QString relativePath;//不带/而且不带最外层目录路径
	int type; //file or dirs，pad
	//int index; //用于表示先后顺序，用于向前向后的排序查找
	QTreeWidgetItem* parent; //父节点
	QTreeWidgetItem* selfItem; //如果是目录，则标记自己的节点
	fileAttriNode_()
	{
		parent = nullptr;
	}
	fileAttriNode_(QString relativePath_)
	{
		relativePath = relativePath_;
	}

	bool operator==(const fileAttriNode_& other) const
	{
		return (relativePath.compare(other.relativePath) == 0);
	}

}fileAttriNode;



const int MARGIN_NONE = 0;
const int MARGIN_SYNC_BT = 1;
const int MARGIN_SYNC_BT_BIT_MASK = 0x2;

const int MARGIN_VER_LINE = 2;
const int MARGIN_VER_LINE_BIT_MASK = 0x4;


const int MARGIN_LINE_NUM = 3;

enum WORK_STATUS
{
	FREE_STATUS = 0,
	CMP_WORKING
};


#define OPEN_UNDO_REDO 1

#ifdef OPEN_UNDO_REDO

class BlockUserData;


struct WalkFileInfo {
	int direction;
	QTreeWidgetItem* root;
	QString path;
	WalkFileInfo(int dire_, QTreeWidgetItem* root_, QString path_) :direction(dire_), root(root_), path(path_)
	{

	}
};

const int Item_RelativePath = Qt::ToolTipRole;
const int Item_Index = Qt::UserRole + 1;
const int DIR_ITEM_MAXSIZE_FILE = Qt::UserRole + 2;
#endif

//两种母版。一种是txt可自定义关键字。一种是CPP，可自定义关键字。
enum UserLangMother
{
	MOTHER_NONE = LangType::L_USER_TXT,
	MOTHER_CPP,
};

QString getUserLangDirPath();

//在这定义一次即可。
//#define uos 1

#ifdef Q_OS_WIN
#undef uos
#endif

#ifdef ubu
#undef uos
#endif

#ifdef uos
#undef ubu
#endif

#ifdef Q_OS_MAC
#undef uos
#endif

void showFileInExplorer(QString path);
QString tranFileSize(qint64 fileSize);
