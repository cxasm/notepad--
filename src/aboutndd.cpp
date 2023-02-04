#include "aboutndd.h"

AboutNdd::AboutNdd(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.label->setOpenExternalLinks(true);
}

AboutNdd::~AboutNdd()
{}

void AboutNdd::appendText(QString text)
{
	ui.nddMsgText->appendPlainText(text);
}
