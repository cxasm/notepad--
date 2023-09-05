#include "markdownview.h"

MarkdownView::MarkdownView(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

MarkdownView::~MarkdownView()
{

}


void MarkdownView::viewMarkdown(QString& text)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
	QString tips = QString(
			"NOTE: Your Qt version is lower than 5.14, so you can't preview Markdown for the time being."
			"\n"
			"\n"
			"%1").arg(text);
	ui.textEdit->setPlainText(tips);
#else
	ui.textEdit->setMarkdown(text);
#endif
	
}