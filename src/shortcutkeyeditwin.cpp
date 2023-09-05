#include "shortcutkeyeditwin.h"

#include <QMessageBox> 

ShortcutKeyEditWin::ShortcutKeyEditWin(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.keySequenceEdit->setFocus();
}

ShortcutKeyEditWin::~ShortcutKeyEditWin()
{}

void ShortcutKeyEditWin::setCurKeyDesc(QString desc)
{
	ui.curKeylineEdit->setText(desc);
}

void ShortcutKeyEditWin::setTitle(QString title)
{
	this->setWindowTitle(title);
}

QKeySequence ShortcutKeyEditWin::getNewKeySeq()
{
	return m_newKeys;
}

//确认修改，先检查一下。最多四个键
void  ShortcutKeyEditWin::slot_ok()
{
	QKeySequence keys = ui.keySequenceEdit->keySequence();
	if (keys.isEmpty())
	{
		QMessageBox::warning(this, tr("Error"), tr("input right key !"));
		return;
	}

	m_newKeys = keys;
	done(1);
}

//删除为空
void  ShortcutKeyEditWin::on_delete()
{
	done(2);
}
