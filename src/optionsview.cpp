#include "optionsview.h"
#include "doctypelistview.h"
#include "texteditsetwin.h"
#include "ccnotepad.h"

OptionsView::OptionsView(QWidget* pNotepadWin, QWidget *parent)
	: QWidget(parent), m_pNotepadWin(pNotepadWin)
{
	ui.setupUi(this);

	//只在文件对比中出现；在编辑框模式下不出现，这个关联文件容易误解。
	if (pNotepadWin == nullptr)
	{
	DocTypeListView* p = new DocTypeListView(this);
	ui.stackedWidget->addWidget(p);

	//文件关联 file correlation
	ui.optionListWidget->addItem(tr("File Correlation"));
	}


	ui.optionListWidget->addItem(tr("Compare File Types"));

	TextEditSetWin* p2 = new TextEditSetWin(this);
	p2->setNotePadWin(pNotepadWin);

	ui.stackedWidget->addWidget(p2);
	ui.optionListWidget->addItem(tr("Text And Fonts"));

	connect(ui.optionListWidget, &QListWidget::currentRowChanged, this, &OptionsView::slot_curRowChanged);
	connect(p2, &TextEditSetWin::sendTabFormatChange, this, &OptionsView::sendTabFormatChange);
	connect(p2, &TextEditSetWin::signProLangFontChange, this, &OptionsView::signProLangFontChange);
}

OptionsView::~OptionsView()
{
}

void OptionsView::slot_curRowChanged(int row)
{
	if (row < ui.stackedWidget->count())
	{
		ui.stackedWidget->setCurrentIndex(row);
	}
}

void OptionsView::slot_ok()
{
	close();
}
