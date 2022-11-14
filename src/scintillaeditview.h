#pragma once
#include <qsciscintilla.h>
#include <Scintilla.h>
#include <SciLexer.h>
#include <QMouseEvent>
#include <QMimeData>
#include <unordered_set>
#include "common.h"
#include "Sorters.h"


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

enum TextCaseType
{
	UPPERCASE,
	LOWERCASE,
	TITLECASE_FORCE,
	TITLECASE_BLEND,
	SENTENCECASE_FORCE,
	SENTENCECASE_BLEND,
	INVERTCASE,
	RANDOMCASE
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

	static QsciLexer * createLexer(int lexerId, QString tag="");
	
	void appendMarkRecord(FindRecords *r);
	void releaseAllMark();
	QList<FindRecords*>& getCurMarkRecord();

	bool gotoPrePos();
	bool gotoNextPos();


	void adjuctSkinStyle();

	//设置文档的缩进参考线
	void setIndentGuide(bool willBeShowed);

	void convertSelectedTextTo(const TextCaseType & caseToConvert);

	void removeAnyDuplicateLines();

	void insertCharsFrom(size_t position, const QByteArray & text2insert) const;


	std::pair<size_t, size_t> getSelectionLinesRange(intptr_t selectionNumber = -1) const;

	void insertNewLineAboveCurrentLine(bool check = true);

	void insertNewLineBelowCurrentLine(bool check = true);

	void sortLines(size_t fromLine, size_t toLine, ISorter * pSort);

	intptr_t lastZeroBasedLineNumber() const {
		auto endPos = execute(SCI_GETLENGTH);
		return execute(SCI_LINEFROMPOSITION, endPos);
	};

	intptr_t getCurrentLineNumber()const {
		return execute(SCI_LINEFROMPOSITION, execute(SCI_GETCURRENTPOS));
	};

signals:
	void delayWork();

private:

	void getText(char * dest, size_t start, size_t end) const;

	QString getGenericTextAsQString(size_t start, size_t end) const;

	QString getEOLString();
	intptr_t replaceTarget(QByteArray & str2replace, intptr_t fromTargetPos, intptr_t toTargetPos) const;
	void appandGenericText(const QByteArray & text2Append) const;


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
	void changeCase(const TextCaseType & caseToConvert, QString & strToConvert) const;
	void clearIndicator(int indicatorNumber);

	void highlightViewWithWord(QString & word2Hilite);

	

private slots:
	void slot_delayWork();
	void slot_scrollXValueChange(int value);
	void slot_clearHightWord();

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

	bool m_hasHighlight;
};
