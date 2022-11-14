#include "ndstyleditemdelegate.h"
#include <QTextDocument>
#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QTextEdit>

NdStyledItemDelegate::NdStyledItemDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
}

NdStyledItemDelegate::~NdStyledItemDelegate()
{
}

//重载使可以支持富文本格式的文字
void NdStyledItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QStyleOptionViewItem viewOption(option);
	initStyleOption(&viewOption, index);
	if (option.state.testFlag(QStyle::State_HasFocus))
		viewOption.state = viewOption.state ^ QStyle::State_HasFocus;

	// ... 省略
	// 设置显示文本为空，使用默认样式
	QStyle *pStyle = viewOption.widget ? viewOption.widget->style() : QApplication::style();

	QTextDocument doc;
	doc.setHtml(viewOption.text);

	viewOption.text.clear();

	pStyle->drawControl(QStyle::CE_ItemViewItem, &viewOption, painter, viewOption.widget);

	QAbstractTextDocumentLayout::PaintContext paintContext;

	QRect textRect = pStyle->subElementRect(QStyle::SE_ItemViewItemText, &viewOption);
	painter->save();
	// 坐标变换，将左上角设置为原点
	painter->translate(textRect.topLeft());
	// 设置HTML绘制区域
	painter->setClipRect(textRect.translated(-textRect.topLeft()));

	doc.documentLayout()->draw(painter, paintContext);

	painter->restore();
}
