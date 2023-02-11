#pragma once

#include <QMainWindow>
#include <QListWidgetItem>
#include <qscilexer.h>
#include <QCloseEvent>
#include "ui_qtlangset.h"
#include "rcglobal.h"
#include "Qsci/qsciscintilla.h"
#include "Scintilla.h"
#include "SciLexer.h"

class QsciLexer;


enum GLOBAL_STYLE_SET {
	GLOBAL_FONT=0, //字体
	GLOBAL_FONT_SIZE, //字体大小
	GLOBAL_FONT_BOLD,//粗体
	GLOBAL_FONT_UNDERLINE,//下划线
	GLOBAL_FONT_ITALIC,//倾斜
	GLOBAL_FG_COLOR,//前景色
	GLOBAL_BK_COLOR,
};



class QtLangSet : public QMainWindow
{
	Q_OBJECT

public:
	QtLangSet(QString initTag, QWidget *parent = nullptr);
	~QtLangSet();

	/*static void setAllLangFontFgColorToDarkStyle();*/
    //static bool isExistDarkLangSetings();
	void selectInitLangTag(QString initLangTag);
	static bool readLangSettings(QsciLexer * lexer, QString tag, int StyleId = -1);
	static bool readLangOriginSettings(QsciLexer* lexer, QString tag, int StyleId=-1, bool force=false);
private:
	//static void setLangFontFgColorToDarkStyle(LangType langId, QString tag="");
	static void createOneLangStyleFromThemes(int styleId, LangType langId, QString tag = "");
	void startSignSlot();



signals:
	void viewStyleChange(QString tagName, int styleId, QColor & fgColor, QColor & bkColor, QFont & font, bool fontChange);

	//整个lexer发生了大改变
	void viewLexerChange(QString tagName);

protected:
	void closeEvent(QCloseEvent *e);

	void syncShowStyleItemToUI(QListWidgetItem * item);

private slots:

	void slot_itemSelect(QListWidgetItem * item);
	void slot_langListCurRowChanged(int row);
	
	void slot_styleItemSelect(QListWidgetItem * item);
	void slot_styleListCurRowChanged(int row);

	void slot_saveClick();
	void slot_reset();
	void slot_changeFgColor();
	void slot_changeBkColor();
	void slot_fontBoldChange(int state);
	void slot_fontItalicChange(int state);
	void slot_fontUnderlineChange(int state);
	void slot_fontSizeChange(int v);
	void slot_fontChange(const QFont & font);
	void slot_userLangItemSelect(QListWidgetItem * item);

	void slot_useAlobalFgColor(bool);
	void slot_useAlobalBkColor(bool check);
	void slot_useAlobalFont(bool);
	void slot_useAlobalFontSize(bool);
	void slot_useAlobalFontBold(bool);
	void slot_useAlobalFontUnderline(bool);
	void slot_useAlobalFontItalic(bool);
	void on_themesChange(int index);

	void on_showGlobalItem();


private:
	void initLangList();
	bool readLangSetFile(QString langName, QString & keyword, QString & motherLang, QString & extTypes);
	void initUserDefineLangList();
	void setStyleShow(QFont & font, QColor& fcColor, QColor &bkColor);
	static void saveLangeSet(QsciLexer * lexer, int styleId=-1);
	bool saveCurLangSettings();
	void fillForegroundColor(QColor & fcColor);
	void fillBackgroundColor(QColor & bkColor);
	void displayUserMotherLangsStyle(QString langTagName, UserLangMother motherLang);
	void getCurUiFont(QFont & font);
	void updateAllLangeStyleWithGlobal(GLOBAL_STYLE_SET flag);
	void restoreOriginLangOneStyle(GLOBAL_STYLE_SET flag);
	void restoreOriginLangAllStyle();
	void previewAllGoblalChange();

	void getCurUseLexerTags(QVector<QString>& tag);
	void enableFontChangeSensitive(bool isSensitive);
	void updateThemes();

	bool isUseGlobalFgColor();
	bool isUseGlobalBgColor();

	//void setGlobalFgColor(int style, QColor color);
	//void setGlobalBgColor(int style, QColor color);
	//void setGlobalFont(int style, QFont font);

	bool isExistThemesSetings(int styleId);
	void createUserStyleFormThemes(int styleId);
private:
	Ui::QtLangSetClass ui;

	QsciLexer *m_selectLexer;

	int m_selectStyleId;

	//是否当前语法的style发生了变化
	bool m_isStyleChange;

	//是否当前选择风格的子风格发生了变化。
	bool m_isStyleChildChange;
	QsciLexer::StyleData m_curStyleData;

	//第一次以当前edit语法为准
	QString m_initShowLexerTag;

	QListWidgetItem * m_previousSysLangItem;

	//是否是全局修改项
	bool m_isGlobelItem;

	int m_themesId;

	int m_lastThemesId;

public:
	////黑色模式下不显眼的颜色，替换到显眼颜色
	//static QMap<QString, QColor> s_darkColorMap;
};
