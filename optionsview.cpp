#include "optionsview.h"
#include "doctypelistview.h"
#include "texteditsetwin.h"
#include "ccnotepad.h"

OptionsView::OptionsView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	DocTypeListView* p = new DocTypeListView(this);
	//p->show();
	ui.stackedWidget->addWidget(p);

	//文件关联 file correlation
	ui.optionListWidget->addItem(tr("File Correlation"));


	ui.optionListWidget->addItem(tr("Compare File Types"));

	TextEditSetWin* p2 = new TextEditSetWin(this);

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
