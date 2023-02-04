#ifndef DONATE_H
#define DONATE_H

#include <QWidget>
#include "ui_donate.h"

class Donate : public QWidget
{
	Q_OBJECT

public:
	Donate(QWidget *parent = 0);
	~Donate();

private:
	Ui::DonateClass ui;
};

#endif // DONATE_H
