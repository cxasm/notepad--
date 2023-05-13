#pragma once

#include <QWidget>
#include <QButtonGroup>
#include "ui_md5hash.h"

class Md5hash : public QWidget
{
	Q_OBJECT

public:
	Md5hash(QWidget *parent = nullptr);
	~Md5hash();

private slots:
	void slot_select();
	void on_hash();
	void on_methodIdChange(int id);
	void on_copyClipboard();

private:
	Ui::Md5hashClass ui;
	QButtonGroup m_btGroup;
	QStringList m_fileList;
	bool m_isFile;
};
