#include "pluginmgr.h"
#include "rcglobal.h"


PluginMgr::PluginMgr(QWidget *parent, QList<NDD_PROC_DATA>& pluginList)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.pluginTable->horizontalHeader()->setStretchLastSection(true); 
	ui.pluginTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui.pluginTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
	//ui.pluginTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	init(pluginList);
}

PluginMgr::~PluginMgr()
{}

void PluginMgr::init(QList<NDD_PROC_DATA>& pluginList)
{
	ui.pluginTable->clearContents();
	ui.pluginTable->setRowCount(0);

	for (int i = 0; i < pluginList.size(); ++i)
	{
		ui.pluginTable->insertRow(i);

		NDD_PROC_DATA v = pluginList.at(i);

		ui.pluginTable->setItem(i, 0, new QTableWidgetItem(v.m_strPlugName));
		ui.pluginTable->setItem(i, 1, new QTableWidgetItem(v.m_version));
		ui.pluginTable->setItem(i, 2, new QTableWidgetItem(v.m_auther));
		ui.pluginTable->setItem(i, 3, new QTableWidgetItem(v.m_strComment));
		ui.pluginTable->setItem(i, 4, new QTableWidgetItem(v.m_strFilePath));
	}
}

void PluginMgr::slot_openPluginDir()
{
	QString strDir = qApp->applicationDirPath();
	QString path = QString("%1/plugin").arg(strDir);

	showFileInExplorer(path);
}
