#include "nddpluginapi.h"
#include "ccnotepad.h"

NddPluginApi::NddPluginApi(QObject *parent)
	: QObject(parent)
{}

NddPluginApi::~NddPluginApi()
{}

void NddPluginApi::setMainNotePad(QWidget * pWidget)
{
	m_mainNotePad = pWidget;
}

//注意这里实际返回的是ScintillaEditView
QsciScintilla* NddPluginApi::getCurrentEidtHandle()
{
	CCNotePad* pNotepad = dynamic_cast<CCNotePad*>(m_mainNotePad);
	if (pNotepad != nullptr)
	{
		return pNotepad->getCurEditView();
	}
	return nullptr;
}
