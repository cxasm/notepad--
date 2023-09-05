#include "renamewin.h"
#include "progresswin.h"

#include <QFileDialog>
#include <QRadioButton> 
#include <QMessageBox>
#include <QInputDialog>

//批量修改文件夹下文件名称的功能

ReNameWin::ReNameWin(QWidget *parent)
	: QWidget(parent), m_extComBoxNum(0)
{
	ui.setupUi(this);

	connect(ui.radioButtonAddPrefix, &QRadioButton::toggled, this, [this](bool status) {slot_renameOptionsChange(1,status); });
	connect(ui.radioButtonDelPrefix, &QRadioButton::toggled, this, [this](bool status) {slot_renameOptionsChange(2, status); });
	connect(ui.radioButtonAddSuffix, &QRadioButton::toggled, this, [this](bool status) {slot_renameOptionsChange(3, status); });
	connect(ui.radioButtonDelSuffix, &QRadioButton::toggled, this, [this](bool status) {slot_renameOptionsChange(4, status); });
	connect(ui.radioButtonLower, &QRadioButton::toggled, this, [this](bool status) {slot_renameOptionsChange(5, status); });
	connect(ui.radioButtonUpper, &QRadioButton::toggled, this, [this](bool status) {slot_renameOptionsChange(6, status); });
}

ReNameWin::~ReNameWin()
{
}


void ReNameWin::slot_selectDir()
{
	QString workDir = ui.lineEditDir->text();
	QString rootpath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), workDir, QFileDialog::DontResolveSymlinks);

	if (!rootpath.isEmpty())
	{
		ui.lineEditDir->setText(rootpath);
	}
}


//改名，前缀后缀名称的修改
void ReNameWin::slot_renameOptionsChange(int id, bool status)
{
	switch (id)
	{
	case 1:
		ui.lineEditAddPrefix->setEnabled(status);
		break;
	case 2:
		ui.lineEditDelPrefix->setEnabled(status);
		break;
	case 3:
		ui.lineEditAddSuffix->setEnabled(status);
		break;
	case 4:
		ui.lineEditDelSuffix->setEnabled(status);
		break;
	default:
		break;
	}
}

void ReNameWin::slot_userDefineExt()
{
	bool ok = false;
	QString text = QInputDialog::getText(this, tr("input file ext()"), tr("ext (Start With .)"), QLineEdit::Normal, QString(".cpp"), &ok);

	if (ok && !text.isEmpty())
	{
		text = text.trimmed();
		ui.comboBoxExt->addItem(text);

		++m_extComBoxNum;

		ui.comboBoxExt->setCurrentIndex(m_extComBoxNum);
	}
}

void ReNameWin::slot_startRename()
{
	if (ui.optionsTab->currentIndex() == 0)
	{
		changeFileName();
	}
	else if (ui.optionsTab->currentIndex() == 1)
	{
		changeFileExt();
	}
	
}

void ReNameWin::changeFileName()
{
	QString dealDir = ui.lineEditDir->text();
	if (dealDir.isEmpty())
	{
		QMessageBox::warning(this, tr("Notice"), tr("Please Select Dir"));
		return;
	}

	int type = -1;
	QString extrenFileName;

	bool addPrefix = ui.radioButtonAddPrefix->isChecked();
	if (addPrefix && ui.lineEditAddPrefix->text().isEmpty())
	{
		QMessageBox::warning(this, tr("Notice"), tr("Please Input Add File Prefix"));
		return;
	}
	if (addPrefix)
	{
		type = 0;
		extrenFileName = ui.lineEditAddPrefix->text();
	}

	bool delPrefix = ui.radioButtonDelPrefix->isChecked();
	if (delPrefix && ui.lineEditDelPrefix->text().isEmpty())
	{
		QMessageBox::warning(this, tr("Notice"), tr("Please Input Del File Prefix"));
		return;
	}
	if (delPrefix)
	{
		type = 1;
		extrenFileName = ui.lineEditDelPrefix->text();
	}

	bool addSuffix = ui.radioButtonAddSuffix->isChecked();
	if (addSuffix && ui.lineEditAddSuffix->text().isEmpty())
	{
		QMessageBox::warning(this, tr("Notice"), tr("Please Input Add File Suffix"));
		return;
	}
	if (addSuffix)
	{
		type = 2;
		extrenFileName = ui.lineEditAddSuffix->text();
	}

	bool delSuffix = ui.radioButtonDelSuffix->isChecked();
	if (delSuffix && ui.lineEditDelSuffix->text().isEmpty())
	{
		QMessageBox::warning(this, tr("Notice"), tr("Please Input Del File Suffix"));
		return;
	}
	if (delSuffix)
	{
		type = 3;
		extrenFileName = ui.lineEditDelSuffix->text();
	}

	bool toLowerFileName = ui.radioButtonLower->isChecked();
	if (toLowerFileName)
	{
		type = 4;
	}

	bool toUpperFileName = ui.radioButtonUpper->isChecked();
	if (toUpperFileName)
	{
		type = 5;
	}

	if (type == -1)
	{
		QMessageBox::warning(this, tr("Notice"), tr("Please Select One Operator"));
		return;
	}

	QList<QString> dirsList;
	dirsList.append(dealDir);

	int fileNums = 0;
	int failNums = 0;
	bool dealChildDir = ui.checkBoxDealChildDir->isChecked();

	QString oldName;
	QString newName;

	auto getNewName = [](QString oldName, int type, const QString& addOrDelFix)->QString {

		QFileInfo fi(oldName);
		QDir dir = fi.absoluteDir();


		switch (type)
		{
			//增加前缀
		case 0:
			return QString("%1/%2.%3").arg(dir.absolutePath()).arg(addOrDelFix+fi.baseName()).arg(fi.suffix());
		case 1:
		{//删除前缀
			if (fi.baseName().startsWith(addOrDelFix) && (fi.baseName() != addOrDelFix))
			{
				return QString("%1/%2.%3").arg(dir.absolutePath()).arg(fi.baseName().mid(addOrDelFix.length())).arg(fi.suffix());
			}
		}
		break;
		case 2:
		{
			//增加后缀
			return QString("%1/%2.%3").arg(dir.absolutePath()).arg(fi.baseName()+ addOrDelFix).arg(fi.suffix());
		}
			break;
		case 3:
			//删除后缀
		{
			if (fi.baseName().endsWith(addOrDelFix) && (fi.baseName() != addOrDelFix))
			{
				return QString("%1/%2.%3").arg(dir.absolutePath()).arg(fi.baseName().mid(0, fi.baseName().length() - addOrDelFix.length())).arg(fi.suffix());
			}
		}
				
			break;
		case 4:
			return QString("%1/%2.%3").arg(dir.absolutePath()).arg(fi.baseName().toLower()).arg(fi.suffix());
		case 5:
			return QString("%1/%2.%3").arg(dir.absolutePath()).arg(fi.baseName().toUpper()).arg(fi.suffix());
		default:
			break;
		}
		return QString();
	};

	ProgressWin* m_loadFileProcessWin = new ProgressWin(this);

	m_loadFileProcessWin->setWindowModality(Qt::WindowModal);

	m_loadFileProcessWin->info(tr("rename file in progress, please wait ..."));

	m_loadFileProcessWin->show();

	int processTotal = 0;
	bool isExistChildDir = false;

	while (!dirsList.isEmpty())
	{
		QString path = dirsList.takeFirst();

		/*添加path路径文件*/
		QDir dir(path);

		//遍历各级子目录

		if (dealChildDir)
		{
			QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks);   //获取当前所有目录

			for (int i = 0; i != folder_list.size(); ++i)         //自动递归添加各目录到上一级目录
			{
				QString namepath = folder_list.at(i).absoluteFilePath();    //获取路径
				//必须放前面，因为以文件夹个数处理为进度条计数
				dirsList.push_front(namepath);
			}
			if ((processTotal == 0) && dirsList.size() > 0)
			{
				//以目录个数大概去统计进度
				processTotal = dirsList.size();
				m_loadFileProcessWin->setTotalSteps(processTotal);

				isExistChildDir = true;
			}

			if (dirsList.size() < processTotal)
			{
				m_loadFileProcessWin->moveStep();
			}
		}

		QDir dir_file(path);
		dir_file.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);//获取当前所有文件
		QFileInfoList list_file = dir_file.entryInfoList();

		//说明没有子文件夹
		if ((processTotal == 0) && list_file.size() > 0)
		{
			processTotal = list_file.size();
			m_loadFileProcessWin->setTotalSteps(processTotal);
		}

		for (int i = 0; i < list_file.size(); ++i)
		{
			QFileInfo fileInfo = list_file.at(i);
			oldName = fileInfo.absoluteFilePath();

			newName = getNewName(oldName,type, extrenFileName);

			if (!newName.isEmpty() && (newName != oldName))
			{
				if (!QFile::rename(oldName, newName))
				{
					failNums++;
					m_loadFileProcessWin->info(tr("failed %1 file path %2, please check").arg(failNums).arg(oldName));
				}
				fileNums++;
			}

			//没有子文件夹时，按文件数量走
			if (!isExistChildDir)
			{
				m_loadFileProcessWin->moveStep();
			}
		}
	}

	QMessageBox::information(this, tr("Notice"), tr("Deal Finished, totol %1 files, failed %2 files").arg(fileNums).arg(failNums));

	delete m_loadFileProcessWin;
}

void ReNameWin::changeFileExt()
{
	QString dealDir = ui.lineEditDir->text();
	QString destExt = ui.lineEditDestExt->text();
	if (destExt.startsWith('.'))
	{
		destExt = destExt.mid(1, destExt.length() - 1);
	}

	if (dealDir.isEmpty() || destExt.isEmpty())
	{
		QMessageBox::warning(this, tr("Notice"), tr("Please Select Dir Or Dest Ext"));
		return;
	}

	QString filterExt;
	bool isNeedFilterExt = false;

	if (0 != ui.comboBoxExt->currentIndex())
	{
		filterExt = ui.comboBoxExt->currentText();

		if (filterExt.startsWith('.'))
		{
			filterExt = filterExt.mid(1, filterExt.length() - 1);
		}
	}
	//检查是否需要过滤只处理ext类型
	if (!filterExt.isEmpty())
	{
		isNeedFilterExt = true;
	}

	QList<QString> dirsList;
	dirsList.append(dealDir);

	int fileNums = 0;
	int failNums = 0;
	bool dealChildDir = ui.checkBoxDealChildDir->isChecked();

	QString oldName;
	QString newName;

	auto getNewName = [](QString oldName, const QString& destExt)->QString{

		QFileInfo fi(oldName);

		//没有后缀名，则不修改
		if (fi.suffix().isEmpty())
		{
			return QString();
		}
		int oldExtSize = fi.suffix().length();

		return QString("%1%2").arg(oldName.mid(0, oldName.size()-oldExtSize)).arg(destExt);
	};

	ProgressWin* m_loadFileProcessWin = new ProgressWin(this);
	
	m_loadFileProcessWin->setWindowModality(Qt::WindowModal);

	m_loadFileProcessWin->info(tr("rename file in progress, please wait ..."));

	m_loadFileProcessWin->show();

	int processTotal = 0;
	bool isExistChildDir = false;

	while (!dirsList.isEmpty())
	{
		QString path = dirsList.takeFirst();

		/*添加path路径文件*/
		QDir dir(path);

		//遍历各级子目录

		if (dealChildDir)
		{
			QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks);   //获取当前所有目录

			for (int i = 0; i != folder_list.size(); ++i)         //自动递归添加各目录到上一级目录
			{
				QString namepath = folder_list.at(i).absoluteFilePath();    //获取路径
				//必须放前面，因为以文件夹个数处理为进度条计数
				dirsList.push_front(namepath);
			}
			if ((processTotal == 0) && dirsList.size() > 0)
			{
				//以目录个数大概去统计进度
				processTotal = dirsList.size();
				m_loadFileProcessWin->setTotalSteps(processTotal);

				isExistChildDir = true;
			}

			if (dirsList.size() < processTotal)
			{
				m_loadFileProcessWin->moveStep();
			}
		}

		QDir dir_file(path);
		dir_file.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);//获取当前所有文件
		QFileInfoList list_file = dir_file.entryInfoList();

		//说明没有子文件夹
		if ((processTotal == 0) && list_file.size() > 0)
		{
			processTotal = list_file.size();
			m_loadFileProcessWin->setTotalSteps(processTotal);
		}

		for (int i = 0; i < list_file.size(); ++i)
		{  
			QFileInfo fileInfo = list_file.at(i);
			oldName = fileInfo.absoluteFilePath();

			if (isNeedFilterExt)
			{
				if (fileInfo.suffix() != filterExt)
				{
					continue;
				}
			}

			newName = getNewName(oldName, destExt);

			if (!newName.isEmpty() && (newName != oldName))
			{
				if (!QFile::rename(oldName, newName))
				{
					failNums++;
					m_loadFileProcessWin->info(tr("failed %1 file path %2, please check").arg(failNums).arg(oldName));
				}
			}

			//没有子文件夹时，按文件数量走
			if (!isExistChildDir)
			{
				m_loadFileProcessWin->moveStep();
			}
		}

		fileNums += list_file.size();
	}

	QMessageBox::information(this, tr("Notice"), tr("Deal Finished, totol %1 files, failed %2 files").arg(fileNums).arg(failNums));

	delete m_loadFileProcessWin;
}
