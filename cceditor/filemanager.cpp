#include "filemanager.h"
#include "scintillaeditview.h"
#include "scintillahexeditview.h"
#include "CmpareMode.h"
#include "ccnotepad.h"

#include <QMessageBox>
#include <QFile>
#include <QtGlobal>
#include <qscilexer.h>


FileManager::~FileManager()
{
}

ScintillaEditView* FileManager::newEmptyDocument()
{
	ScintillaEditView* pEdit = new ScintillaEditView(nullptr);
	return pEdit;
}

ScintillaHexEditView* FileManager::newEmptyHexDocument()
{
	ScintillaHexEditView* pEdit = new ScintillaHexEditView(nullptr);
	return pEdit;
}

//从尾部找前面的换行符号。返回的是需要回溯的个数
int findLineEndPos(const char* buf, int size)
{
	int ret = 0;
	bool isfound = false;
	for (int i = size - 1; i >= 0; --i)
	{
		if (buf[i] == '\n')
		{
			isfound = true;
			break;
		}
		++ret;
	}

	//如果没有找到，怀疑是mac格式，按照\r结尾解析
	if (!isfound)
	{
		for (int i = size - 1; i >= 0; --i)
		{
			if (buf[i] == '\r')
			{
				isfound = true;
				break;
			}
			++ret;
		}
	}

	if (isfound)
	{
		return ret;
	}

	//说明是一个巨长的行，这种情况不是很好。直接读取了，不管是否进行了行截断
	return 0;
}

//从行首找后面的换行符号。返回的是需要前进的个数，即把前面掐掉一节，让返回在一行的行首位置
int findLineStartPos(const char* buf, int size)
{
	int ret = 0;
	bool isfound = false;
	for (int i = 0; i < size; ++i)
	{
		++ret;
		if (buf[i] == '\n')
		{
			isfound = true;
			break;
		}
	}

	//如果没有找到，怀疑是mac格式，按照\r结尾解析
	if (!isfound)
	{
		for (int i = size - 1; i >= 0; --i)
		{
			++ret;
			if (buf[i] == '\r')
			{
				isfound = true;
				break;
			}
		}
	}

	if (isfound)
	{
		return ret;
	}

	//说明是一个巨长的行，这种情况不是很好。直接读取了，不管是否进行了行截断
	return 0;
}


//返回第一个空闲的id，m_newFileIdList必须有序，从小到大，从0开始
//序号也从0开始，notepad++是从1开始
int FileManager::getNextNewFileId()
{
	if (m_newFileIdList.isEmpty())
	{
		return 0;
	}

	int index = 0;
	bool isFind = false;

	for (int i = 0; i < m_newFileIdList.size(); ++i)
	{
		if (m_newFileIdList.at(i).index > i)
		{
			index = i;
			isFind = true;
			break;
		}
	}

	if (!isFind)
	{
		index = m_newFileIdList.size();
	}

	return index;
}

//务必要保证不能重复id，所以newnode后，必须接着调用insertNewFileNode，避免重复了
void FileManager::insertNewFileNode(NewFileIdMgr node)
{
	m_newFileIdList.append(node);
	std::sort(m_newFileIdList.begin(), m_newFileIdList.end(), [](NewFileIdMgr& a, NewFileIdMgr& b) {
		return a.index < b.index;
	});
}

//删除newfile id的节点
void FileManager::delNewFileNode(int fileIndex)
{
	for (int i = 0; i < m_newFileIdList.size(); ++i)
	{
		if (m_newFileIdList.at(i).index == fileIndex)
		{
			m_newFileIdList.removeAt(i);
			break;
		}
	}
}

//这里是以文本方式加载文件。但是可能遇到的是二进制文件，里面会做判断
//二进制时hexAsk是否询问，当用户指定打开格式时，不需要询问
int FileManager::loadFileDataInText(ScintillaEditView* editView, QString filePath, CODE_ID& fileTextCode, RC_LINE_FORM& lineEnd,CCNotePad * callbackObj, bool hexAsk)
{
	QFile file(filePath);


	//如果文件不存在，直接返回
	if (!file.exists())
	{
		return -1;
	}

	QFlags<QFileDevice::Permission> power = QFile::permissions(filePath);

#if 0
	if (!power.testFlag(QFile::ReadOwner))
	{
		//文件不能读
		QMessageBox::warning(nullptr, tr("Error"), tr("Open File %1 failed Can not read auth").arg(filePath));
		
		return 1;
	}
#endif

	//直接以只读的方式打开，至于能不能保存，是保存时需要考虑的问题。
	//只需要在保存的时候获取admin权限即可
	QIODevice::OpenMode mode;
	
	mode = QIODevice::ExistingOnly | QIODevice::ReadOnly;

#if 0
	if (!power.testFlag(QFile::WriteUser))
	{
		//文件不能写
		mode = QIODevice::ExistingOnly | QIODevice::ReadOnly;
	}
	else
	{
		mode = QIODevice::ExistingOnly | QIODevice::ReadWrite;
	}
#endif

	if (!file.open(mode))
	{
		qDebug() << file.error();
#ifdef Q_OS_WIN
		//打开失败，这里一般是权限问题导致。如果是windows，在外面申请权限后继续处理
		if (QFileDevice::OpenError == file.error())
			{
			if (callbackObj != nullptr)
			{
				return callbackObj->runAsAdmin(filePath);
			}
			return 1;
			}
#endif
#ifdef Q_OS_UNIX
		QMessageBox::warning(nullptr, tr("Error"), tr("Open File %1 failed").arg(filePath));
#endif
		return 2;
	}

	qint64 fileSize = file.size();

	qint64 bufferSizeRequested = fileSize + qMin((qint64)(1 << 20), (qint64)(fileSize / 6));

	// As a 32bit application, we cannot allocate 2 buffer of more than INT_MAX size (it takes the whole address space)
	if (bufferSizeRequested > INT_MAX)
	{
		QMessageBox::warning(nullptr, tr("Error"), tr("File is too big to be opened by Notepad--"));
		file.close();
		return 3;
	}

	QList<LineFileInfo> outputLineInfoVec;

	int maxLineSize = 0;
	int charsNums = 0;
	bool isHexFile = false;

	fileTextCode = CmpareMode::scanFileOutPut(fileTextCode,filePath, outputLineInfoVec, maxLineSize, charsNums, isHexFile);

	if (isHexFile && hexAsk)
	{
		//检测到文件很可能是二进制文件，询问用户，是否以二进制加载
		int ret = QMessageBox::question(nullptr, tr("Open with Text or Hex?"), tr("The file %1 is likely to be binary. Do you want to open it in binary?").arg(filePath), tr("Hex Open"), tr("Text Open"), tr("Cancel"));
		
		if (ret == 0)
		{
			//16进制打开
			file.close();
			return 4;
		}
		else if (ret == 1)
		{
			//继续以文本打开
	}
		else
		{
			//取消，不打开
			file.close();
			return 2;
		}
	}

	if (maxLineSize > 0)
	{
		editView->execute(SCI_SETSCROLLWIDTH, maxLineSize*10);
	}

	//以第一行的换行为文本的换行符
	lineEnd = UNKNOWN_LINE;

	if (!outputLineInfoVec.isEmpty())
	{
		lineEnd = static_cast<RC_LINE_FORM>(outputLineInfoVec.at(0).lineEndFormat);
	}

	if (lineEnd == UNKNOWN_LINE)
	{
#ifdef _WIN32
		lineEnd = DOS_LINE;
#else
		lineEnd = UNIX_LINE;
#endif
	}
	QString text;
	text.reserve(charsNums + 1);

	for (QList<LineFileInfo>::iterator it = outputLineInfoVec.begin(); it != outputLineInfoVec.end(); ++it)
	{
		text.append(it->unicodeStr);
	}

	file.close();

	//利用前面5行，进行一个编程语言的判断
	QString headContens;

	for (int i = 0; (i < outputLineInfoVec.size() && i < 5); ++i)
	{
		headContens.append(outputLineInfoVec.at(i).unicodeStr);
	}

	std::string headstr = headContens.toStdString();

	LangType _language = detectLanguageFromTextBegining((const unsigned char *)headstr.data(), headstr.length());

	if (_language>= 0 && _language < L_EXTERNAL)
	{
		//editView->execute(SCI_SETLEXER, ScintillaEditView::langNames[_language].lexerID);
		QsciLexer* lexer = editView->createLexer(_language);
		editView->setLexer(lexer);
	}
	
	//如果检测到时16进制文件，但是强行以二进制打开，则有限走setUtf8Text。
	if (!isHexFile)
	{
	editView->setText(text);
	}
	else
	{
		//这种情况，为了不编辑二进制模式，是可能只读的。
		if (1 == editView->setUtf8Text(text))
		{
			return 5;//只读模式
		}
	}
	
	return 0;
}

//加载文件，只为查找使用
int FileManager::loadFileForSearch(ScintillaEditView* editView, QString filePath)
{
	QFile file(filePath);

	QFlags<QFileDevice::Permission> power = QFile::permissions(filePath);

	if (!power.testFlag(QFile::ReadOwner))
	{
		//文件不能读
		return 1;
	}

	QIODevice::OpenMode mode;

	if (!power.testFlag(QFile::WriteOwner))
	{
		//文件不能写
		mode = QIODevice::ExistingOnly | QIODevice::ReadOnly;
	}
	else
	{
		mode = QIODevice::ExistingOnly | QIODevice::ReadWrite;
	}

	if (!file.open(mode))
	{
		qDebug() << file.error();
		return 2;
	}

	qint64 fileSize = file.size();

	qint64 bufferSizeRequested = fileSize + qMin((qint64)(1 << 20), (qint64)(fileSize / 6));

	// As a 32bit application, we cannot allocate 2 buffer of more than INT_MAX size (it takes the whole address space)
	if (bufferSizeRequested > INT_MAX)
	{
		file.close();
		return 3;
	}

	QList<LineFileInfo> outputLineInfoVec;

	int maxLineSize = 0;
	int charsNums = 0;
	bool isHexFile = false;

	CODE_ID fileTextCode = CODE_ID::UNKOWN;

	CmpareMode::scanFileOutPut(fileTextCode, filePath, outputLineInfoVec, maxLineSize, charsNums, isHexFile);

	if (isHexFile)
	{
		qDebug() << filePath;
		file.close();
		return 4;
	}

	if (maxLineSize > 0)
	{
		editView->execute(SCI_SETSCROLLWIDTH, maxLineSize * 10);
	}

	QString text;
	text.reserve(charsNums + 1);

	for (QList<LineFileInfo>::iterator it = outputLineInfoVec.begin(); it != outputLineInfoVec.end(); ++it)
	{
		text.append(it->unicodeStr);
	}

	file.close();

	editView->setText(text);

	return 0;
}

const int ONE_PAGE_BYTES = 4096;

//加载下一页或者上一页。(二进制模式）
int  FileManager::loadFilePreNextPage(int dir, QString& filePath, HexFileMgr* & hexFileOut)
{
	if (m_hexFileMgr.contains(filePath))
	{
		hexFileOut = m_hexFileMgr.value(filePath);

		qint64 pos = hexFileOut->fileOffset;

		if (dir == 1 && (pos >= 0))
		{
			//上一页
			pos = pos - hexFileOut->contentRealSize - ONE_PAGE_BYTES;
			if (pos < 0)
			{
				pos = 0;
			}
		}
		else if(dir == 2 && (pos < hexFileOut->fileSize))
		{
		
		}
		else
		{
			return 1;
		}

		char* buf = new char[ONE_PAGE_BYTES+1];

		hexFileOut->file->seek(pos);
		qint64 ret = hexFileOut->file->read(buf, ONE_PAGE_BYTES);
		if (ret <= 0)
		{
			return -1;
		}
		else
		{
			//读取成功
			hexFileOut->fileOffset = hexFileOut->file->pos();

			if (hexFileOut->contentBuf != nullptr)
			{
				delete[]hexFileOut->contentBuf;
			}

			hexFileOut->contentBuf = buf;
			hexFileOut->contentRealSize = ret;
		}
		return 0;
	}
	return -1;
}

const int ONE_PAGE_TEXT_SIZE = 200 * 1024;

//加载下一页或者上一页。(文本模式）
int  FileManager::loadFilePreNextPage(int dir, QString& filePath, TextFileMgr* & textFileOut)
{
	if (m_bigTxtFileMgr.contains(filePath))
	{
		textFileOut = m_bigTxtFileMgr.value(filePath);

		qint64 pos = textFileOut->fileOffset;

		int canReadSize = 0;

		if (dir == 1 && (pos >= 0))
		{
			//读取上一页
			pos = pos - textFileOut->contentRealSize - ONE_PAGE_TEXT_SIZE;
			if (pos < 0)
			{
				//前面的内容不足以ONE_PAGE_TEXT_SIZE字节
				canReadSize = textFileOut->fileOffset - textFileOut->contentRealSize;
				if (canReadSize <= 0)
				{
					return 1;
				}
				pos = 0;
			}
			else
			{
				canReadSize = ONE_PAGE_TEXT_SIZE;
			}
		}
		else if (dir == 2 && (pos < textFileOut->fileSize))
		{
			canReadSize = ONE_PAGE_TEXT_SIZE;
		}
		else
		{
			return 1;
		}

		char* buf = new char[canReadSize + 1];
		buf[canReadSize] = '\0';

		textFileOut->file->seek(pos);
		qint64 ret = textFileOut->file->read(buf, canReadSize);
		if (ret <= 0)
		{
			return -1;
		}
		else
		{
			//读取成功
			//如果是往后读取
			if (dir == 2)
			{
				//读取了1M的内容，从内容尾部往前查找，找到第一个换行符号。如果没有怎么办？说明是一个巨长的行，不妙
				buf[ret] = '\0';

				int preLineEndPos = 0;
				
				if (textFileOut->file->pos() < textFileOut->fileSize)//反之已经到尾部了，不需要往前找行首了
				{
					preLineEndPos = findLineEndPos(buf, ret);
					if (preLineEndPos > 0)
					{
						//给后面的字符填\0，让字符串正常结尾\0
						buf[ret - preLineEndPos] = '\0';
					}
				}

				textFileOut->fileOffset = textFileOut->file->pos() -preLineEndPos;
				if (preLineEndPos > 0)
				{
					//文件seek到下一行的首位置。
					textFileOut->file->seek(textFileOut->fileOffset);
				}

				if (textFileOut->contentBuf != nullptr)
				{
					delete[]textFileOut->contentBuf;
				}

				textFileOut->contentBuf = buf;
				textFileOut->contentRealSize = ret - preLineEndPos;
			}
			else if (dir == 1)
			{
				//如果是往前读取
				//读取了1M的内容，往内容前面往后查找，找到第一个换行符号。如果没有怎么办？说明是一个巨长的行，不妙
				buf[ret] = '\0';

				int preLineStartPos = 0;

				if (textFileOut->file->pos() > canReadSize)//==canReadSize说明已经在文件最前面了。不在最前面，需要
				{
					preLineStartPos = findLineStartPos(buf, ret);
					if (preLineStartPos > 0)
					{
						//把\n前面的内容去掉，通过内存move的方式。
						memmove(buf, buf+preLineStartPos,ret - preLineStartPos);
						buf[ret - preLineStartPos] = '\0';
					}
				}

				textFileOut->fileOffset = textFileOut->file->pos();

				if (textFileOut->contentBuf != nullptr)
				{
					delete[]textFileOut->contentBuf;
				}

				textFileOut->contentBuf = buf;
				textFileOut->contentRealSize = ret - preLineStartPos;
			}
		}
		return 0;
	}
	return -1;
}

//从指定地址开始加载文件
int FileManager::loadFileFromAddr(QString filePath, qint64 addr, HexFileMgr* & hexFileOut)
{
	if (m_hexFileMgr.contains(filePath))
	{
		hexFileOut = m_hexFileMgr.value(filePath);

		//超过文件大小
		if (addr < 0 || addr >= hexFileOut->fileSize)
		{
			return -2;
		}

		//4K对齐
		addr &= 0xfffffffffff0;

		char* buf = new char[ONE_PAGE_BYTES + 1];
		hexFileOut->file->seek(addr);

		qint64 ret = hexFileOut->file->read(buf, ONE_PAGE_BYTES);
		if (ret <= 0)
		{
			return -1;
		}
		else
		{
			//读取成功
			hexFileOut->fileOffset = hexFileOut->file->pos();

			if (hexFileOut->contentBuf != nullptr)
			{
				delete[]hexFileOut->contentBuf;
			}

			hexFileOut->contentBuf = buf;
			hexFileOut->contentRealSize = ret;
		}
		return 0;
	}

	return -1;
}

//从指定地址开始加载文本文件
int FileManager::loadFileFromAddr(QString filePath, qint64 addr, TextFileMgr* & textFileOut)
{
	if (m_bigTxtFileMgr.contains(filePath))
	{
		textFileOut = m_bigTxtFileMgr.value(filePath);

		//超过文件大小
		if (addr < 0 || addr >= textFileOut->fileSize)
		{
			return -2;
		}

		char* buf = new char[ONE_PAGE_TEXT_SIZE + 1];
		buf[ONE_PAGE_TEXT_SIZE] = '\0';

		textFileOut->file->seek(addr);

		qint64 ret = textFileOut->file->read(buf, ONE_PAGE_TEXT_SIZE);
		if (ret <= 0)//-1是出错。0也是没有读到
		{
			return -1;
		}
		else
		{
		
			int preLineEndPos = 0;
			buf[ret] = '\0';

			if (textFileOut->file->pos() < textFileOut->fileSize)//反之已经到尾部了，不需要往前找行了
			{
				preLineEndPos = findLineEndPos(buf, ret);
				if (preLineEndPos > 0)
				{
					//给后面的字符填\0，让字符串正常结尾\0
					buf[ret - preLineEndPos] = '\0';
				}
			}

			int preLineStartPos = findLineStartPos(buf, ret);
			if (preLineStartPos > 0 && preLineStartPos < ret) //preLineStartPos如果大于ret，则全部都被跳过了，不会显示，是个特例
			{
				memmove(buf, buf + preLineStartPos, ret - preLineStartPos);
				buf[ret - preLineStartPos] = '\0';
			}
			else
			{
				//如果没做调整，则后续不需要偏移，这里必须preLineStartPos赋0值
				preLineStartPos = 0;
			}
			
			//只需要文件调到上一行的行位即可。
			textFileOut->fileOffset = textFileOut->file->pos() - preLineEndPos;
			if (preLineEndPos > 0)
			{
				//文件seek到下一行的首位置。
				textFileOut->file->seek(textFileOut->fileOffset);
			}

			if (textFileOut->contentBuf != nullptr)
			{
				delete[]textFileOut->contentBuf;
			}

			textFileOut->contentBuf = buf;
			textFileOut->contentRealSize = ret - preLineEndPos - preLineStartPos;
		}
		return 0;
	}

	return -1;
}

//加载二进制文件。从curPos开始，每行16个byte，每次读取64行，一共1024个byte
bool FileManager::loadFileData(QString filePath, HexFileMgr* & hexFileOut)
{
	QFile *file = new QFile(filePath);

	if (!file->open(QIODevice::ReadOnly | QIODevice::ExistingOnly))
	{
		return false;
	}

	//小于10k的文件一次性全部读取完毕
	const int LITTLE_FILE_MAX = 10240;

	int readBytes = 0;


	if (file->size() <= LITTLE_FILE_MAX)
	{
		readBytes = LITTLE_FILE_MAX;
	}
	else
	{
		//对于大于10K的文件，每次只读4K
		readBytes = ONE_PAGE_BYTES;
	}

	char* buf = new char[readBytes];
	qint64 ret = file->read(buf, readBytes);
	if (ret == -1)
	{
		//错误
		file->close();
		delete file;
		return false;
	}
	else
	{
		HexFileMgr* hexFile = nullptr;

		if (!m_hexFileMgr.contains(filePath))
		{
			hexFile = new HexFileMgr();
			hexFile->filePath = filePath;
			hexFile->file = file;
			hexFile->fileOffset = file->pos();
			hexFile->fileSize = file->size();
			hexFile->contentBuf = buf;
			hexFile->contentRealSize = ret;
			m_hexFileMgr.insert(filePath, hexFile);
		}
		else
		{
			//理论上这里永远不走
			hexFile = m_hexFileMgr.value(filePath);
			hexFile->fileOffset = file->pos();
			hexFile->contentBuf = buf;
			hexFile->contentRealSize = ret;
		}

		hexFileOut = hexFile;

		return true;
	}

	return false;
}

//加载大文本文件。从0开始读取ONE_PAGE_TEXT_SIZE 500K的内容
bool FileManager::loadFileData(QString filePath, TextFileMgr* & textFileOut)
{
	QFile *file = new QFile(filePath);

	if (!file->open(QIODevice::ReadOnly | QIODevice::ExistingOnly))
	{
		return false;
	}

	int readBytes = ONE_PAGE_TEXT_SIZE;

	char* buf = new char[ONE_PAGE_TEXT_SIZE+1];
	buf[ONE_PAGE_TEXT_SIZE] = '\0';

	qint64 ret = file->read(buf, readBytes);
	if (ret <= 0)
	{
		//错误
		file->close();
		delete file;
		return false;
	}
	else
	{
		//读取了1M的内容，从尾部往找，找到第一个换行符号。如果没有怎么办？说明是一个巨长的行，不妙
		buf[ret] = '\0';

		int preLineEndPos = findLineEndPos(buf,ret);
		if (preLineEndPos > 0)
		{
			//给后面的字符填\0，让字符串正常结尾\0
			buf[ret - preLineEndPos] = '\0';
		}

		TextFileMgr* txtFile = nullptr;

		if (!m_bigTxtFileMgr.contains(filePath))
		{
			txtFile = new TextFileMgr();
			txtFile->filePath = filePath;
			txtFile->file = file;
			txtFile->fileOffset = file->pos() - preLineEndPos;
			if (preLineEndPos > 0)
			{
				//文件seek到下一行的首位置。下次读的时候，头部肯定是一行的行首啦
				file->seek(txtFile->fileOffset);
			}
			txtFile->fileSize = file->size();
			txtFile->contentBuf = buf;
			txtFile->contentRealSize = ret - preLineEndPos;
			m_bigTxtFileMgr.insert(filePath, txtFile);
		}
		else
		{
			//理论上这里永远不走
			assert(false);
			txtFile = m_bigTxtFileMgr.value(filePath);
			txtFile->fileOffset = file->pos();
			txtFile->contentBuf = buf;
			txtFile->contentRealSize = ret;
		}

		textFileOut = txtFile;

		return true;
	}

	return false;
}

HexFileMgr * FileManager::getHexFileHand(QString filepath)
{
	if (m_hexFileMgr.contains(filepath))
	{
		return m_hexFileMgr.value(filepath);
	}

	return nullptr;
}

void FileManager::closeHexFileHand(QString filepath)
{
	if (m_hexFileMgr.contains(filepath))
	{
		HexFileMgr* v = m_hexFileMgr.value(filepath);
		v->destory();
		delete v;
		m_hexFileMgr.remove(filepath);
	}
}

void FileManager::closeBigTextFileHand(QString filepath)
{
	if (m_bigTxtFileMgr.contains(filepath))
	{
		TextFileMgr* v = m_bigTxtFileMgr.value(filepath);
		v->destory();
		delete v;
		m_bigTxtFileMgr.remove(filepath);
	}
}


//检查文件的编程语言
LangType FileManager::detectLanguageFromTextBegining(const unsigned char *data, size_t dataLen)
{
	struct FirstLineLanguages
	{
		std::string pattern;
		LangType lang;
	};

	// Is the buffer at least the size of a BOM?
	if (dataLen <= 3)
		return L_TXT;

	// Eliminate BOM if present
	size_t i = 0;
	if ((data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF) || // UTF8 BOM
		(data[0] == 0xFE && data[1] == 0xFF && data[2] == 0x00) || // UTF16 BE BOM
		(data[0] == 0xFF && data[1] == 0xFE && data[2] == 0x00))   // UTF16 LE BOM
		i += 3;

	// Skip any space-like char
	for (; i < dataLen; ++i)
	{
		if (data[i] != ' ' && data[i] != '\t' && data[i] != '\n' && data[i] != '\r')
			break;
	}

	// Create the buffer to need to test
	const size_t longestLength = 40; // shebangs can be large
	std::string buf2Test = std::string((const char *)data + i, longestLength);

	// Is there a \r or \n in the buffer? If so, truncate it
	auto cr = buf2Test.find("\r");
	auto nl = buf2Test.find("\n");
	auto crnl = qMin(cr, nl);
	if (crnl != std::string::npos && crnl < longestLength)
		buf2Test = std::string((const char *)data + i, crnl);

	// First test for a Unix-like Shebang
	// See https://en.wikipedia.org/wiki/Shebang_%28Unix%29 for more details about Shebang
	std::string shebang = "#!";

	size_t foundPos = buf2Test.find(shebang);
	if (foundPos == 0)
	{
		// Make a list of the most commonly used languages
        const size_t NB_SHEBANG_LANGUAGES = 7;
		FirstLineLanguages ShebangLangs[NB_SHEBANG_LANGUAGES] = {
			{ "sh",		L_BASH },
			{ "python", L_PYTHON },
			{ "perl",	L_PERL },
			{ "php",	L_PHP },
			{ "ruby",	L_RUBY },
            { "node",	L_JAVASCRIPT },
            { "Makefile",	L_MAKEFILE}
		};

		// Go through the list of languages
		for (i = 0; i < NB_SHEBANG_LANGUAGES; ++i)
		{
			if (buf2Test.find(ShebangLangs[i].pattern) != std::string::npos)
			{
				return ShebangLangs[i].lang;
			}
		}

		// Unrecognized shebang (there is always room for improvement ;-)
		return L_TXT;
	}

	// Are there any other patterns we know off?
	const size_t NB_FIRST_LINE_LANGUAGES = 5;
	FirstLineLanguages languages[NB_FIRST_LINE_LANGUAGES] = {
		{ "<?xml",			L_XML },
		{ "<?php",			L_PHP },
		{ "<html",			L_HTML },
		{ "<!DOCTYPE html",	L_HTML },
		{ "<?",				L_PHP } // MUST be after "<?php" and "<?xml" to get the result as accurate as possible
	};

	for (i = 0; i < NB_FIRST_LINE_LANGUAGES; ++i)
	{
		foundPos = buf2Test.find(languages[i].pattern);
		if (foundPos == 0)
		{
			return languages[i].lang;
		}
	}

	// Unrecognized first line, we assume it is a text file for now
	return L_UNKNOWN;
}
