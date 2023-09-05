#include "filelistview.h"
#include "rcglobal.h"
#include "ccnotepad.h"
#include <QListWidgetItem>
#include <QMenu>


//当显示文件多了以后，把所有文件列在该列表中

FileListView::FileListView(QWidget *parent)
	: QWidget(parent), m_pNotepad(nullptr)
{
	ui.setupUi(this);

	connect(ui.filelistWidget, &QListWidget::itemDoubleClicked, this, &FileListView::itemDoubleClicked);

	setContextMenuPolicy(Qt::CustomContextMenu);  //设置枚举值

	connect(this, &QListWidget::customContextMenuRequested, this, &FileListView::slot_ShowPopMenu);
}

FileListView::~FileListView()
{
	m_fileEditMap.clear();
}

void FileListView::setNotepadWin(QWidget* pNotepad)
{
	m_pNotepad = pNotepad;
}

//右键菜单
void FileListView::slot_ShowPopMenu(const QPoint& pos)
{
	QListWidgetItem* curItem = ui.filelistWidget->itemAt(pos);
	if (curItem != nullptr)
	{
		QMenu* menu = new QMenu(this);

		QAction* actionAdd = menu->addAction(tr("Show File in Explorer"), this, [&]() {
			showFileInExplorer(curItem->text());
		});

		menu->addAction(tr("Close File"), this, [&]() {

			QString filePath = curItem->text();

			if (m_fileEditMap.contains(filePath))
			{
				CCNotePad* pNotePad = dynamic_cast<CCNotePad*>(m_pNotepad);

				if (m_pNotepad != nullptr)
				{
					pNotePad->closeFileByEditWidget(m_fileEditMap.value(curItem->text()).pEditWidget);
				}
				//注意这里不需要在删除m_fileEditMap元素，因为closeFileByEditWidget里面会调用。
			}
		});

		//没有名称表示是对齐的item，不存在对应的文件，只是占位
		if (curItem->text().isEmpty())
		{
			actionAdd->setEnabled(false);
		}

		if (menu)
		{
			menu->setAttribute(Qt::WA_DeleteOnClose);
			menu->exec(QCursor::pos());
		}
	}
}

void FileListView::delFileItem(QString & filePath)
{
	if (m_fileEditMap.contains(filePath))
	{
		m_fileEditMap.remove(filePath);
		QList<QListWidgetItem*> items = ui.filelistWidget->findItems(filePath, Qt::MatchFixedString);

		if (!items.isEmpty())
		{
			delete items.at(0);
		}

	}
}

QWidget* FileListView::getWidgetByFilePath(QString filePath)
{
	if (m_fileEditMap.contains(filePath))
	{
		return m_fileEditMap.value(filePath).pEditWidget;
	}
	return nullptr;
}

void FileListView::setCurItem(QString filePath)
{
	if (m_fileEditMap.contains(filePath))
	{
		ui.filelistWidget->setCurrentItem(m_fileEditMap.value(filePath).pListItem);
	}
}

void FileListView::addFileItem(QString& filePath, QWidget* edit)
{
	if (!m_fileEditMap.contains(filePath))
	{
		QListWidgetItem* newItem = new QListWidgetItem(filePath);
		ui.filelistWidget->addItem(newItem);

		m_fileEditMap.insert(filePath, FileListItemData(edit, newItem));
	}
}

