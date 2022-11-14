#include "qtlangset.h"
#include "scintillaeditview.h"
#include "jsondeploy.h"
#include "rcglobal.h"
#include <SciLexer.h>
#include <qscilexer.h>
#include <QSettings>
#include <QColorDialog> 
#include <QMessageBox>
#include <QSpinBox>
#include <QDebug>
#include <QDir>

#if 0
//要注意与common.h的保持一致。
enum LangType {
	L_TEXT = -1, L_PHP, L_C, L_CPP, L_CS, L_OBJC, L_JAVA, L_RC, \
	L_HTML, L_XML, L_MAKEFILE, L_PASCAL, L_BATCH, L_INI, L_ASCII, L_USER, \
	L_ASP, L_SQL, L_VB, L_JS, L_CSS, L_PERL, L_PYTHON, L_LUA, \
	L_TEX, L_FORTRAN, L_BASH, L_FLASH, L_NSIS, L_TCL, L_LISP, L_SCHEME, \
	L_ASM, L_DIFF, L_PROPS, L_PS, L_RUBY, L_SMALLTALK, L_VHDL, L_KIX, L_AU3, \
	L_CAML, L_ADA, L_VERILOG, L_MATLAB, L_HASKELL, L_INNO, L_SEARCHRESULT, \
	L_CMAKE, L_YAML, L_COBOL, L_GUI4CLI, L_D, L_POWERSHELL, L_R, L_JSP, \
	L_COFFEESCRIPT, L_JSON, L_JAVASCRIPT, L_FORTRAN_77, L_BAANC, L_SREC, \
	L_IHEX, L_TEHEX, L_SWIFT, \
	L_ASN1, L_AVS, L_BLITZBASIC, L_PUREBASIC, L_FREEBASIC, \
	L_CSOUND, L_ERLANG, L_ESCRIPT, L_FORTH, L_LATEX, \
	L_MMIXAL, L_NIM, L_NNCRONTAB, L_OSCRIPT, L_REBOL, \
	L_REGISTRY, L_RUST, L_SPICE, L_TXT2TAGS, L_VISUALPROLOG, L_TYPESCRIPT, \
	L_EDIFACT, L_MARKDOWN, L_OCTAVE, L_PO, L_POV, L_IDL, L_GO, L_TXT, \
	// Don't use L_JS, use L_JAVASCRIPT instead
	// The end of enumated language type, so it should be always at the end
	L_EXTERNAL = 100, L_USER_DEFINE = 200
};
#endif


QtLangSet::QtLangSet(QString initTag, QWidget *parent)
	: QMainWindow(parent), m_selectLexer(nullptr), m_selectStyleId(0), m_isStyleChange(false),m_isStyleChildChange(false), m_initShowLexerTag(initTag), m_previousSysLangItem(nullptr)
{
	ui.setupUi(this);
	initLangList();
	initUserDefineLangList();

	startSignSlot();
}

QtLangSet::~QtLangSet()
{
	if (nullptr != m_selectLexer)
	{
		delete m_selectLexer;
		m_selectLexer = nullptr;
	}
}



void QtLangSet::startSignSlot()
{
	//开始本来使用的是QListWidget::currentItemChanged信号，但是发现该信号存在莫名其妙的触发机制。
	//估计是QT5.12的bug。所以换成itemClicked信号
	connect(ui.langListWidget, &QListWidget::itemClicked, this, &QtLangSet::slot_itemSelect);
	connect(ui.userLangListWidget, &QListWidget::itemClicked, this, &QtLangSet::slot_userLangItemSelect);


	connect(ui.styleListWidget, &QListWidget::currentItemChanged, this, &QtLangSet::slot_styleItemSelect);

	connect(ui.boldCheckBox, &QCheckBox::stateChanged, this, &QtLangSet::slot_fontBoldChange);
	connect(ui.italicCheckBox, &QCheckBox::stateChanged, this, &QtLangSet::slot_fontItalicChange);
	connect(ui.underlineCheckBox, &QCheckBox::stateChanged, this, &QtLangSet::slot_fontUnderlineChange);
	connect(ui.fontSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slot_fontSizeChange(int)));
	connect(ui.fontComboBox, &QFontComboBox::currentFontChanged, this, &QtLangSet::slot_fontChange);
}

//让初始化时选择当前编辑器的语言
void QtLangSet::selectInitLangTag(QString initLangTag)
{

	//第一次必须以初始化lexid显示。这样做实在是因为初始化list启动后，会自动把第一个设置为当前。
	//这里还是有问题，是qt5.12.4的bug机制导致:如果没有选择QListWidget，会总是自动选择第一个
	if (!initLangTag.isEmpty())
	{
		QList<QListWidgetItem*> destItems = ui.langListWidget->findItems(initLangTag,Qt::MatchExactly);
		if (!destItems.isEmpty())
		{
			QListWidgetItem* newItem = destItems.first();

			ui.langListWidget->scrollToItem(newItem);
			ui.langListWidget->setCurrentItem(newItem);
			ui.userLangListWidget->setCurrentItem(nullptr);

			slot_itemSelect(newItem);
		}
		else
		{
			destItems = ui.userLangListWidget->findItems(initLangTag, Qt::MatchExactly);
			if (!destItems.isEmpty())
			{
				QListWidgetItem* userItem = destItems.first();
				ui.userLangListWidget->scrollToItem(userItem);
				ui.userLangListWidget->setCurrentItem(userItem);
				ui.langListWidget->setCurrentItem(nullptr);

				slot_userLangItemSelect(userItem);

			}
		}
	}
}

void QtLangSet::slot_fontBoldChange(int state)
{
	//即时设置风格
	if (m_selectLexer != nullptr)
	{
		if (m_curStyleData.font.bold() != ((state == Qt::Unchecked) ? false : true))
		{
			m_curStyleData.font.setBold((state == Qt::Unchecked) ? false : true);
			m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
			m_isStyleChange = true;
			emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, m_curStyleData.color, m_curStyleData.paper, m_curStyleData.font, true);
		}
	}
}

void QtLangSet::slot_fontItalicChange(int state)
{
	//即时设置风格
	if (m_selectLexer != nullptr)
	{
		if (m_curStyleData.font.italic() != ((state == Qt::Unchecked) ? false : true))
		{
			m_curStyleData.font.setItalic((state == Qt::Unchecked) ? false : true);
			m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
			m_isStyleChange = true;
			emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, m_curStyleData.color, m_curStyleData.paper, m_curStyleData.font, true);
		}
	}
}

void QtLangSet::slot_fontUnderlineChange(int state)
{
	//即时设置风格
	if (m_selectLexer != nullptr)
	{
		if (m_curStyleData.font.underline() != ((state == Qt::Unchecked) ? false : true))
		{
			m_curStyleData.font.setUnderline((state == Qt::Unchecked) ? false : true);
			m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
			m_isStyleChange = true;
			emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, m_curStyleData.color, m_curStyleData.paper, m_curStyleData.font, true);
		}
	}
}

void QtLangSet::slot_fontSizeChange(int v)
{
	//即时设置风格
	if (m_selectLexer != nullptr)
	{
		if (!ui.modifyAllFont->isChecked())
		{
		if (m_curStyleData.font.pointSize() != v)
		{
			m_curStyleData.font.setPointSize(v);
			m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
			m_isStyleChange = true;
				emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, m_curStyleData.color, m_curStyleData.paper, m_curStyleData.font, true);
		}
	}
		else
		{
			m_curStyleData.font.setPointSize(v);
			m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
			m_isStyleChange = true;
			//即时设置风格
			if (m_selectLexer != nullptr)
			{
				int row = ui.styleListWidget->count();

				QListWidgetItem* item = nullptr;

				for (int i = 0; i < row; ++i)
				{
					item = ui.styleListWidget->item(i);
					int styleId = item->data(Qt::UserRole).toInt();

					//所有字体大小修改为一致
					QFont f = m_selectLexer->font(styleId);
					f.setPointSize(v);
					m_selectLexer->setFont(f, styleId);
}

				saveCurLangSettings();
				emit viewLexerChange(m_selectLexer->lexerTag());
			}
		}
	}
}

void QtLangSet::slot_fontChange(const QFont &font)
{
	//即时设置风格
	if (m_selectLexer != nullptr)
	{
		if (!ui.modifyAllFont->isChecked())
		{
		if (m_curStyleData.font != font)
		{
			QFont oldf = m_curStyleData.font;
			m_curStyleData.font = font;
			m_curStyleData.font.setBold(oldf.bold());
			m_curStyleData.font.setItalic(oldf.italic());
			m_curStyleData.font.setUnderline(oldf.underline());

			m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
			m_isStyleChange = true;
				emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, m_curStyleData.color, m_curStyleData.paper, m_curStyleData.font, true);
		}
	}
		else
		{
			//统一修改字体，大小和风格保存不变
			QFont oldf = m_curStyleData.font;
			m_curStyleData.font = font;
			m_curStyleData.font.setBold(oldf.bold());
			m_curStyleData.font.setItalic(oldf.italic());
			m_curStyleData.font.setUnderline(oldf.underline());

			m_isStyleChange = true;
			//即时设置风格
			if (m_selectLexer != nullptr)
			{
				int row = ui.styleListWidget->count();

				QListWidgetItem* item = nullptr;

				for (int i = 0; i < row; ++i)
				{
					item = ui.styleListWidget->item(i);
					int styleId = item->data(Qt::UserRole).toInt();
					QFont f = m_selectLexer->font(styleId);

					//新字体的名统一设置，但是风格字体大小保存老的一致
					QFont newFont = m_curStyleData.font;
					newFont.setBold(f.bold());
					newFont.setItalic(f.italic());
					newFont.setUnderline(f.underline());
					newFont.setPointSize(f.pointSize());

					m_selectLexer->setFont(newFont, styleId);
}

				saveCurLangSettings();
				emit viewLexerChange(m_selectLexer->lexerTag());
			}
		}
	}
}

void QtLangSet::initLangList()
{

	for (int i = 0; i <= L_TXT; ++i)
	{
		QsciLexer *pLexer = ScintillaEditView::createLexer(i);
		if (nullptr != pLexer)
		{
			QString langName = pLexer->lexerTag();
			//qDebug() << langName << pLexer->lexer();
			QListWidgetItem *item = new QListWidgetItem(langName);
			item->setData(Qt::UserRole,i);
			ui.langListWidget->addItem(item);
			delete pLexer;
		}
		
	}

	//怀疑这个是队列链接的，会延迟执行。故总是导致选择的选不中，而是第一个语言asp。
	ui.langListWidget->sortItems(Qt::AscendingOrder);

}

//isLoadToUI是否加载显示到当前UI界面
bool QtLangSet::readLangSetFile(QString langName,QString &keyword, QString &motherLang, QString & extTypes)
{
	QString userLangFile = QString("notepad/userlang/%1").arg(langName);//自定义语言中不能有.字符，否则可能有错，后续要检查
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, userLangFile);
	qs.setIniCodec("UTF-8");
	qDebug() << qs.fileName();

	if (!qs.contains("mz"))
	{
		return false;
	}

	//自定义语言格式。
	//mz:ndd
	//name:xxx
	//mother:xxx none/cpp/html 就三种
	//ext:xx xx xx 文件关联后缀名
	//keword:xxx
	keyword = qs.value("keyword").toString();
	extTypes = qs.value("ext").toString();
	motherLang = qs.value("mother").toString();

	return true;
}

//初始化用户自定义的语言配置
void QtLangSet::initUserDefineLangList()
		{
	QString userLangDirPath = getUserLangDirPath();

	//遍历文件夹
	QDir dir_file(userLangDirPath);
	QFileInfoList list_file = dir_file.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name);

	for (int i = 0; i < list_file.size(); ++i)
	{  //将当前目录中所有文件添加到treewidget中
		QFileInfo fileInfo = list_file.at(i);

		//这个文件是ext和tag的映射文件，不做配置解析
		if (fileInfo.baseName() == "ext_tag")
		{
			continue;
		}

		ui.userLangListWidget->addItem(fileInfo.baseName());
	}
	ui.userLangListWidget->sortItems(Qt::AscendingOrder);
}


//切换当前语言。
//QListWidget::currentItemChanged这个信号qt5.12.10中应该是存在bug,总是莫名其妙的要发一次currentItemChanged信号出来。导致第一次该选的项被覆盖。
//换成itemClicked信号
void QtLangSet::slot_itemSelect(QListWidgetItem *item)
{
	if (item == nullptr)
	{
		return;
		}
	//qDebug() << item->text() << (previous ? previous->text() : 0);


	ui.langListWidget->scrollToItem(item);

	ui.userLangListWidget->setCurrentItem(nullptr);

	//保存前一个已经修改的。询问用户是否保存
	if (m_isStyleChange && m_previousSysLangItem != nullptr && m_selectLexer !=nullptr)
	{
		if (QMessageBox::Yes == QMessageBox::question(this, tr("Save Change"), tr("%1 style configuration has been modified. Do you want to save it?").arg(m_previousSysLangItem->text())))
		{
			saveCurLangSettings();
		}
	}

	if (!item->isSelected())
	{
		item->setSelected(true);
	}

	int lexId = item->data(Qt::UserRole).toInt();

	ui.styleListWidget->clear();

	if (m_selectLexer != nullptr)
	{
		delete m_selectLexer;
		m_selectLexer = nullptr;
	}

	QsciLexer *pLexer = ScintillaEditView::createLexer(lexId);
	if (nullptr != pLexer)
	{
		m_selectLexer = pLexer;

		QString keyword(m_selectLexer->keywords(1));

		ui.keywordTe->setPlainText(keyword);
		ui.motherLangCb->setCurrentIndex(0);

		for (int i = 0; i <= 255; ++i)
		{
			QString desc = pLexer->description(i);
			if (!desc.isEmpty())
			{
				QListWidgetItem *itemtemp = new QListWidgetItem(desc);
				itemtemp->setData(Qt::UserRole, i);
				ui.styleListWidget->addItem(itemtemp);
			}
		}
	}

	m_previousSysLangItem = item;

	//自动显示第一条的结果
	QListWidgetItem *itemtemp = ui.styleListWidget->item(0);
	if (itemtemp != nullptr)
	{
		itemtemp->setSelected(true);
		slot_styleItemSelect(itemtemp, nullptr);
}

	m_isStyleChange = false;

	ui.extFileType->clear();
	ui.motherLangCb->setCurrentIndex(0);
}

//根据母亲模块语言，把属性风格显示出来
void QtLangSet::displayUserMotherLangsStyle(QString langTagName, UserLangMother motherLang)
{
	QsciLexer *pLexer = nullptr;
	
	ui.styleListWidget->clear();

	switch (motherLang)
	{
	case MOTHER_NONE:
		pLexer = ScintillaEditView::createLexer(L_USER_TXT, langTagName);
		break;
	case MOTHER_CPP:
		pLexer = ScintillaEditView::createLexer(L_USER_CPP, langTagName);
		break;
	default:
		pLexer = ScintillaEditView::createLexer(L_USER_TXT, langTagName);
		break;
	}

	if (nullptr != pLexer)
	{
		//这里一定要以新语言的名称作为lexer的tag。
		//pLexer->setLexerTag(langTagName);
		pLexer->setIsUserDefineKeywords(true);

		if (m_selectLexer != nullptr)
		{
			delete m_selectLexer;
		}

		m_selectLexer = pLexer;

		for (int i = 0; i <= 255; ++i)
		{
			QString desc = pLexer->description(i);
			if (!desc.isEmpty())
			{
				QListWidgetItem *itemtemp = new QListWidgetItem(desc);
				itemtemp->setData(Qt::UserRole, i);
				ui.styleListWidget->addItem(itemtemp);
			}
		}
	}

	//自动显示第一条的结果
	QListWidgetItem *itemtemp = ui.styleListWidget->item(0);
	if (itemtemp != nullptr)
	{
		itemtemp->setSelected(true);
		slot_styleItemSelect(itemtemp, nullptr);
}

	m_isStyleChange = false;

}


//点击当前用户自定义语言的语言item。todo:暂时没有做前一个的修改保存，需要用户每次修改后自己点击save，否则可能不会保存
void QtLangSet::slot_userLangItemSelect(QListWidgetItem *item)
{
	if (item == nullptr)
	{
		return;
	}

	m_previousSysLangItem = nullptr;
	ui.langListWidget->setCurrentItem(nullptr);

	QString langName = item->text();

	QString keyword;
	QString motherLang;
	QString extTypes;

	if (readLangSetFile(langName, keyword, motherLang, extTypes))
	{
		ui.keywordTe->setPlainText(keyword);
		ui.motherLangCb->setCurrentText(motherLang);
		ui.extFileType->setText(extTypes);

		//根据母亲模块语言，把属性风格显示出来
		displayUserMotherLangsStyle(langName, (UserLangMother)(LangType::L_USER_TXT + ui.motherLangCb->currentIndex()));
	}
	else
	{
		ui.statusBar->showMessage(tr("Read %1 language user define  format error.").arg(langName), 10000);
	}
}


//获取当前ui上的字体
void QtLangSet::getCurUiFont(QFont& font)
{
	font = ui.fontComboBox->currentFont();
	font.setBold(ui.boldCheckBox->isChecked());
	font.setItalic(ui.italicCheckBox->isChecked());
	font.setUnderline(ui.underlineCheckBox->isChecked());
	font.setPointSize(ui.fontSpinBox->value());
}

void QtLangSet::closeEvent(QCloseEvent * e)
{
	saveCurLangSettings();
}

//把item的对应风格显示在界面上
void QtLangSet::syncShowStyleItemToUI(QListWidgetItem *item)
{
	if (item != nullptr && m_selectLexer != nullptr)
	{
		int styleId = item->data(Qt::UserRole).toInt();
		m_selectStyleId = styleId;
		QsciLexer::StyleData & sd = m_selectLexer->styleData(styleId);
		m_curStyleData = sd;
		setStyleShow(sd.font, sd.color, sd.paper);
		m_isStyleChange = false;
	}
}
//点击当前的风格item
void QtLangSet::slot_styleItemSelect(QListWidgetItem *current, QListWidgetItem *previous)
	{
#if 0
	if (current != nullptr && m_selectLexer != nullptr)
	{
		int styleId = current->data(Qt::UserRole).toInt();
		m_selectStyleId = styleId;
		QsciLexer::StyleData & sd = m_selectLexer->styleData(styleId);
		m_curStyleData = sd;
		setStyleShow(sd.font, sd.color, sd.paper);
		m_isStyleChange = false;
	}
#endif
	syncShowStyleItemToUI(current);
}

void QtLangSet::fillForegroundColor(QColor& fcColor)
{
	QPixmap f(32, 32);
	f.fill(fcColor);
	ui.label_fc->setPixmap(f);
}

void QtLangSet::fillBackgroundColor(QColor &bkColor)
{

	QPixmap b(32, 32);
	b.fill(bkColor);
	ui.label_bc->setPixmap(b);
}

void QtLangSet::setStyleShow(QFont& font, QColor& fcColor, QColor &bkColor)
{
	ui.fontComboBox->setCurrentFont(font);
	ui.fontSpinBox->setValue(font.pointSize());

	ui.boldCheckBox->setChecked(font.bold());
	ui.italicCheckBox->setChecked(font.italic());
	ui.underlineCheckBox->setChecked(font.underline());

	fillForegroundColor(fcColor);
	fillBackgroundColor(bkColor);
	
}

//读取特定语言的设置
bool QtLangSet::readLangSettings(QsciLexer *lexer, QString tag)
{
	QString cfgPath = QString("notepad/%1").arg(tag);

	QSettings qs(QSettings::IniFormat, QSettings::UserScope, cfgPath);
	if (QFile::exists(qs.fileName()))
	{
	return lexer->readSettings(qs);
}
	return false;
}

#if 0
void QtLangSet::setCurSelectLang(QString tag)
{
	QList<QListWidgetItem*> destItem = ui.langListWidget->findItems(tag, Qt::MatchExactly);
	if (!destItem.isEmpty())
	{
		QListWidgetItem* itemtemp = destItem.first();
		itemtemp->setSelected(true);
		//slot_itemSelect(itemtemp, nullptr);
	}
}
#endif

bool QtLangSet::saveCurLangSettings()
{
	if (m_isStyleChange && m_selectLexer != nullptr)
	{
		QString tag = m_selectLexer->lexerTag();
		QString cfgPath = QString("notepad/%1").arg(tag);

		QSettings qs(QSettings::IniFormat, QSettings::UserScope, cfgPath);
		m_selectLexer->writeSettings(qs);
		m_isStyleChange = false;
		return true;
	}
	return false;
}

//修改前景色
void QtLangSet::slot_changeFgColor()
{
	QColor color = QColorDialog::getColor(Qt::white, this, tr("Style Foreground Color"));
	if (color.isValid())
	{
		if (!ui.modiryAllColor->isChecked())
		{
		//当前前景色是否变化
		if (m_curStyleData.color != color)
		{
			m_curStyleData.color = color;
			fillForegroundColor(color);
			m_isStyleChange = true;

			//即时设置风格
			if (m_selectLexer != nullptr)
			{
					m_selectLexer->setColor(color, m_selectStyleId);

					emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, color, m_curStyleData.paper, m_curStyleData.font, false);
			}
		}
	}
		else
		{
			//全部风格颜色修改
			//当前前景色是否变化
			m_curStyleData.color = color;
			fillForegroundColor(color);
			m_isStyleChange = true;

			//即时设置风格
			if (m_selectLexer != nullptr)
			{
				int row = ui.styleListWidget->count();

				QListWidgetItem* item = nullptr;

				for (int i = 0; i < row; ++i)
				{
					item = ui.styleListWidget->item(i);
					int styleId = item->data(Qt::UserRole).toInt();
					m_selectLexer->setColor(color, styleId);
				}

				saveCurLangSettings();
				emit viewLexerChange(m_selectLexer->lexerTag());
			}
	
}
	}
	
}

//修改背景色
void QtLangSet::slot_changeBkColor()
{
	QColor color = QColorDialog::getColor(Qt::white, this, tr("Style Background Color"));
	if (color.isValid())
	{
		//当前前景色是否变化
		if (m_curStyleData.paper != color)
		{
			m_curStyleData.paper = color;
			fillBackgroundColor(color);
			m_isStyleChange = true;

			//即时设置风格
			if (m_selectLexer != nullptr)
			{
				m_selectLexer->setPaper(color, m_selectStyleId);
				emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, color, m_curStyleData.paper, m_curStyleData.font, false);
			}
		}
	}
}

void QtLangSet::slot_saveClick()
{
	saveCurLangSettings();
}



//恢复初始化设置
void QtLangSet::slot_reset()
{
	if (m_selectLexer != nullptr)
	{
		if (QMessageBox::Yes != QMessageBox::question(this, tr("Reset Style"), tr("Are you sure to reset language %1 sytle").arg(m_selectLexer->lexerTag())))
		{
			return;
		}
		//如果存在自定义的配置，也删除掉
		QString cfgPath = QString("notepad/%1").arg(m_selectLexer->lexerTag());

		QSettings qs(QSettings::IniFormat, QSettings::UserScope, cfgPath);
		if (QFile::exists(qs.fileName()))
		{
			QFile::remove(qs.fileName());
		}

		//一定要先保存，因为selectInitLangTag后可能会变化

		int row = ui.styleListWidget->currentRow();

		selectInitLangTag(m_selectLexer->lexerTag());

		//手动刷新UI
		QListWidgetItem* styleItem = ui.styleListWidget->item(row);

		syncShowStyleItemToUI(styleItem);

		emit viewLexerChange(m_selectLexer->lexerTag());
	}
}
