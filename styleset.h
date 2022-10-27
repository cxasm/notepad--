#pragma once
#include <QColor>

class StyleSet 
{
public:
	StyleSet();
	~StyleSet();

	static void setCommonStyle(QColor foldfgColor_, QColor foldbgColor_, QColor marginsBackgroundColor_, QString colorName);

	static void setSkin(int id);

	static void setDefaultStyle();
	static void setLightStyle();

	static void setThinBlueStyle();
	static void setThinYellowStyle();
	static void setRiceYellowStyle();

	static void setSilverStyle();

	static void setLavenderBlushStyle();
	static void setMistyRoseStyle();

	static QColor foldfgColor;
	static QColor foldbgColor;
	static QColor marginsBackgroundColor;
};
