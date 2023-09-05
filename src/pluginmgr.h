#pragma once

#include <QMainWindow>
#include "ui_pluginmgr.h"
#include "pluginGl.h"

class PluginMgr : public QMainWindow
{
	Q_OBJECT

public:
	PluginMgr(QWidget *parent, QList<NDD_PROC_DATA>& pluginList);
	~PluginMgr();

private:
	void init(QList<NDD_PROC_DATA>& pluginList);

private slots:
	void slot_openPluginDir();

private:
	Ui::PluginMgrClass ui;
};
