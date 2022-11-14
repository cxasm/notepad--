#ifndef DONATE_H
#define DONATE_H

#include <QWidget>
#include "ui_donate.h"

class donate : public QWidget
{
	Q_OBJECT

public:
	donate(QWidget *parent = 0);
	~donate();

private:
	Ui::donate ui;
};

#endif // DONATE_H
