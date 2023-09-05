#pragma once
#include <QColor>
#include <QFont>

enum StyleId {
	DEFAULT_SE=0,
	BESPIN,
	BLACK_BOARD,
	BLUE_LIGHT,
	CHOCO,
	DANSLE_RUSH_DARK,
	DEEP_BLACK,
	LAVENDER,
	HOT_FUDGE_SUNDAE,
	MISTY_ROSE,
	MONO_INDUSTRIAL,
	MONOKAI,
	OBSIDIAN,
	PLASTIC_CODE,
	RUBY_BLUE,
	TWILIGHT,
	VIBRANT_INK,
	YELLOW_RICE,
	MAX_SE,
};

#define BLACK_SE DEEP_BLACK

struct One_Stype_Info {
	int styleId;
	QColor fgColor;
	QColor bgColor;
	QFont font; //这个font代表两个含有，1是字体，而是字体大小。有些属性，比如括号大小，他们没有字体，但是有大小，则使用字体大小来表示
	One_Stype_Info() :styleId(-1)
	{
	}
	One_Stype_Info(int id, QColor fg, QColor bg) :styleId(id), fgColor(fg), bgColor(bg)
	{
	}
};


//当前的全局风格属性，所有的样式修改，都是先修改这个结构体，然后其余地方再根据这个修改后的结构体，来进行同步更新
struct GLOBAL_STYLE_OPS {
	One_Stype_Info global_style;
	One_Stype_Info default_style;
	One_Stype_Info indent_guideline;
	One_Stype_Info brace_highight;
	One_Stype_Info bad_brace_color;
	One_Stype_Info current_line_background_color;
	One_Stype_Info select_text_color;
	One_Stype_Info caret_colour;
	One_Stype_Info edge_colour;
	One_Stype_Info line_number_margin;
	One_Stype_Info bookmark_margin;
	One_Stype_Info fold;
	One_Stype_Info fold_active;
	One_Stype_Info fold_margin;
	One_Stype_Info white_space_stybol;
	One_Stype_Info smart_highlighting;
	One_Stype_Info find_mark_style;
	One_Stype_Info mark_style_1;
	One_Stype_Info mark_style_2;
	One_Stype_Info mark_style_3;
	One_Stype_Info mark_style_4;
	One_Stype_Info mark_style_5;
	One_Stype_Info incremental_highlight;
	One_Stype_Info tags_match_highlight;
	One_Stype_Info tags_attribute;
	//One_Stype_Info active_tab_focused;
	//One_Stype_Info active_tab_unfocused;
	//One_Stype_Info active_tab_text;
	//One_Stype_Info inactive_tabs;
	One_Stype_Info url_hoverred;
};

class StyleSet 
{
public:
	StyleSet();
	~StyleSet();

	static void setCommonStyle(QColor foldfgColor_, QColor foldbgColor_, QColor marginsBackgroundColor_, QString colorName);
	static void init();
	static void loadGolbalStyle();
	static void reloadGolbalStyleFromSetFile();
	static void setSkin(int id);
	static void setCurrentStyle(int themes);
	static QString getCurrentStyle();
	static StyleId getCurrentSytleId()
	{
		return StyleId(m_curStyleId);
	}
	static bool isCurrentDeepStyle();

	static QString getStyleName(int styleId);
	static QColor foldfgColor;
	static QColor foldbgColor;
	static QColor marginsBackgroundColor;
	static QColor bookmarkBkColor;
	static int m_curStyleId;

	static void setDefaultStyle();
	static void setLightStyle();
	static void setThinBlueStyle();
	static void setThinYellowStyle();
	static void setRiceYellowStyle();
	static void setSilverStyle();
	static void setLavenderBlushStyle();
	static void setMistyRoseStyle();
	static void setBlackStyle();
	static void setCommonStyle();

	static void setGlobalFgColor(int style, QColor color);
	static void setGlobalBgColor(int style, QColor color);
	static void setGlobalFont(int style, QFont font);

	static GLOBAL_STYLE_OPS* s_global_style;
};
