#pragma once

#include <QLineEdit>
#include <QLineEdit>
#include <QDropEvent>
#include <QMimeData>
#include <QDragEnterEvent>

class DragLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	DragLineEdit(QWidget*parent);
	virtual ~DragLineEdit();


protected:
	///< 拖动文件到窗口 触发
	void dragEnterEvent(QDragEnterEvent* event) override;
	///< 拖动文件到窗口移动文件 触发
	void dragMoveEvent(QDragMoveEvent* event) override;
	///< 拖动文件到窗口释放文件触发
	void dropEvent(QDropEvent* event) override;
};
