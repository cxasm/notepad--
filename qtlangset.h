#pragma once

#include <QMainWindow>
#include <QListWidgetItem>
#include <qscilexer.h>
#include <QCloseEvent>
#include "ui_qtlangset.h"

class QsciLexer;

class QtLangSet : public QMainWindow
{
	Q_OBJECT

public:
	QtLangSet(QString initTag, QWidget *parent = nullptr);
	~QtLangSet();

	void startSignSlot();

	static 	bool readLangSettings(QsciLexer * lexer, QString tag);

#if 0
	void setCurSelectLang(QString tag);
#endif

signals:
	void viewStyleChange(int lexerId, int styleId, QColor & fgColor, QColor & bkColor, QFont & font, bool fontChange);

protected:
	void closeEvent(QCloseEvent *e);

private slots:

	void slot_itemSelect(QListWidgetItem * item, QListWidgetItem * previous);
	void slot_styleItemSelect(QListWidgetItem * item, QListWidgetItem *previous);
	void slot_saveClick();

	void slot_changeFgColor();
	void slot_changeBkColor();

	void slot_fontBoldChange(int state);
	void slot_fontItalicChange(int state);
	void slot_fontUnderlineChange(int state);
	void slot_fontSizeChange(int v);
	void slot_fontChange(const QFont & font);
private:
	void initLangList();
	void setStyleShow(QFont & font, QColor& fcColor, QColor &bkColor);
	bool saveCurLangSettings();
	void fillForegroundColor(QColor & fcColor);
	void fillBackgroundColor(QColor & bkColor);
	void getCurUiFont(QFont & font);

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

	bool m_first;
};
