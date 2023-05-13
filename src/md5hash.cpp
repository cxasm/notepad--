#include "md5hash.h"
#include "ccnotepad.h"
#include "ctipwin.h"

#include <QCryptographicHash>
#include <QFile>
#include <QFileDialog>
#include <QClipboard>

Md5hash::Md5hash(QWidget *parent)
	: QWidget(parent), m_isFile(false)
{
	ui.setupUi(this);

	
	m_btGroup.addButton(ui.md4RadioBt, 0);
	m_btGroup.addButton(ui.md5RadioBt, 1);
	m_btGroup.addButton(ui.sha1RadioBt, 2);
	m_btGroup.addButton(ui.sha256RadioBt, 4);
	m_btGroup.addButton(ui.sha3RadioBt, 12);
	m_btGroup.addButton(ui.kec256RadioBt, 8);

#if (QT_VERSION <= QT_VERSION_CHECK(5,15,0))
	connect(&m_btGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &Md5hash::on_methodIdChange);
#else
	connect(&m_btGroup, &QButtonGroup::idClicked, this, &Md5hash::on_methodIdChange);
#endif

	connect(ui.srcTextEdit,&QPlainTextEdit::textChanged,this,&Md5hash::on_hash);
}

Md5hash::~Md5hash()
{}

void Md5hash::slot_select()
{
	QFileDialog fd(this, QString(), CCNotePad::s_lastOpenDirPath);
	fd.setFileMode(QFileDialog::ExistingFile);
	m_fileList.clear();

	if (fd.exec() == QDialog::Accepted)   //如果成功的执行
	{
		m_fileList = fd.selectedFiles();      //返回文件列表的名称

		if (!m_fileList.isEmpty())
		{
			m_isFile = true;
			ui.srcTextEdit->setPlainText(m_fileList.join("\n"));
		}

	}
	else
	{
		fd.close();
	}
}

void Md5hash::on_methodIdChange(int id)
{
	on_hash();
}

void Md5hash::on_hash()
{
	QCryptographicHash::Algorithm method = QCryptographicHash::Md5;

	if (ui.md5RadioBt->isChecked())
	{
		method = QCryptographicHash::Md5;
	}
	else if (ui.md4RadioBt->isChecked())
	{
		method = QCryptographicHash::Md4;
	}
	else if (ui.sha1RadioBt->isChecked())
	{
		method = QCryptographicHash::Sha1;
	}
	else if (ui.sha256RadioBt->isChecked())
	{
		method = QCryptographicHash::Sha256;
	}
	else if (ui.sha3RadioBt->isChecked())
	{
		method = QCryptographicHash::Sha3_256;
	}
	else if (ui.kec256RadioBt->isChecked())
	{
		method = QCryptographicHash::Keccak_256;
	}

	//如果是文本
	if (!m_isFile)
	{
		QString text = ui.srcTextEdit->toPlainText();

		QByteArray data = text.toUtf8();

		if (!text.isEmpty())
		{
			QByteArray result = QCryptographicHash::hash(data, method);
			ui.hashTextEdit->setPlainText(result.toHex());
		}
	}
	else
	{
		QCryptographicHash fileHash(method);
		QByteArray rs;

		QList<QByteArray> result;

		for (int i = 0; i < m_fileList.size(); ++i)
		{
			rs.clear();
			QFile file(m_fileList.at(i));
			if (file.open(QIODevice::ReadOnly))
			{
				if (fileHash.addData(&file))
				{
					rs = fileHash.result();
					result.append(rs.toHex());
				}
				else
				{
					result.append("Error Null");
				}
				file.close();
			}
			else
			{
				result.append("Error Null");
			}
			
		}
		for (int i = 0; i < result.size(); ++i)
		{
			QString info = QString("File %1 cyp hash is \n%2").arg(m_fileList.at(i)).arg(QString(result.at(i)));
			ui.hashTextEdit->appendPlainText(info);
		}
		m_isFile = false;
	}
}

void Md5hash::on_copyClipboard()
{
	if (!ui.hashTextEdit->toPlainText().isEmpty())
	{
		QClipboard* clipboard = QApplication::clipboard();
		clipboard->setText(ui.hashTextEdit->toPlainText());

		CTipWin::showTips(this, tr("Copy to clipboard Finished!"), 1200);
	}
}