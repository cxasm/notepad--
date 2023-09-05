#include "draglineedit.h"

DragLineEdit::DragLineEdit(QWidget *parent)
	: QLineEdit(parent)
{
	setAcceptDrops(true);
}

DragLineEdit::~DragLineEdit()
{
}

void DragLineEdit::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasFormat("text/uri-list")) //只能打开文本文件
	{
		event->accept(); //可以在这个窗口部件上拖放对象
	}
	else
	{
		event->ignore();
	}
}

void DragLineEdit::dragMoveEvent(QDragMoveEvent* )
{
}

void DragLineEdit::dropEvent(QDropEvent* e)
{
	QList<QUrl> urls = e->mimeData()->urls();
	if (urls.isEmpty())
		return;

	QString fileName = urls.first().toLocalFile();

	if (fileName.isEmpty())
	{
		return;
	}

	if (!QFile::exists(fileName))
	{
		return;
	}

	this->setText(fileName);

	e->accept();

	emit this->returnPressed();
}
