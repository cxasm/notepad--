#pragma once

#include "common.h"
#include "rcglobal.h"

#include <QString>
#include <QObject>
#include <QList>

class ScintillaEditView;
class ScintillaHexEditView;

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
	HexFileMgr() :file(nullptr), fileOffset(0), lineSize(64), fileSize(0), contentBuf(nullptr), contentRealSize(0)
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
	TextFileMgr() :file(nullptr), fileOffset(0), lineSize(64), fileSize(0), contentBuf(nullptr), contentRealSize(0)
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

class CCNotePad;

class FileManager:public QObject
{
	Q_OBJECT
public:
	ScintillaEditView* newEmptyDocument();

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

	bool loadFileData(QString filePath, TextFileMgr *& textFileOut);

	HexFileMgr* getHexFileHand(QString filepath);

	void closeHexFileHand(QString filepath);

	void closeBigTextFileHand(QString filepath);

	LangType detectLanguageFromTextBegining(const unsigned char * data, size_t dataLen);

	static FileManager& getInstance() {
		static FileManager instance;
		return instance;
	};

private:
	FileManager() = default;
	~FileManager();

	FileManager(const FileManager&) = delete;
	FileManager& operator=(const FileManager&) = delete;

	QList<NewFileIdMgr> m_newFileIdList;

	QMap<QString, HexFileMgr*> m_hexFileMgr;

	QMap<QString, TextFileMgr*> m_bigTxtFileMgr;
};

