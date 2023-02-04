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

//const int MARK_BOOKMARK = 24;
//const int MARK_HIDELINESBEGIN = 23;
//const int MARK_HIDELINESEND = 22;
//const int MARK_HIDELINESUNDERLINE = 21;
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

enum Comment_Mode 
{ 
	cm_comment = 0, 
	cm_uncomment, 
	cm_toggle 
};
const bool L2R = true;
const bool R2L = false;

struct ColumnModeInfo {
	intptr_t _selLpos = 0;
	intptr_t _selRpos = 0;
	intptr_t _order = -1; // 0 based index
	bool _direction = L2R; // L2R or R2L
	intptr_t _nbVirtualCaretSpc = 0;
	intptr_t _nbVirtualAnchorSpc = 0;

	ColumnModeInfo(intptr_t lPos, intptr_t rPos, intptr_t order, bool dir = L2R, intptr_t vAnchorNbSpc = 0, intptr_t vCaretNbSpc = 0)
		: _selLpos(lPos), _selRpos(rPos), _order(order), _direction(dir), _nbVirtualAnchorSpc(vAnchorNbSpc), _nbVirtualCaretSpc(vCaretNbSpc) {};

	bool isValid() const {
		return (_order >= 0 && _selLpos >= 0 && _selRpos >= 0 && _selLpos <= _selRpos);
	};
};

struct SortInPositionOrder {
	bool operator() (ColumnModeInfo& l, ColumnModeInfo& r) {
		return (l._selLpos < r._selLpos);
	}
};

struct SortInSelectOrder {
	bool operator() (ColumnModeInfo& l, ColumnModeInfo& r) {
		return (l._order < r._order);
	}
};

typedef std::vector<ColumnModeInfo> ColumnModeInfos;

const int MASK_FORMAT = 0x03;
const int MASK_ZERO_LEADING = 0x04;
const int BASE_10 = 0x00; // Dec
const int BASE_16 = 0x01; // Hex
const int BASE_08 = 0x02; // Oct
const int BASE_02 = 0x03; // Bin

#define URL_INDIC 8

enum urlMode {
	urlDisable = 0, urlNoUnderLineFg, urlUnderLineFg, urlNoUnderLineBg, urlUnderLineBg,
	urlMin = urlDisable,
	urlMax = urlUnderLineBg
};
#define INDIC_EXPLORERLINK 22

enum Font_Set_Bit {
	Bold_Bit = 0x1,
	Italic_Bit = 0x2,
	Underline_Bit = 0x4,
	Font_Name_Bit = 0x8,
	Font_Size_Bit = 0x10,
	ALL_SET_Bit = 0x1f,
};

class FindRecords;
class CCNotePad;
struct BigTextEditFileMgr;

class ScintillaEditView : public QsciScintilla
{
	Q_OBJECT

public:
	ScintillaEditView(QWidget *parent,bool isBigText = false);
	virtual ~ScintillaEditView();

	virtual void setLexer(QsciLexer *lexer = 0);

	void setNoteWidget(QWidget* win);
	//void resetDefaultFontStyle();
	sptr_t execute(quint32 Msg, uptr_t wParam = 0, sptr_t lParam = 0) const;

	static QsciLexer * createLexer(int lexerId, QString tag="", bool isOrigin=false, int styleId=-1);
	static QString getTagByLexerId(int lexerId);

	void appendMarkRecord(FindRecords *r);
	void releaseAllMark();
	QList<FindRecords*>& getCurMarkRecord();

	bool gotoPrePos();
	bool gotoNextPos();


	/*virtual void adjuctSkinStyle();*/

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

	void bookmarkToggle(intptr_t lineno) const;
	void bookmarkClearAll() const;
	void bookmarkNext(bool forwardScan);
	
	void cutMarkedLines();
	void copyMarkedLines();
	void replaceMarkedline(int ln, QByteArray & str);
	void pasteToMarkedLines();
	void deleteMarkedLines(bool isMarked);
	void inverseMarks();
	intptr_t searchInTarget(QByteArray & text2Find, size_t fromPos, size_t toPos) const;
	intptr_t replaceTargetRegExMode(QByteArray & re, intptr_t fromTargetPos = -1, intptr_t toTargetPos = -1) const;
	intptr_t replaceTarget(QByteArray & str2replace, intptr_t fromTargetPos = -1, intptr_t toTargetPos = -1) const;

	//设置不同风格
	void setStyleOptions();

	ColumnModeInfos getColumnModeSelectInfo();

	void columnReplace(ColumnModeInfos& cmi, QByteArray& str);

	void setMultiSelections(const ColumnModeInfos& cmi);

	void columnReplace(ColumnModeInfos& cmi, int initial, int incr, int repeat, int format, bool isCapital, QByteArray& prefix);

	void setBigTextMode(bool isBigText);
	void showBigTextLineAddr(qint64 fileOffset);
	void showBigTextLineAddr(qint64 fileStartOffset, qint64 fileEndOffset);
	void showBigTextRoLineNum(BigTextEditFileMgr* txtFile, int blockIndex);
	void updateThemes();
	void clearSuperBitLineCache();

	//下面三个函数，是设置全局样式的接口。全局样式不同于每个语法中的样式
	void setGlobalFgColor(int style);
	void setGlobalBgColor(int style);
	void setGlobalFont(int style);
	
	//获取当前块的开始行号。只在大文件只读模式下有效。其余模式下均返回0
	quint32 getBigTextBlockStartLine();
	void setBigTextBlockStartLine(quint32 line);
signals:
	void delayWork();

protected:

	virtual void addHotSpot();
	void setStylesFont(const QFont& f, int style, int setBitMask = ALL_SET_Bit);

private:

	void getText(char * dest, size_t start, size_t end) const;

	QString getGenericTextAsQString(size_t start, size_t end) const;

	QString getEOLString();

	void appandGenericText(const QByteArray & text2Append) const;
	QString getMarkedLine(int ln);
	void deleteMarkedline(int ln);
	void setFoldColor(int margin, QColor fgClack, QColor bkColor, QColor foreActive);
	bool doBlockComment(Comment_Mode currCommentMode);
	bool undoStreamComment(bool tryBlockComment = true);
	bool doStreamComment();
	void getVisibleStartAndEndPosition(int * startPos, int * endPos);
	void changeStyleColor(int sytleId);
	void initStyleColorMenu();

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
	void contextUserDefineMenuEvent(QMenu * menu) override;

	

public slots:
	void updateLineNumberWidth(int lineNumberMarginDynamicWidth=0);
	void slot_linePosChanged(int line, int pos);

private:
	void updateLineNumbersMargin(bool forcedToHide);
	void autoAdjustLineWidth(int xScrollValue);
	void showMargin(int whichMarge, bool willBeShowed);
	void init();
	void bookmarkAdd(intptr_t lineno) const;
	void bookmarkDelete(size_t lineno) const;
	bool bookmarkPresent(intptr_t lineno) const;

	void changeCase(const TextCaseType & caseToConvert, QString & strToConvert) const;
	void clearIndicator(int indicatorNumber);

	void highlightViewWithWord(QString & word2Hilite);

	void slot_markColorGroup(QAction * action);

	void replaceSelWith(const char* replaceText);
private slots:
	void slot_delayWork();
	void slot_scrollYValueChange(int value);
	void slot_clearHightWord();

	void slot_bookMarkClicked(int margin, int line, Qt::KeyboardModifiers state);



private:

	SCINTILLA_FUNC m_pScintillaFunc;
	SCINTILLA_PTR  m_pScintillaPtr;

	CCNotePad* m_NoteWin;
	int m_preFirstLineNum;

	QList<FindRecords *> m_curMarkList;

	int m_curPos;
	//回退到上个位置
	QList<int> m_prePosRecord;

	//往前到下个位置
	QList<int> m_nextPosRecord;

	QPixmap* m_bookmarkPng;

	QMenu* m_styleColorMenu;
	QList<QAction*> m_styleMarkActList;

	bool m_isBigText;//大文本

	quint32 m_curBlockLineStartNum;

	QMap<qint64, quint32> m_addrLineNumMap;//大文本模式下，地址和行号的对应关系。只需要首尾即可
public:
	static int s_tabLens;
	static bool s_noUseTab;
	static int s_bigTextSize;

	bool m_hasHighlight;
};
