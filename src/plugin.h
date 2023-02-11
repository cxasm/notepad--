#pragma once
#include "pluginGl.h"
#include <functional>
class QMenu;
class QsciScintilla;
class QWidget;

typedef int (*NDD_PROC_MAIN_CALLBACK)(QWidget* parent, const QString& strFileName, std::function<QsciScintilla*()>getCurEdit, NDD_PROC_DATA* procData);

int loadProc(const QString& strDirOut, std::function<void(NDD_PROC_DATA&, QMenu*)> funcallback, QMenu* pUserData);
