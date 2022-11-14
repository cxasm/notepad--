#include "dirfindfile.h"

DirFindFile::DirFindFile(int dire, QWidget *parent): QWidget(parent)
{
	ui.setupUi(this);

	if (dire == 0)
	{
		ui.findLeftRadioButton->setChecked(true);
	}
	else if (dire == 1)
	{
		ui.findRightRadioButton->setChecked(true);
	}
}

DirFindFile::~DirFindFile()
{

}

void DirFindFile::slot_findPrev()
{
	QString name = ui.fileNameLineEdit->text();
	name = name.trimmed();

	int dire = (ui.findLeftRadioButton->isChecked() ? 0:1);

	bool sens = ui.caseSensitiveCheckBox->isChecked();

	emit signFindFile(dire, 0, name, sens);
}

void DirFindFile::slot_findNext()
{
	QString name = ui.fileNameLineEdit->text();
	name = name.trimmed();

	int dire = (ui.findLeftRadioButton->isChecked() ? 0 : 1);

	bool sens = ui.caseSensitiveCheckBox->isChecked();

	emit signFindFile(dire, 1, name, sens);
}
