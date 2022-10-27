#pragma once
#include <qsciscintilla.h>
#include <Scintilla.h>
#include <SciLexer.h>
#include <QMouseEvent>
#include <QMimeData>
#include "common.h"


typedef sptr_t(*SCINTILLA_FUNC) (sptr_t ptr, unsigned int, uptr_t, sptr_t);
typedef sptr_t SCINTILLA_PTR;

const int MARK_BOOKMARK = 24;
const int MARK_HIDELINESBEGIN = 23;
const int MARK_HIDELINESEND = 22;
const int MARK_HIDELINESUNDERLINE = 21;
const int NB_FOLDER_STATE = 7;

struct LanguageName {
	const QString lexerName;
	const QString shortName;
	const QString longName;
	LangType LangID = L_TXT;
	int lexerID = 0;
};

struct PosInfo {
	int line;
	int index;
	PosInfo(int line_, int index_):line(line_),index(index_)
	{

	}
};

class FindRecords;

class ScintillaEditView : public QsciScintilla
{
	Q_OBJECT

public:
	ScintillaEditView(QWidget *parent);
	~ScintillaEditView();

	void setNoteWidget(QWidget* win);
	//void resetDefaultFontStyle();
	sptr_t execute(quint32 Msg, uptr_t wParam = 0, sptr_t lParam = 0) const;

	static QsciLexer * createLexer(int lexerId);
	
	void appendMarkRecord(FindRecords *r);
	void releaseAllMark();
	QList<FindRecords*>& getCurMarkRecord();

	bool gotoPrePos();
	bool gotoNextPos();

	void adjuctSkinStyle();

	//设置文档的缩进参考线
	void setIndentGuide(bool willBeShowed);

	//virtual void setLexer(QsciLexer *lexer = 0) override;

signals:
	void delayWork();

public:
	static const int _SC_MARGE_LINENUMBER;
	static const int _SC_MARGE_SYBOLE;
	static const int _SC_MARGE_FOLDER;
#ifdef Q_OS_WIN //目前这个破玩意压根没用
	static LanguageName langNames[L_EXTERNAL + 1];
#endif
	static const int _markersArray[][NB_FOLDER_STATE];

protected:
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* e) override;
	void mouseDoubleClickEvent(QMouseEvent *e) override;

public slots:
	void updateLineNumberWidth(int lineNumberMarginDynamicWidth=0);
	void slot_linePosChanged(int line, int pos);
private:
	void updateLineNumbersMargin(bool forcedToHide);
	void autoAdjustLineWidth(int xScrollValue);
	void showMargin(int whichMarge, bool willBeShowed);
	void init();

private slots:
	void slot_delayWork();
	void slot_scrollXValueChange(int value);
	

private:

	SCINTILLA_FUNC m_pScintillaFunc;
	SCINTILLA_PTR  m_pScintillaPtr;

	QWidget* m_NoteWin;
	int m_preFirstLineNum;

	QList<FindRecords *> m_curMarkList;

	int m_curPos;
	//回退到上个位置
	QList<int> m_prePosRecord;

	//往前到下个位置
	QList<int> m_nextPosRecord;

public:
	static int s_tabLens;
	static bool s_noUseTab;
	static int s_bigTextSize;
};
