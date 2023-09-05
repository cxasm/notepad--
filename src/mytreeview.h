#pragma once

#include <QTreeView>
#include <QModelIndexList>

class MyTreeView  : public QTreeView
{
public:
	MyTreeView(QWidget *parent);
	~MyTreeView();
	void getSelectedIndexes(QModelIndexList& selectList);
};
