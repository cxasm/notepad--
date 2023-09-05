#include "langstyledefine.h"
#include "userlexdef.h"
#include "extlexermanager.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QDebug>



QString LangStyleDefine::s_userLangDirPath = "";

LangStyleDefine::LangStyleDefine(QWidget *parent): QMainWindow(parent)
{
	ui.setupUi(this);

	loadUserLangs();

	connect(ui.curDefineLangCb, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LangStyleDefine::slot_langsChange);
}

LangStyleDefine::~LangStyleDefine()
{}

//isLoadToUI是否加载显示到当前UI界面
bool LangStyleDefine::readLangSetFile(QString langName, bool isLoadToUI)
{
	QString userLangFile = QString("notepad/userlang/%1").arg(langName);//自定义语言中不能有.字符，否则可能有错，后续要检查
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, userLangFile);
	qs.setIniCodec("UTF-8");
	qDebug() << qs.fileName();

	if (!qs.contains("mz"))
	{
		return false;
	}

	if (isLoadToUI)
	{
		//自定义语言格式。
		//mz:ndd
		//name:xxx
		//mother:xxx none/cpp/html 就三种
		//ext:xx xx xx 文件关联后缀名
		//keword:xxx
		ui.keyWordEdit->setPlainText(qs.value("keyword").toString());

		qDebug() << qs.value("keyword").toString();
		ui.extNameLe->setText(qs.value("ext").toString());

		qDebug() << qs.value("ext").toString();
		ui.motherLangCb->setCurrentText(qs.value("mother").toString());
	}

	return true;
}

//查找既有的用户自定义语言配置
void LangStyleDefine::loadUserLangs()
{
	s_userLangDirPath = getUserLangDirPath();

	//遍历文件夹
	QDir dir_file(s_userLangDirPath);
	//dir_file.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);//获取当前所有文件
	QFileInfoList list_file = dir_file.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name);

	bool isFirst = true;
	bool readOk = true;
	for (int i = 0; i < list_file.size(); ++i)
	{  //将当前目录中所有文件添加到treewidget中
		QFileInfo fileInfo = list_file.at(i);

		//这个文件是ext和tag的映射文件，不做配置解析
		if (fileInfo.baseName() == "ext_tag")
		{
			continue;
		}

		if (isFirst)
		{
			readOk = readLangSetFile(fileInfo.baseName(), true);
			if (readOk)
			{
				isFirst = false;
			}
		}
		else
		{
			readOk = readLangSetFile(fileInfo.baseName(), false);
		}

		if (readOk)
		{
		ui.curDefineLangCb->addItem(fileInfo.baseName());
	}
}
}

void LangStyleDefine::slot_new()
{
	QString name = QInputDialog::getText(this, tr("Create New Languages"), tr("Please Input Languages Name"));

	if (!name.isEmpty())
	{
		if (-1 != name.indexOf("."))
		{
			QMessageBox::warning(this, tr("Name Error"), tr("Name can not contains char '.' "));
			return;
		}
		disconnect(ui.curDefineLangCb, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LangStyleDefine::slot_langsChange);

		ui.curDefineLangCb->addItem(name);
		ui.curDefineLangCb->setCurrentIndex(ui.curDefineLangCb->count()-1);
		ui.extNameLe->clear();
		ui.motherLangCb->setCurrentIndex(0);
		ui.keyWordEdit->clear();

		connect(ui.curDefineLangCb, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LangStyleDefine::slot_langsChange);

}
}

void LangStyleDefine::slot_save()
{
	if (ui.extNameLe->text().trimmed().isEmpty())
	{
		QMessageBox::warning(this, tr("Ext is empty"), tr("input ext file tyle. Split with space char"));
		return;
	}

	if (ui.keyWordEdit->toPlainText().trimmed().isEmpty())
	{
		QMessageBox::warning(this, tr("Keyword is empty"), tr("input Keyword. Split with space char"));
		return;
	}

	QString newLangName = ui.curDefineLangCb->currentText().trimmed();

	if (newLangName.isEmpty())
	{
		QMessageBox::warning(this, tr("Language name is empty"), tr("Select Definition Language Text"));
		return;
	}

	QString keywords = ui.keyWordEdit->toPlainText().trimmed();

	int motherLangs = ui.motherLangCb->currentIndex();
	motherLangs += LangType::L_USER_TXT;

	UserLexDef *pCppLexer = new UserLexDef(this);
	pCppLexer->setMotherLang(UserLangMother(motherLangs));
	pCppLexer->setExtFileTypes(ui.extNameLe->text().trimmed());
	pCppLexer->setKeyword(keywords);
	pCppLexer->writeUserSettings(newLangName);

	//把新语言tagName,和关联ext单独存放起来。后面只读取一个文件就能获取所有，避免遍历慢
	QString extsFile = QString("notepad/userlang/ext_tag");//ext_tag是存在所有tag ext的文件
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, extsFile);
	qs.setIniCodec("UTF-8");

	QStringList extList = ui.extNameLe->text().trimmed().split(" ");
	extList.append(QString::number(motherLangs)); //最后一个是mother lexer

	qs.setValue(newLangName, extList);

	//更新当前ExtLexerManager::getInstance()。如果不更新，就要重启软件才能生效
	for (int i = 0, s = extList.size(); i < s; ++i)
	{
		ExtLexerManager::getInstance()->addNewExtType(extList.at(i), LangType(motherLangs), newLangName);
}
	
	ui.statusBar->showMessage(tr("Save %1 language finished !").arg(newLangName), 10000);

}


void LangStyleDefine::slot_langsChange(int index)
{
	QString name = ui.curDefineLangCb->currentText();
	ui.keyWordEdit->clear();
	ui.extNameLe->clear();
	ui.motherLangCb->setCurrentIndex(0);
	readLangSetFile(name,true);
}

//删除当前的语言
void LangStyleDefine::slot_delete()
{
	QString name = ui.curDefineLangCb->currentText();
	//删除该语言

	if (QMessageBox::Yes != QMessageBox::question(this, tr("Delete Language"), tr("Are you sure delete user define lanuage %1").arg(name)))
	{
		return;
	}

	{
		QString userLangFile = QString("notepad/userlang/%1").arg(name);//自定义语言中不能有.字符，否则可能有错，后续要检查
		QSettings qs(QSettings::IniFormat, QSettings::UserScope, userLangFile);
		qs.setIniCodec("UTF-8");

		//删除userlang下面的tag.ini
		QFile::remove(qs.fileName());
	}

	{
		//把新语言在ext_tag中的关联文件记录也删除
		QString extsFile = QString("notepad/userlang/ext_tag");//ext_tag是存在所有tag ext的文件
		QSettings qs(QSettings::IniFormat, QSettings::UserScope, extsFile);
		qs.setIniCodec("UTF-8");

		QStringList extList = qs.value(name).toStringList();
		//更新当前ExtLexerManager::getInstance()。如果不更新，就要重启软件才能生效
		for (int i = 0, s = extList.size(); i < s; ++i)
		{
			ExtLexerManager::getInstance()->remove(extList.at(i));
		}
		qs.remove(name);
	}

	{
		//如果存在自定义的配置，也删除掉
		QString cfgPath = QString("notepad/%1").arg(name);

		QSettings qs(QSettings::IniFormat, QSettings::UserScope, cfgPath);
		if (QFile::exists(qs.fileName()))
		{
			QFile::remove(qs.fileName());
		}
	}

	ui.curDefineLangCb->removeItem(ui.curDefineLangCb->currentIndex());

	ui.statusBar->showMessage(tr("Delete %1 language finished !").arg(name), 10000);
}
