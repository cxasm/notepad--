#include "mytreeview.h"

MyTreeView::MyTreeView(QWidget *parent)
	: QTreeView(parent)
{}

MyTreeView::~MyTreeView()
{}

void MyTreeView::getSelectedIndexes(QModelIndexList & selectList)
{
	selectList = this->selectedIndexes();
}
