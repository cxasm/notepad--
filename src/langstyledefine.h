#pragma once

#include <QMainWindow>
#include "ui_langstyledefine.h"
#include "rcglobal.h"


class LangStyleDefine : public QMainWindow
{
	Q_OBJECT

public:
	LangStyleDefine(QWidget *parent = nullptr);
	~LangStyleDefine();

private :
	void loadUserLangs();
	bool readLangSetFile(QString langName, bool isLoadToUI=false);

private slots:
	void slot_new();
	void slot_save();
	void slot_langsChange(int index);

	void slot_delete();

private:
	Ui::LangStyleDefineClass ui;

	static QString s_userLangDirPath;
};


