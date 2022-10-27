#pragma once

#include <qsciscintilla.h>
#include <Scintilla.h>
#include <QDragEnterEvent>
#include <QDropEvent>

typedef sptr_t(*SCINTILLA_FUNC) (sptr_t ptr, unsigned int, uptr_t, sptr_t);
typedef sptr_t SCINTILLA_PTR;

class ScintillaHexEditView : public QsciScintilla
{
	Q_OBJECT

public:
	ScintillaHexEditView(QWidget *parent = Q_NULLPTR);
	virtual ~ScintillaHexEditView();

	void setNoteWidget(QWidget * win);

	sptr_t execute(quint32 Msg, uptr_t wParam = 0, sptr_t lParam = 0) const;

private:
	void init();

protected:
	void dragEnterEvent(QDragEnterEvent * event);
	void dropEvent(QDropEvent * e);


private:
	static bool _SciInit;

	SCINTILLA_FUNC m_pScintillaFunc = nullptr;
	SCINTILLA_PTR  m_pScintillaPtr = 0;

	QWidget* m_NoteWin;
};
