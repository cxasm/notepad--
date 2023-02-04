#pragma once

#include "common.h"
#include "rcglobal.h"

#include <QString>
#include <QObject>
#include <QList>
#include <QFile>

class ScintillaEditView;
class ScintillaHexEditView;

//小于100k的文件一次性全部读取完毕
const int LITTLE_FILE_MAX = 1024000;

enum ERROR_TYPE {
	NONE_ERROR=-1,
	OPEN_EMPTY_FILE=0,
};

struct NewFileIdMgr {
	int index;
	ScintillaEditView* editView;

	NewFileIdMgr(int index_, ScintillaEditView* editView_):index(index_), editView(editView_)
	{

	}
};

//管理二进制文件的信息
struct HexFileMgr {
	QString filePath;
	QFile* file;
	qint64 fileOffset;
	qint64 fileSize;
	qint16 lineSize;//每次读取多少行，默认为64行，没行16个字符
	char* contentBuf;
	int contentRealSize;
	bool onetimeRead; //是否一次性全部读取到内存。小于等于LITTLE_FILE_MAX的才会全部读取
	HexFileMgr() :file(nullptr), fileOffset(0), lineSize(64), fileSize(0), contentBuf(nullptr), contentRealSize(0),onetimeRead(false)
	{

	}
	void destory()
	{
		if (file != nullptr)
		{
			file->close();
			delete file;
			file = nullptr;
		}
		if (contentBuf != nullptr)
		{
			delete[]contentBuf;
			contentBuf = nullptr;
		}
	}
private:
	HexFileMgr& operator=(const HexFileMgr&) = delete;
	HexFileMgr(const HexFileMgr&) = delete;
};

//管理大文本文件的信息
struct TextFileMgr {
	QString filePath;
	QFile* file;
	qint64 fileOffset;
	qint64 fileSize;
	qint16 lineSize;//每次读取多少行，默认每次读取1024行。但是最大不超过1M的内容。
	char* contentBuf;
	int contentRealSize;
	int loadWithCode;
	int lineEndType;//行尾类型，win linux mac
	
	TextFileMgr() :file(nullptr), fileOffset(0), lineSize(64), fileSize(0), contentBuf(nullptr), contentRealSize(0), loadWithCode(CODE_ID::UNKOWN),lineEndType(RC_LINE_FORM::UNKNOWN_LINE)
	{

	}
	void destory()
	{
		if (file != nullptr)
		{
			file->close();
			delete file;
			file = nullptr;
		}
		if (contentBuf != nullptr)
		{
			delete[]contentBuf;
			contentBuf = nullptr;
		}
	}
private:
	TextFileMgr& operator=(const TextFileMgr&) = delete;
	TextFileMgr(const TextFileMgr&) = delete;
};

struct BlockIndex {
	qint64 fileOffset;//块的开始地址
	quint32 fileSize;//块的大小
	quint32 lineNumStart;//块中行号的开始值
	quint32 lineNum;//块中的行数量
};

//管理大文本文件,可以编辑的信息
struct BigTextEditFileMgr {
	QString filePath;
	QFile* file;
	uchar* filePtr;//使用的是文件映射的方式打开
	quint32 m_curBlockIndex; //当前展示中的块索引序号
	int loadWithCode; //以何种编码来加载解析文件。默认UTF8
	int lineEndType;//行尾类型，win linux mac
	static const qint16 BLOCK_SIZE = 1;//块大小，单位M。开始是4M，发现块越大，行越多，那么在一块中定位行的位置越慢

	QVector<BlockIndex> blocks;//每一块的索引。打开文件的时候，需要建立该索引
	
	BigTextEditFileMgr():filePtr(nullptr), file(nullptr), m_curBlockIndex(0), loadWithCode(CODE_ID::UNKOWN), lineEndType(RC_LINE_FORM::UNKNOWN_LINE)
	{
	}
	void destory()
	{
		if (filePtr != nullptr)
		{
			if (file != nullptr)
			{
				file->unmap(filePtr);
				file->close();
				delete file;
				file = nullptr;
			}
			filePtr = nullptr;
		}
	}
private:
	BigTextEditFileMgr& operator=(const TextFileMgr&) = delete;
	BigTextEditFileMgr(const TextFileMgr&) = delete;
};

class CCNotePad;

class FileManager:public QObject
{
	Q_OBJECT
public:
	ScintillaEditView* newEmptyDocument(bool isBigText = false);

	ScintillaHexEditView * newEmptyHexDocument();

	int getNextNewFileId();

	void insertNewFileNode(NewFileIdMgr node);

	void delNewFileNode(int fileIndex);

	int loadFileDataInText(ScintillaEditView * editView, QString filePath, CODE_ID & fileTextCode, RC_LINE_FORM &lineEnd, CCNotePad * callbackObj=nullptr, bool hexAsk = true);

	int loadFileForSearch(ScintillaEditView * editView, QString filePath);

	//int loadFileData(ScintillaEditView * editView, QString filePath, CODE_ID & fileTextCode, RC_LINE_FORM & lineEnd);

	int loadFilePreNextPage(int dir, QString & filePath, HexFileMgr *& hexFileOut);

	int loadFilePreNextPage(int dir, QString & filePath, TextFileMgr *& hexFileOut);

	int loadFileFromAddr(QString filePath, qint64 addr, HexFileMgr *& hexFileOut);

	int loadFileFromAddr(QString filePath, qint64 addr, TextFileMgr *& hexFileOut);

	bool loadFileData(QString filePath, HexFileMgr * & hexFileOut);

	bool loadFileData(QString filePath, TextFileMgr *& textFileOut, RC_LINE_FORM & lineEnd);

	bool loadFileDataWithIndex(QString filePath, BigTextEditFileMgr*& textFileOut);

	HexFileMgr* getHexFileHand(QString filepath);

	BigTextEditFileMgr* getBigFileEditMgr(QString filepath);

	TextFileMgr* getSuperBigFileMgr(QString filepath);

	int getBigFileBlockId(QString filepath, quint32 lineNum);

	void closeHexFileHand(QString filepath);

	void closeSuperBigTextFileHand(QString filepath);

	void closeBigTextRoFileHand(QString filepath);

	LangType detectLanguageFromTextBegining(const unsigned char * data, size_t dataLen);

	static FileManager& getInstance() {
		static FileManager instance;
		return instance;
	};

	ERROR_TYPE getLastErrorCode()
	{
		return m_lastErrorCode;
	}

	void resetLastErrorCode()
	{
		m_lastErrorCode = NONE_ERROR;
	}

private:
	FileManager();
	~FileManager();
	int createBlockIndex(BigTextEditFileMgr* txtFile);

	FileManager(const FileManager&) = delete;
	FileManager& operator=(const FileManager&) = delete;

	QList<NewFileIdMgr> m_newFileIdList;

	QMap<QString, HexFileMgr*> m_hexFileMgr;

	QMap<QString, TextFileMgr*> m_bigTxtFileMgr;

	QMap<QString, BigTextEditFileMgr*> m_bigTxtEditFileMgr;

	ERROR_TYPE m_lastErrorCode;
};

