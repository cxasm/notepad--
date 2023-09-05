#include "aboutndd.h"

AboutNdd::AboutNdd(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.label->setOpenExternalLinks(true);
	// 隐藏最大化、最小化按钮
	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
	connect(ui.aboutPushButton, &QPushButton::clicked, this, &AboutNdd::onButtonOkayClicked);
}

AboutNdd::~AboutNdd()
{}

void AboutNdd::appendText(QString text)
{
	ui.nddMsgText->appendPlainText(text);
}

void AboutNdd::onButtonOkayClicked()
{
	close();
}
