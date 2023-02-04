#include "bigfilemessage.h"

BigFileMessage::BigFileMessage(QWidget *parent)
	: QDialog(parent), m_result(-1)
{
	ui.setupUi(this);
}

BigFileMessage::~BigFileMessage()
{}

void BigFileMessage::setDefOpenMode(NddDocType defMode)
{
	switch (defMode)
	{
	case TXT_TYPE:
		ui.textMode->setChecked(true);
		break;
	case BIG_TEXT_RO_TYPE:
		ui.bigTextMode->setChecked(true);
		break;
	case BIG_EDIT_RW_TYPE:
		break;
	case SUPER_BIG_TEXT_RO_TYPE:
		ui.superBigTextMode->setChecked(true);
		break;
	case HEX_TYPE:
		ui.hexMode->setChecked(true);
		break;
	default:
		break;
	}
}

void BigFileMessage::setTip(QString msg)
{
	ui.label->setText(msg);
}

void BigFileMessage::slot_okBt()
{
	if (ui.textMode->isChecked())
	{
		m_result = TXT_TYPE;//普通文本
	}
	else if(ui.bigTextMode->isChecked())
	{
		m_result = BIG_TEXT_RO_TYPE; //小于8G的大文本
	}
	else if (ui.superBigTextMode->isChecked())
	{
		m_result = SUPER_BIG_TEXT_RO_TYPE;//8G以上的大文本
	}
	else if (ui.hexMode->isChecked())
	{
		m_result = HEX_TYPE;//二进制
	}
	done(m_result);
}

void BigFileMessage::slot_cancelBt()
{
	m_result = -1;
	done(m_result);
}
