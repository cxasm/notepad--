#pragma once

#include <QMainWindow>
#include "ui_langextset.h"

class LangExtSet : public QMainWindow
{
	Q_OBJECT

public:
	LangExtSet(QWidget *parent = nullptr);
	~LangExtSet();
	static void loadExtRelevanceToMagr();

private:
	void initLangName();
	void updataExtLexerManager(QString tag, int lexId, QStringList& oldExtList, QStringList& newExtList);

protected:
	void closeEvent(QCloseEvent* e);

private slots:
	void slot_itemChanged(QTableWidgetItem* item);
	void slot_currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);
	void slot_save();

private:
	Ui::LangExtSetClass ui;
	bool m_isChanged;
};
