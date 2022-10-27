#include "doctypelistview.h"

#include <QInputDialog>
#include <QLineEdit>
#include <QtDebug>
#include <QThread>
#include "jsondeploy.h"

//已知的文档类型

QMap<QString, bool>* DocTypeListView::s_supportFileExts = nullptr;

QStringList DocTypeListView::s_supportFileExt = (QStringList ()<< "common" << "ms int/inf" << "c,c++,objc,go" << "java,c#,pascal" \
<< "web script" << "public script" << "property script" << "fortran,Tex,SQL" << "misc" \
<< "Qt" << "customize");

QMap<QString, bool>* DocTypeListView::s_binFileExts = nullptr;


QStringList DocTypeListView::s_extBindFileType;
#if 0
= (QStringList() << ".txt:.log" << ".ini:.inf" << ".h:.hh:.hpp:.hxx:.c:.cpp:.cxx:.cc:.m:.mm:.vcxproj:.vcproj:.props:vsprops:mainfest:.go:.mod" \
	<< ".java:.cs:.pas:.pp:.inc" << ".html:.htm:.shtml:.shtm:.hta:.asp:.aspx:.css:.js:.json:.jsm:.jsp:.php:.php3:.php4:.php5:.phps:.phpt:.phtml:.xml:.xhtml:.xht:.xul:.kml:.xaml:.xsml" \
	<< ".sh:.bsh:.bash:.bat:.cmd:.nsi:.nsh:.lua:.pl:.pm:.py" << ".rc:.as:.mx:.vb:.vbs" << ".f:.for:.f90:.f95:.f2k:.tex:.sql" \
	<< ".nfo:.mak" << ".ui:.pro:.pri:.qrc" << "");
#endif

QStringList HEX_FILE_EXTS_LIST = (QStringList() << "exe" << "dll" << "png" << "jpg" << "doc" << "docx" << "ppt" << "pptx" \
	<< "zip" << "gz" << "rar" << "pdf" << "7z" << "lib" << "so" << "db" << "obj" << "pdb" << "bmp" << "ico" << "qm" << "icns" << "jpeg" << "res" << "exp" << "ilk" << "deb");

//20220402发现bug:在1.11中引入深入对比后，会在子线程中调用这个初始化函数。引发了后续的sqlite错误。
//务必要保证这些实例和数据库对象，是在主线程中创建的。
void DocTypeListView::initSupportFileTypes()
{
	assert(QThread::currentThread() == qApp->thread());

	if (s_supportFileExts == nullptr)
	{
		s_supportFileExts = new QMap<QString, bool>;
		s_binFileExts = new QMap<QString, bool>;

		for (int i = 0; i < HEX_FILE_EXTS_LIST.size(); ++i)
		{
			s_binFileExts->insert(HEX_FILE_EXTS_LIST.at(i),true);
	}
	}
	else
	{
		//已经初始化过了。避免多次
		return;
	}

	//如果数据库不存在，则使用默认值来进行初始化
	if (!JsonDeploy::isDbExist())
	{
		QStringList types;
		types << ".txt:.log" << ".ini:.inf" << ".h:.hh:.hpp:.hxx:.c:.cpp:.cxx:.cc:.m:.mm:.vcxproj:.vcproj:.props:vsprops:mainfest:.go:.mod" \
			<< ".java:.cs:.pas:.pp:.inc" << ".html:.htm:.shtml:.shtm:.hta:.asp:.aspx:.css:js:.json:.jsm:.jsp:.php:.php3:.php4:.php5:.phps:.phpt:.phtml:.xml:.xhtml:.xht:.xul:.kml:.xaml:.xsml" \
			<< ".sh:.bsh:.bash:.bat:.cmd:.nsi:.nsh:.lua:.pl:.pm:.py" << ".rc:.as:.mx:.vb:.vbs" << ".f:.for:.f90:.f95:.f2k:.tex:.sql" \
			<< "nfo:.mak" << ".ui:.pro:.pri:.qrc";

		QString typeStr = types.join(":");

		QStringList typeList = typeStr.split(":");

		for (QString var : typeList)
		{
			if (!var.isEmpty())
			{
				s_supportFileExts->insert(var.mid(1), true);
			}
		}

		s_extBindFileType << ".txt:.log" << ".ini:.inf" << ".h:.hh:.hpp:.hxx:.c:.cpp:.cxx:.cc:.m:.mm:.vcxproj:.vcproj:.props:vsprops:mainfest:.go:.mod" \
			<< ".java:.cs:.pas:.pp:.inc" << ".html:.htm:.shtml:.shtm:.hta:.asp:.aspx:.css:js:.json:.jsm:.jsp:.php:.php3:.php4:.php5:.phps:.phpt:.phtml:.xml:.xhtml:.xht:.xul:.kml:.xaml:.xsml" \
			<< ".sh:.bsh:.bash:.bat:.cmd:.nsi:.nsh:.lua:.pl:.pm:.py" << ".rc:.as:.mx:.vb:.vbs" << ".f:.for:.f90:.f95:.f2k:.tex:.sql" \
			<< ".nfo:.mak" << ".ui:.pro:.pri:.qrc" << "";
	}
	else
	{
		//从数据库动态读取配置
		QString key("typelist");

		QString typeList = JsonDeploy::getKeyValueFromLongSets(key);

		if (typeList.isEmpty())
		{
			//没有读取到，这是第一次启动软件，没有写入导致。在这里进行第一次的写入

			QStringList types;
			types << ".txt:.log" << ".ini:.inf" << ".h:.hh:.hpp:.hxx:.c:.cpp:.cxx:.cc:.m:.mm:.vcxproj:.vcproj:.props:vsprops:mainfest:.go:.mod" \
				<< ".java:.cs:.pas:.pp:.inc" << ".html:.htm:.shtml:.shtm:.hta:.asp:.aspx:.css:js:.json:.jsm:.jsp:.php:.php3:.php4:.php5:.phps:.phpt:.phtml:.xml:.xhtml:.xht:.xul:.kml:.xaml:.xsml" \
				<< ".sh:.bsh:.bash:.bat:.cmd:.nsi:.nsh:.lua:.pl:.pm:.py" << ".rc:.as:.mx:.vb:.vbs" << ".f:.for:.f90:.f95:.f2k:.tex:.sql" \
				<< ".nfo:.mak" << ".ui:.pro:.pri:.qrc" << "";

			s_extBindFileType = types;

			QString typeStr = types.join("|");

			JsonDeploy::addKeyValueToLongSets(key, typeStr);

			typeStr.replace('|',':');

			QStringList list = typeStr.split(":");

			for (QString var : list)
			{
				if (!var.isEmpty())
				{
					s_supportFileExts->insert(var.mid(1), true);
				}
			}
			
		}
		else
		{
			//从数据库中读取的内容进行动态配置
			s_extBindFileType = typeList.split("|");

			typeList.replace('|', ':');

			QStringList list = typeList.split(":");

			for (QString var : list)
			{
				if (!var.isEmpty())
				{
					s_supportFileExts->insert(var.mid(1), true);
				}
			}
		}
	}
}



//是否支持文件后缀名,注意ext必须要带.xxx格式
bool DocTypeListView::isSupportExt(QString ext)
{
	if (s_supportFileExts == nullptr)
	{
		initSupportFileTypes();
	}

	return s_supportFileExts->contains(ext);
}

//是否是二进制格式的后缀文件
bool DocTypeListView::isHexExt(QString ext)
{
	if (s_binFileExts == nullptr)
	{
		initSupportFileTypes();
	}

	return s_binFileExts->contains(ext);
}

DocTypeListView::DocTypeListView(QWidget *parent)
	: QWidget(parent),m_isDirty(false)
{
	ui.setupUi(this);

	if (s_supportFileExt.isEmpty())
	{
		s_supportFileExt << "common" << "ms int/inf" << "c,c++,objc,go" << "java,c#,pascal" \
			<< "web script" << "public script" << "property script" << "fortran,Tex,SQL" << "misc" \
			<< "Qt" << "customize";
	}

	initSupportFileTypes();
	
#if 0
	if (s_extBindFileType.isEmpty())
	{
		s_extBindFileType << ".txt:.log" << ".ini:.inf" << ".h:.hh:.hpp:.hxx:.c:.cpp:.cxx:.cc:.m:.mm:.vcxproj:.vcproj:.props:vsprops:mainfest:.go:.mod" \
			<< ".java:.cs:.pas:.pp:.inc" << ".html:.htm:.shtml:.shtm:.hta:.asp:.aspx:.css:js:.json:.jsm:.jsp:.php:.php3:.php4:.php5:.phps:.phpt:.phtml:.xml:.xhtml:.xht:.xul:.kml:.xaml:.xsml" \
			<< ".sh:.bsh:.bash:.bat:.cmd:.nsi:.nsh:.lua:.pl:.pm:.py" << ".rc:.as:.mx:.vb:.vbs" << ".f:.for:.f90:.f95:.f2k:.tex:.sql" \
			<< ".nfo:.mak" << ".ui:.pro:.pri:.qrc" << "";
	}
#endif

	ui.extListWidget->addItems(s_supportFileExt);

	connect(ui.extListWidget, &QListWidget::currentRowChanged, this, &DocTypeListView::slot_curRowChanged);

	setContextMenuPolicy(Qt::CustomContextMenu);

	m_menu = new QMenu(this);
	connect(this, &QWidget::customContextMenuRequested, this, &DocTypeListView::slot_customContextMenuRequested);

	m_menu->addAction("add Custom extension", this, &DocTypeListView::slot_addCustomType);

	connect(ui.customListWidget, &QListWidget::itemClicked, this, &DocTypeListView::slot_customListItemClicked);
	connect(ui.typListWidget, &QListWidget::itemClicked, this, &DocTypeListView::slot_typListItemClicked);
}

DocTypeListView::~DocTypeListView()
{
	save();
}

void DocTypeListView::save()
{
	if (m_isDirty)
	{
		QString key("typelist");
		QString typeStr = s_extBindFileType.join("|");
		JsonDeploy::updataKeyValueFromLongSets(key, typeStr);
		m_isDirty = false;
	}
}

void DocTypeListView::slot_curRowChanged(int row)
{
	ui.typListWidget->clear();

	if (row < s_extBindFileType.count())
	{
		QString typeStr = s_extBindFileType.at(row);
		if (typeStr.isEmpty())
		{
			return;
		}

		QStringList typeList = typeStr.split(":");

		if (typeList.size() > 0)
		{
			ui.typListWidget->addItems(typeList);
		}
	}
}

void DocTypeListView::slot_customContextMenuRequested(const QPoint& /*pos*/)
{
	m_menu->exec(QCursor::pos());
}

void DocTypeListView::slot_addCustomType()
{
	bool ok = false;
	QString text = QInputDialog::getText(this, tr("input file ext()"), tr("ext (Split With :)"), QLineEdit::Normal, QString(".h:.cpp"), &ok);

	if (ok && !text.isEmpty())
	{
		text = text.trimmed();
		QStringList extList = text.split(":");

		ui.customListWidget->addItems(extList);
	}
}

void DocTypeListView::slot_add()
{
	int row = ui.customListWidget->currentRow();

	if (row < 0)
	{
		return;
	}

	QListWidgetItem* item = ui.customListWidget->takeItem(row);
	ui.typListWidget->addItem(item);

	int index = ui.extListWidget->currentRow();

	if (index < s_extBindFileType.size())
	{
		if (!s_extBindFileType[index].isEmpty())
		{
			QString curText = QString(":%1").arg(item->text());
			s_extBindFileType[index].append(curText);
		}
		else
		{
			s_extBindFileType[index].append(item->text());
		}
		QString var = item->text();

		if (!var.isEmpty())
		{
			s_supportFileExts->insert(var.mid(1), true);

			m_isDirty = true;
		}
	}
}

void DocTypeListView::slot_sub()
{
	int row = ui.typListWidget->currentRow();

	if (row < 0)
	{
		return;
	}

	QListWidgetItem* item = ui.typListWidget->takeItem(row);
	ui.customListWidget->addItem(item);

	int index = ui.extListWidget->currentRow();

	if (index < s_extBindFileType.size())
	{
		QString curText = QString("%1:").arg(item->text());

		if (s_extBindFileType[index].contains(curText))
		{
			s_extBindFileType[index].remove(curText);
		}
		else if (s_extBindFileType[index].endsWith(item->text()))
		{
			int lens = s_extBindFileType[index].size();

			int delLens = item->text().size();

			if (lens > delLens)
			{
				//删除:xxx
				s_extBindFileType[index].remove(lens - delLens -1, delLens + 1);
			}
			else
			{
				s_extBindFileType[index].clear();
			}
		}

		QString var = item->text();

		if (!var.isEmpty())
		{
			s_supportFileExts->remove(var.mid(1));
			m_isDirty = true;
		}

	}
}

void DocTypeListView::slot_customListItemClicked(QListWidgetItem* /*item*/)
{
	//qDebug() << "custom list item click";

	ui.subToolButton->setEnabled(false);
	ui.addToolButton->setEnabled(true);
}

void DocTypeListView::slot_typListItemClicked(QListWidgetItem* /*item*/)
{
	//qDebug() << "type list item click";

	ui.subToolButton->setEnabled(true);
	ui.addToolButton->setEnabled(false);
}

