#pragma once

#include <QMainWindow>
#include "ui_markdownview.h"

class MarkdownView : public QMainWindow
{
	Q_OBJECT

public:
	MarkdownView(QWidget *parent = nullptr);
	~MarkdownView();
	void viewMarkdown(QString& text);

private:
	Ui::MarkdownViewClass ui;
};
