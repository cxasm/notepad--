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
	ui.textEdit->setMarkdown(text);
}