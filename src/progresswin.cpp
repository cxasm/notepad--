#include "progresswin.h"
#include <QCoreApplication>
#include <QMessageBox>

ProgressWin::ProgressWin(QWidget *parent)
	: QDialog(parent), m_curStep(0),m_isCancel(false)
{
	ui.setupUi(this);
}

ProgressWin::~ProgressWin()
{
}


void ProgressWin::info(QString text)
{
	ui.output->append(text);
}

void ProgressWin::setTotalSteps(int step)
{
	ui.progressBar->setValue(0);
	ui.progressBar->setMaximum(step);
	m_curStep = 0;
}

void ProgressWin::moveStep(bool isRest)
{
	++m_curStep;
	ui.progressBar->setValue(m_curStep);
	ui.progressBar->update();

	if (isRest)
	{
		QCoreApplication::processEvents();
}
}

int ProgressWin::getTotalStep()
{
	return ui.progressBar->maximum();
}

void ProgressWin::setStep(int step)
{
	ui.progressBar->setValue(step);
	ui.progressBar->update();
	m_curStep = step;
	//QCoreApplication::processEvents();
}

bool ProgressWin::isCancel()
{
	return m_isCancel;
}

void ProgressWin::setCancel()
{
	m_isCancel = true;
	emit quitClick();
}

void ProgressWin::closeEvent(QCloseEvent* e)
{
	e->ignore();
}

void ProgressWin::slot_quitBt()
{
	if (QMessageBox::Yes != QMessageBox::question(this, tr("Notice"), tr("Are you sure to cancel?")))
	{
		return;
	}

	m_isCancel = true;
	emit quitClick();
}
