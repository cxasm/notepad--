#include "qttestclass.h"
#include <qsciscintilla.h>

QtTestClass::QtTestClass(QWidget *parent, QsciScintilla* pEdit)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_pEdit = pEdit;
}

QtTestClass::~QtTestClass()
{}

void QtTestClass::on_upper()
{
	QString text = m_pEdit->text();

	text = text.toUpper();

	m_pEdit->setText(text);
}

void QtTestClass::on_lower()
{
	QString text = m_pEdit->text();

	text = text.toLower();

	m_pEdit->setText(text);
}