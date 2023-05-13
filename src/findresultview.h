#pragma once
#include <QWidget>
#include <qsciscintilla.h>
#include <QMouseEvent>
#include <QMenu>

extern const int MARGIN_VER_LINE;
extern const int STYLE_COLOUR_TITLE;
extern const int STYLE_COLOUR_DEST_FILE;
extern const int STYLE_COLOUR_KEYWORD_HIGH;
extern const int STYLE_COLOUR_KEYWORD_BACK_HIGH;
extern const int STYLE_DEEP_COLOUR_KEYWORD_HIGH;
extern const int STYLE_DEEP_COLOUR_DEST_FILE;

class FindResultWin;

class FindResultView  : public QsciScintilla
{
	Q_OBJECT

public:
	FindResultView(QWidget* parent);
	virtual ~FindResultView();

	void setLineBackColorStyle(int line, int style);
	void setLineColorStyle(int line, int pos, int length, int style);

signals:
	void lineDoubleClick(int line);

public slots:
	void on_foldAll();
private slots:

	void on_expandAll();
	//void on_copySelect();
	void on_copySelectLine();
	void on_clear();
	void on_close();
	void on_warp();

protected:
	void mouseDoubleClickEvent(QMouseEvent* e) override;
	void contextUserDefineMenuEvent(QMenu* menu) override;
	void setFoldColor(int margin, QColor fgClack, QColor bkColor, QColor foreActive);

private:
	FindResultWin* m_resultWin;
};
