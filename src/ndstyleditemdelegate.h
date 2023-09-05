#pragma once

#include <QStyledItemDelegate>

class NdStyledItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	NdStyledItemDelegate(QObject *parent);
	virtual ~NdStyledItemDelegate();
	void setFontSize(int size);

protected:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
	int m_defaultFontSize;
};
