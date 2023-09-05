#pragma once

#include <QWidget>
#include "ui_qttestclass.h"

class QsciScintilla;
class QtTestClass : public QWidget
{
	Q_OBJECT

public:
	QtTestClass(QWidget *parent, QsciScintilla* pEdit);
	~QtTestClass();

private slots:
	void on_upper();
	void on_lower();
private:
	Ui::QtTestClassClass ui;
	QsciScintilla* m_pEdit;
};
