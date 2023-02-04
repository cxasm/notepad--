#include "encodeconvert.h"
#include "rcglobal.h"
#include "CmpareMode.h"
#include "doctypelistview.h"

#include <QFileDialog>
#include <QTreeWidgetItem>
#include <QDateTime>
#include <QFutureWatcher>
#include <QString>
#include <QtConcurrent>
#include <QInputDialog>
#include <QDragEnterEvent>


const int ITEM_CODE = Qt::UserRole + 1;

static QString fileSuffix(const QString& filePath)
{
	QFileInfo fi(filePath);
	return fi.suffix();
}

static QString getFileSizeFormat(qint64 size)
{
#if 0
	if (size <= 1000)
	{
		return QString("%1").arg(size);
	}

	QString fileSize = QString("%1").arg(size);

	return QString("%1,%2").arg(fileSize.left(fileSize.count() - 3)).arg(fileSize.right(3));
#endif
	return QString::number(size);
}

EncodeConvert::EncodeConvert(QWidget *parent): QWidget(parent), m_commitCmpFileNums(0), m_finishCmpFileNums(0), m_menu(nullptr)
{
	ui.setupUi(this);

	m_extComBoxNum = 0;
	connect(ui.treeWidget, &QTreeWidget::itemPressed, this, &EncodeConvert::slot_itemClicked);

	setAcceptDrops(true);
}

EncodeConvert::~EncodeConvert()
{

	for (auto var : m_supportFileExt)
	{
		delete var;
	}

	m_supportFileExt.clear();
}

bool EncodeConvert::isSupportExt(int index, QString ext)
{
	bool ret = false;

	if (0 == index)
	{
		ret = DocTypeListView::isSupportExt(ext);
	}
	else if (index >= 1)
	{
		int i = index - 1;

		if (i < m_supportFileExt.count())
		{
			ret = m_supportFileExt[i]->contains(ext);
		}
	}

	return ret;
}

//右键菜单
void EncodeConvert::slot_itemClicked(QTreeWidgetItem* item, int /*column*/)
{
	if ((item != nullptr) && (Qt::RightButton == QGuiApplication::mouseButtons()))
	{

		if (m_menu == nullptr)
		{
			m_menu = new QMenu(this);
			m_menu->addAction(tr("&Show File in Explorer..."), this, [&]() {
				QString path, cmd;

				QTreeWidgetItem* it = ui.treeWidget->currentItem();
				if (it == nullptr)
				{
					return;
				}

				path = QString("%1").arg(it->data(0, Qt::ToolTipRole).toString());
				showFileInExplorer(path);
				});
		}
		m_menu->move(QCursor::pos());
		m_menu->show();
	}
}

//用户自定义类型
void EncodeConvert::slot_userDefineExt()
{
	bool ok = false;
	QString text = QInputDialog::getText(this, tr("input file ext()"),tr("ext (Split With :)"), QLineEdit::Normal, QString(".h:.cpp"), &ok);

	if (ok && !text.isEmpty())
	{
		text = text.trimmed();
		ui.extComboBox->addItem(text);

		QStringList extList = text.split(":");

		QMap<QString, bool>* p = new QMap<QString, bool>;

		for (QString var : extList)
		{
			if (var.startsWith("."))
			{
				p->insert(var.mid(1), true);
			}
		}

		m_supportFileExt.append(p);

		++m_extComBoxNum;

		ui.extComboBox->setCurrentIndex(m_extComBoxNum);
	}
}

//打开文件目录
void EncodeConvert::slot_selectFile()
{
	//加载左边的文件树
	QString rootpath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QString(), QFileDialog::DontResolveSymlinks);

	if (!rootpath.isEmpty())
	{
		ui.treeWidget->clear();

		m_fileAttris.clear();

		loadDir(rootpath);

		setItemIntervalBackground();

		scanFileCode();
	}

	
}

int EncodeConvert::allfile(QTreeWidgetItem* root_, QString path_)
{
	QList<WalkFileInfo> dirsList;
	WalkFileInfo oneDir(0, root_, path_);
	dirsList.append(oneDir);

	int fileNums = 0;

	m_fileDirPath = path_;

	while (!dirsList.isEmpty())
	{
		WalkFileInfo curDir = dirsList.first();
		dirsList.pop_front();

		QTreeWidgetItem* root = curDir.root;
		QString path = curDir.path;
		int direction = curDir.direction;

		/*添加path路径文件*/
		QDir dir(path);          //遍历各级子目录

		//先获取文件到列表
		//再获取文件夹到列表
		QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);   //获取当前所有目录

		for (int i = 0; i != folder_list.size(); ++i)         //自动递归添加各目录到上一级目录
		{
			QString namepath = folder_list.at(i).absoluteFilePath();    //获取路径
			QFileInfo folderinfo = folder_list.at(i);
			QString name = folderinfo.fileName();      //获取目录名

			QTreeWidgetItem* childroot = new QTreeWidgetItem(QStringList() << name);
			childroot->setIcon(0, QIcon(":/Resources/img/dir.png"));
			root->addChild(childroot);              //将当前目录添加成path的子项

			fileAttriNode node;
			node.type = RC_DIR;//是目录
			node.selfItem = childroot;

			node.parent = root;
			node.relativePath = folderinfo.absoluteFilePath();
			//把路径名称保存到tips中，后续需要这个来排序，下同
			childroot->setData(0, Qt::ToolTipRole, node.relativePath);

			m_fileAttris.append(node);

			WalkFileInfo oneDir(direction, childroot, namepath);

			dirsList.push_front(oneDir);
		}

		QDir dir_file(path);
		dir_file.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);//获取当前所有文件
		QFileInfoList list_file = dir_file.entryInfoList();
		for (int i = 0; i < list_file.size(); ++i)
		{  //将当前目录中所有文件添加到treewidget中
			QFileInfo fileInfo = list_file.at(i);

			QString name2 = fileInfo.fileName();
			QTreeWidgetItem* child = new QTreeWidgetItem(QStringList() << name2);
			child->setIcon(0, QIcon(":/Resources/img/point.png"));

			child->setText(1, getFileSizeFormat(fileInfo.size()));

			/*QString lastModifyTime = fileInfo.lastModified().toString("yy/MM/dd hh:mm:ss");
			child->setText(2, lastModifyTime);*/
			root->addChild(child);

			fileAttriNode node;
			node.type = RC_FILE;//是文件
			node.selfItem = child;

			node.parent = root;
			node.relativePath = fileInfo.absoluteFilePath();
			//把路径名称保存到tips中，后续需要这个来排序，下同
		
			child->setData(0, Qt::ToolTipRole, node.relativePath);

			m_fileAttris.append(node);

		}

		fileNums += list_file.size();
	}

	return fileNums;
}


int EncodeConvert::loadDir(QString rootDirPath)
{

	QString rootpath = rootDirPath;

	QTreeWidgetItem* root = nullptr;

	int fileNums = 0;

	ui.treeWidget->setColumnWidth(0, 400);
	ui.treeWidget->clear();
	

	root = new QTreeWidgetItem(ui.treeWidget);
	root->setText(0, rootpath);
	root->setExpanded(true);

	//第一个节点是目录根节点
	fileAttriNode node;
	node.type = RC_DIR;//是目录
	node.selfItem = root;
	node.parent = nullptr;
	node.relativePath = ".";

	m_fileAttris.append(node);

	fileNums = allfile(root, rootpath);
	
	
	return fileNums;
}

QFuture<EncodeThreadParameter*> EncodeConvert::commitTask(std::function<EncodeThreadParameter* (EncodeThreadParameter*)> fun, EncodeThreadParameter* parameter)
{
	/* 这里最开始准备使用信号提交多线程，但是发现std:;function无法使用槽函数机制，需要自己是实现元对象
	* 直接使用QtConcurrent::run机制，不仅简单许多，而且在网上看了资料
	*/
	return QtConcurrent::run(fun, parameter);
}


//对比左右文件的大小，sha1值来判断文件是否相等
QFuture<EncodeThreadParameter_*> EncodeConvert::checkFileCode(QString filePath, QTreeWidgetItem* item)
{
	EncodeThreadParameter_* p = new EncodeThreadParameter_(filePath);
	p->item = item;

	//int 0相等 1 不等
	return commitTask([](EncodeThreadParameter_* parameter)->EncodeThreadParameter_*
		{
			parameter->code = CmpareMode::scanFileRealCode(parameter->filepath);
			return parameter;
		}
	, p);

}


CODE_ID EncodeConvert::convertFileToCode(QString& filePath, CODE_ID srcCode, CODE_ID dstCode)
{

	if (srcCode == CODE_ID::UNKOWN)
	{
		return CODE_ID::UNKOWN;
	}

	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly | QIODevice::ExistingOnly))
	{
		return CODE_ID::UNKOWN;
	}

	QByteArray content = file.readAll();

	file.close();

	int skip = 0;

	switch (srcCode)
	{
	case UNKOWN:
		break;
	case ANSI:
		break;
	case UNICODE_LE:
		skip = 2;
		break;
	case UNICODE_BE:
		skip = 2;
		break;
	case UTF8_NOBOM:
		break;
	case UTF8_BOM:
		skip = 3;
		break;
	case GBK:
		break;
	default:
		break;
	}

	if (!file.open(QIODevice::WriteOnly | QIODevice::ExistingOnly | QIODevice::Truncate))
	{
		return CODE_ID::UNKOWN;
	}


	QByteArray text2Save;

	if (skip == 2 && content.size() >= 2)
	{
		text2Save = QByteArray(content.mid(2));
	}
	else if (skip == 3 && content.size() >= 3)
	{
		text2Save = QByteArray(content.mid(3));
	}
	else
	{
		text2Save = QByteArray(content);
	}

	QString textOut;

	Encode::tranStrToUNICODE(srcCode, text2Save.data(), text2Save.size(), textOut);
	 

	if (dstCode != UNKOWN)
	{
		//QByteArray codeFlag = Encode::getEncodeStartFlagByte(dstCode);

		//20210822 发现大坑，转换到一定格式后，字符串前面自动带了标识，不需要再来检查一次
		//if (!codeFlag.isEmpty())
		//{
		//	//先写入标识头
		//	file.write(codeFlag);
		//}

		//如果编码是已知如下类型，则后续保存其它行时，不修改编码格式，继续按照原编码进行保存
		//前面已经设置过编码了，这里不需要再设置
		if (dstCode == CODE_ID::UTF8_BOM)
		{
			//自动转换不会带UTF-8 BOM，所以自己要在前面写个BOM头。这是一个例外。需要手动写入头
			//其他必然BL LE则不需要。
			QByteArray codeFlag = Encode::getEncodeStartFlagByte(dstCode);

			if (!codeFlag.isEmpty())
			{
				//先写入标识头
				file.write(codeFlag);
			}
		}

		if (textOut.length() > 0)
		{
			//保存时注意编码问题。这个tolocal已经带了字符BOM头了。只要UTF8_BOM不会带
			QByteArray t = textOut.toLocal8Bit();
			file.write(textOut.toLocal8Bit());
		}
	}

	file.close();

	return dstCode;
}

CODE_ID EncodeConvert::getComboBoxCode(int index){
	CODE_ID ret = CODE_ID::UNKOWN;

	if (index < CODE_END)
	{
		ret = (CODE_ID)index;
	}
	return ret;
};

QFuture<EncodeThreadParameter_*> EncodeConvert::convertFileCode(QString filePath, QTreeWidgetItem* item)
{
	EncodeThreadParameter_* p = new EncodeThreadParameter_(filePath);
	p->item = item;

	CODE_ID srcCode = static_cast<CODE_ID>(item->data(0, ITEM_CODE).toInt());

	CODE_ID dstCode = getComboBoxCode(ui.codeToComboBox->currentIndex());


	//int 0相等 1 不等
	return commitTask([=](EncodeThreadParameter_* parameter)->EncodeThreadParameter_*
		{
			if (dstCode != CODE_ID::UNKOWN)
			{
				parameter->code = convertFileToCode(parameter->filepath, srcCode, dstCode);
			}
			else
			{
				parameter->code = UNKOWN;
			}
			return parameter;
		}
	, p);

}

//20220114 仅仅使用第一行失败编码还是不行，因为utf8和gbk其实有相同的编码范围。
//如果识别第一行为gbk的，则直接使用gbk。但是如果识别为utf8的，则需要识别更多的文本内容，这样会更慢
void EncodeConvert::scanFileCode()
{
	m_finishCmpFileNums = 0;
	m_commitCmpFileNums = 0;

	ui.selectFileBt->setEnabled(false);
	ui.startBt->setEnabled(false);
	ui.closeBt->setEnabled(false);

	ui.logTextBrowser->clear();

	ui.logTextBrowser->append(tr("start scan file text code, please wait..."));

	for (QList<fileAttriNode>::iterator iter = m_fileAttris.begin(); iter != m_fileAttris.end(); ++iter)
	{
		if (iter->type == RC_DIR)
		{
			iter->selfItem->setText(2, QString("--"));
		}
		else if ((iter->type == RC_FILE) && DocTypeListView::isSupportExt(fileSuffix(iter->relativePath)))
		{
			QFutureWatcher<EncodeThreadParameter_*>* futureWatcher = new QFutureWatcher<EncodeThreadParameter_*>();

			QObject::connect(futureWatcher, &QFutureWatcher<EncodeThreadParameter_>::finished, this, &EncodeConvert::slot_scanFileCode);

			futureWatcher->setFuture(this->checkFileCode(iter->relativePath,iter->selfItem));

			++m_commitCmpFileNums;
		}
		else
		{
			iter->selfItem->setText(2, tr("ignore"));
		}
	}

	int finishProcessRatio = 0;

	while (m_finishCmpFileNums < m_commitCmpFileNums)
	{
		int curProcessRatio = m_finishCmpFileNums * 100 / m_commitCmpFileNums;

		//没%5更新一下
		if (curProcessRatio - finishProcessRatio >= 5)
		{
			finishProcessRatio = curProcessRatio;
			ui.logTextBrowser->append(tr("please wait, total file %1,cur scan index %2, scan finish %3%").arg(m_commitCmpFileNums).arg(m_finishCmpFileNums).arg(curProcessRatio));
		}
		QCoreApplication::processEvents();
	}
	ui.logTextBrowser->append(tr("scan finished, total file %1").arg(m_commitCmpFileNums));

	ui.selectFileBt->setEnabled(true);
	ui.startBt->setEnabled(true);
	ui.closeBt->setEnabled(true);
}

//文件对比完毕，显示出文件是否意义，不一样则红色字符标识
void EncodeConvert::slot_scanFileCode()
{
	QFutureWatcher<EncodeThreadParameter_*>* s = dynamic_cast<QFutureWatcher<EncodeThreadParameter_*> *>(sender());

	EncodeThreadParameter_* result = s->result();

	//这里释放的内容，其实是在mode里面new出来的
	if (result != nullptr)
	{
		result->item->setText(2, Encode::getCodeNameById(result->code));

		result->item->setData(0, ITEM_CODE, result->code);

		delete result;
		result = nullptr;
	}

	delete s;
	s = nullptr;

	++m_finishCmpFileNums;
}

void EncodeConvert::slot_startConvert()
{
	int extComboBoxIndex = ui.extComboBox->currentIndex();

	CODE_ID dstCode = getComboBoxCode(ui.codeToComboBox->currentIndex());

	m_finishCmpFileNums = 0;
	m_commitCmpFileNums = 0;
	ui.logTextBrowser->clear();

	//如果编码是已知如下类型，则后续保存其它行时，不修改编码格式，继续按照原编码进行保存

	QString destCodeName = Encode::getQtCodecNameById(dstCode);
	if (destCodeName.isEmpty() || destCodeName == "unknown")
	{
		//这里永远不会走。因为界面上不会有未知选项
		assert(false);
		return;
	}
	else
	{
		QTextCodec::setCodecForLocale(QTextCodec::codecForName(destCodeName.toStdString().c_str()));
	}

	ui.selectFileBt->setEnabled(false);
	ui.codeToComboBox->setEditable(false);
	ui.closeBt->setEnabled(false);


	for (QList<fileAttriNode>::iterator iter = m_fileAttris.begin(); iter != m_fileAttris.end(); ++iter)
	{
		if ((iter->type == RC_FILE) && isSupportExt(extComboBoxIndex, fileSuffix(iter->relativePath)))
		{
			qDebug() << iter->relativePath;

			CODE_ID srcCode = static_cast<CODE_ID>(iter->selfItem->data(0, ITEM_CODE).toInt());

			CODE_ID dstCode = getComboBoxCode(ui.codeToComboBox->currentIndex());

			if (srcCode != dstCode)
			{

				QFutureWatcher<EncodeThreadParameter_*>* futureWatcher = new QFutureWatcher<EncodeThreadParameter_*>();

				QObject::connect(futureWatcher, &QFutureWatcher<EncodeThreadParameter_>::finished, this, &EncodeConvert::slot_convertFileFinish);

				futureWatcher->setFuture(this->convertFileCode(iter->relativePath, iter->selfItem));

				++m_commitCmpFileNums;
			}
			else
			{
				iter->selfItem->setText(4, tr("already %1 ignore").arg(Encode::getCodeNameById(srcCode)));
			}
		}
		else
		{
			iter->selfItem->setText(4, tr("ignore"));
		}
	}

	int finishProcessRatio = 0;

	while (m_finishCmpFileNums < m_commitCmpFileNums)
	{
		int curProcessRatio = m_finishCmpFileNums * 100 / m_commitCmpFileNums;

		//没%5更新一下
		if (curProcessRatio - finishProcessRatio >= 5)
		{
			finishProcessRatio = curProcessRatio;
			ui.logTextBrowser->append(tr("total file %1,cur deal index %2,finish %3%").arg(m_commitCmpFileNums).arg(m_finishCmpFileNums).arg(curProcessRatio));
		}
		QCoreApplication::processEvents();
	}

	ui.logTextBrowser->append(tr("total file %1,cur deal index %2,finish 100%").arg(m_commitCmpFileNums).arg(m_finishCmpFileNums));
	ui.logTextBrowser->append(tr("convert finished !"));

	ui.selectFileBt->setEnabled(true);
	ui.codeToComboBox->setEditable(true);
	ui.closeBt->setEnabled(true);
}


//转换完成，设置当前表格上的显示状态
void EncodeConvert::slot_convertFileFinish()
{
	QFutureWatcher<EncodeThreadParameter_*>* s = dynamic_cast<QFutureWatcher<EncodeThreadParameter_*> *>(sender());

	EncodeThreadParameter_* result = s->result();

	//这里释放的内容，其实是在mode里面new出来的
	if (result != nullptr)
	{
		if (result->code != UNKOWN)
		{
			result->item->setText(3, Encode::getCodeNameById(result->code));
			result->item->setText(4, tr("convert finish"));
		}
		else
		{
			result->item->setText(4, tr("convert fail"));
			ui.logTextBrowser->append(tr("file %1 convert failed,pleas check...").arg(result->item->data(0, Qt::ToolTipRole).toString()));
		}

		result->item->setData(0, ITEM_CODE, result->code);

		delete result;
		result = nullptr;
	}

	delete s;
	s = nullptr;

	++m_finishCmpFileNums;
}

//对item进行间隔着色
void EncodeConvert::setItemIntervalBackground()
{
	int curItemIndex = 0;

	QTreeWidgetItemIterator it(ui.treeWidget);
	while (*it) {
		if (curItemIndex % 2 == 1)
		{
			setItemBackground(*it, QColor(0xf8faf9));
		}
		++it;
		++curItemIndex;
	}
}

void EncodeConvert::setItemBackground(QTreeWidgetItem* item, const QColor& color)
{
	QBrush b(color);
	item->setBackground(0, b);
	item->setBackground(1, b);
	item->setBackground(2, b);
	item->setBackground(3, b);
	item->setBackground(4, b);
}

void EncodeConvert::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasFormat("text/uri-list")) //只能打开文本文件
	{
		event->accept(); //可以在这个窗口部件上拖放对象
	}
	else
	{
		event->ignore();
	}

}

void EncodeConvert::dropEvent(QDropEvent* e)
{
	QList<QUrl> urls = e->mimeData()->urls();
	if (urls.isEmpty())
		return;

	QString dirName = urls.first().toLocalFile();

	if (dirName.isEmpty())
	{
		return;
	}

	QDir dir(dirName);

	if (!dir.exists())
	{
		ui.logTextBrowser->append(tr("please drop a file dir ..."));
		return;
	}

	ui.treeWidget->clear();

	m_fileAttris.clear();

	loadDir(dirName);

	setItemIntervalBackground();

	scanFileCode();

	e->accept();

}
