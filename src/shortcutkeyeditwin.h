#pragma once

#include <QDialog>
#include "ui_shortcutkeyeditwin.h"

class ShortcutKeyEditWin : public QDialog
{
	Q_OBJECT

public:
	ShortcutKeyEditWin(QWidget *parent = nullptr);
	~ShortcutKeyEditWin();

	void setCurKeyDesc(QString desc);
	QKeySequence getNewKeySeq();
	void setTitle(QString title);

private slots:
	void slot_ok();
	void on_delete();

private:
	Ui::ShortcutKeyEditWinClass ui;
	QKeySequence m_newKeys;
};
