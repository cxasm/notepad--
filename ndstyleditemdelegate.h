#pragma once

#include <QStyledItemDelegate>

class NdStyledItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	NdStyledItemDelegate(QObject *parent);
	virtual ~NdStyledItemDelegate();

protected:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
