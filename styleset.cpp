#include "styleset.h"
#include <QFile>
#include <QPalette>
#include <QApplication>
#include <QDebug>


QColor StyleSet::foldfgColor(0xe9, 0xe9, 0xe9, 100);
QColor StyleSet::foldbgColor(0xff, 0xff, 0xff);
QColor StyleSet::marginsBackgroundColor(0xf0f0f0);

StyleSet::StyleSet()
{}

StyleSet::~StyleSet()
{}

void StyleSet::setCommonStyle(QColor foldfgColor_, QColor foldbgColor_, QColor marginsBackgroundColor_, QString colorName)
{
	foldfgColor = foldfgColor_;
	foldbgColor = foldbgColor_;
	marginsBackgroundColor = marginsBackgroundColor_;

	QFile file(":/qss/lightbluestyle.qss"); //qss文件路径:/lightblue.qss
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		styleSheet = file.readAll();
		QPalette palette;
		palette.setColor(QPalette::Window, foldbgColor_);
		palette.setColor(QPalette::Base, foldbgColor_);
		palette.setColor(QPalette::Button, foldbgColor_);
		qApp->setPalette(palette);
		if (colorName != "#EAF7FF")
		{
			styleSheet.replace("#EAF7FF", colorName);
		}
		qApp->setStyleSheet(styleSheet);
	}
	file.close();
}

void StyleSet::setSkin(int id)
{
	switch (id)
	{
	case 0:
		setDefaultStyle();
		break;
	case 1:
		setLightStyle();
		break;
	case 2:
		setThinBlueStyle();
		break;
	case 3:
		setThinYellowStyle();
		break;
	case 4:
		setRiceYellowStyle();
		break;
	case 5:
		setSilverStyle();
		break;
	case 6:
		setLavenderBlushStyle();
		break;
	case 7:
		setMistyRoseStyle();
		break;
	default:
		setDefaultStyle();
		break;
	}
}

void StyleSet::setDefaultStyle()
{
	foldfgColor = QColor(0xe9, 0xe9, 0xe9, 100);
	foldbgColor = QColor(0xff, 0xff, 0xff);
	marginsBackgroundColor = QColor(0xf0f0f0);

	QFile file(":/qss/mystyle.qss"); //qss文件路径:/lightblue.qss
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		styleSheet = file.readAll();
		QPalette palette;
		palette.setColor(QPalette::Window, QColor(0xf0, 0xf0, 0xf0));
		palette.setColor(QPalette::Base, QColor(0xff, 0xff, 0xff));
		palette.setColor(QPalette::Button, QColor(0xf0, 0xf0, 0xf0));
		qApp->setPalette(palette);
		qApp->setStyleSheet(styleSheet);
	}
	file.close();
}

void StyleSet::setLightStyle()
{
#if 0
	foldfgColor = QColor(0xea, 0xf7, 0xff, 100);
	foldbgColor = QColor(0xeaf7ff);
	marginsBackgroundColor = QColor(0xeaf7ff);

	QFile file(":/qss/lightbluestyle.qss"); //qss文件路径:/lightblue.qss
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		styleSheet = file.readAll();
		QPalette palette;
		palette.setColor(QPalette::Window, QColor(0xea, 0xf7, 0xff));
		palette.setColor(QPalette::Base, QColor(0xea, 0xf7, 0xff));
		palette.setColor(QPalette::Button, QColor(0xea, 0xf7, 0xff));
		qApp->setPalette(palette);
		qApp->setStyleSheet(styleSheet);
	}
	file.close();
#endif
	setCommonStyle(QColor(0xea, 0xf7, 0xff, 100), QColor(0xeaf7ff), QColor(0xeaf7ff), "#EAF7FF");
}

void StyleSet::setThinBlueStyle()
{
#if 0
	foldfgColor = QColor(0xd7, 0xe3, 0xf4, 100);
	foldbgColor = QColor(0xd7e3f4);
	marginsBackgroundColor = QColor(0xd7e3f4);

	QFile file(":/qss/lightbluestyle.qss"); //qss文件路径:/lightblue.qss
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		styleSheet = file.readAll();
		QPalette palette;
		palette.setColor(QPalette::Window, QColor(0xd7, 0xe3, 0xf4));
		palette.setColor(QPalette::Base, QColor(0xd7, 0xe3, 0xf4));
		palette.setColor(QPalette::Button, QColor(0xd7, 0xe3, 0xf4));
		qApp->setPalette(palette);

		styleSheet.replace("#EAF7FF","#D7E3F4");
		qApp->setStyleSheet(styleSheet);
	}
	file.close();
#endif
	setCommonStyle(QColor(0xd7, 0xe3, 0xf4, 100), QColor(0xd7e3f4), QColor(0xd7e3f4), "#D7E3F4");
}

//纸黄
void StyleSet::setThinYellowStyle()
{
#if 0
	foldfgColor = QColor(0xf9, 0xf0, 0xe1, 100);
	foldbgColor = QColor(0xf9f0e1);
	marginsBackgroundColor = QColor(0xf9f0e1);

	QFile file(":/qss/lightbluestyle.qss"); //qss文件路径:/lightblue.qss
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		styleSheet = file.readAll();
		QPalette palette;
		palette.setColor(QPalette::Window, QColor(0xf9, 0xf0, 0xe1));
		palette.setColor(QPalette::Base, QColor(0xf9, 0xf0, 0xe1));
		palette.setColor(QPalette::Button, QColor(0xf9, 0xf0, 0xe1));
		qApp->setPalette(palette);

		styleSheet.replace("#EAF7FF", "#F9F0E1");

		qApp->setStyleSheet(styleSheet);
	}
	file.close();
#endif
	setCommonStyle(QColor(0xf9, 0xf0, 0xe1, 100), QColor(0xf9f0e1), QColor(0xf9f0e1), "#F9F0E1");
}

//宣纸黄
void StyleSet::setRiceYellowStyle()
{
#if 0
	foldfgColor = QColor(0xf6, 0xf3, 0xea, 100);
	foldbgColor = QColor(0xf6f3ea);
	marginsBackgroundColor = QColor(0xf6f3ea);

	QFile file(":/qss/lightbluestyle.qss"); //qss文件路径:/lightblue.qss
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		styleSheet = file.readAll();
		QPalette palette;
		palette.setColor(QPalette::Window, QColor(0xf6, 0xf3, 0xea));
		palette.setColor(QPalette::Base, QColor(0xf6, 0xf3, 0xea));
		palette.setColor(QPalette::Button, QColor(0xf6, 0xf3, 0xea));
		qApp->setPalette(palette);

		styleSheet.replace("#EAF7FF", "#F6F3EA");

		qApp->setStyleSheet(styleSheet);
	}
	file.close();
#endif
	setCommonStyle(QColor(0xf6, 0xf3, 0xea, 100), QColor(0xf6f3ea), QColor(0xf6f3ea), "#F6F3EA");
}

//银色
void StyleSet::setSilverStyle()
{
#if 0
	foldfgColor = QColor(0xe9, 0xe8, 0xe4, 100);
	foldbgColor = QColor(0xe9e8e4);
	marginsBackgroundColor = QColor(0xe9e8e4);

	QFile file(":/qss/lightbluestyle.qss"); //qss文件路径:/lightblue.qss
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		styleSheet = file.readAll();
		QPalette palette;
		palette.setColor(QPalette::Window, QColor(0xe9, 0xe8, 0xe4));
		palette.setColor(QPalette::Base, QColor(0xe9, 0xe8, 0xe4));
		palette.setColor(QPalette::Button, QColor(0xe9, 0xe8, 0xe4));
		qApp->setPalette(palette);

		styleSheet.replace("#EAF7FF", "#E9E8E4");

		qApp->setStyleSheet(styleSheet);
	}
	file.close();
#endif
	setCommonStyle(QColor(0xe9, 0xe8, 0xe4, 100), QColor(0xe9e8e4), QColor(0xe9e8e4), "#E9E8E4");
}

//谈紫色#FFF0F5
void StyleSet::setLavenderBlushStyle()
{
#if 0
	foldfgColor = QColor(0xff, 0xf0, 0xf5, 100);
	foldbgColor = QColor(0xFFF0F5);
	marginsBackgroundColor = QColor(0xFFF0F5);

	QFile file(":/qss/lightbluestyle.qss"); //qss文件路径:/lightblue.qss
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		styleSheet = file.readAll();
		QPalette palette;
		palette.setColor(QPalette::Window, QColor(0xff, 0xf0, 0xf5));
		palette.setColor(QPalette::Base, QColor(0xff, 0xf0, 0xf5));
		palette.setColor(QPalette::Button, QColor(0xff, 0xf0, 0xf5));
		qApp->setPalette(palette);

		styleSheet.replace("#EAF7FF", "#FFF0F5");

		qApp->setStyleSheet(styleSheet);
	}
	file.close();
#endif
	setCommonStyle(QColor(0xff, 0xf0, 0xf5, 100), QColor(0xFFF0F5), QColor(0xFFF0F5), "#FFF0F5");
}

//MistyRose
void StyleSet::setMistyRoseStyle()
{
#if 0
	foldfgColor = QColor(0xff, 0xe4, 0xe1, 100);
	foldbgColor = QColor(0xFFE4E1);
	marginsBackgroundColor = QColor(0xFFE4E1);

	QFile file(":/qss/lightbluestyle.qss"); //qss文件路径:/lightblue.qss
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		styleSheet = file.readAll();
		QPalette palette;
		palette.setColor(QPalette::Window, QColor(0xff, 0xe4, 0xe1));
		palette.setColor(QPalette::Base, QColor(0xff, 0xe4, 0xe1));
		palette.setColor(QPalette::Button, QColor(0xff, 0xe4, 0xe1));
		qApp->setPalette(palette);

		styleSheet.replace("#EAF7FF", "#FFE4E1");

		qApp->setStyleSheet(styleSheet);
	}
	file.close();
#endif
	setCommonStyle(QColor(0xff, 0xe4, 0xe1, 100), QColor(0xFFE4E1), QColor(0xFFE4E1), "#FFE4E1");
}
