#include "userlexdef.h"
#include "rcglobal.h"
#include <QSettings>


UserLexDef::UserLexDef(QObject *parent):QObject(parent)
{
}

UserLexDef::~UserLexDef()
{
}


//自己定义一套读取和保存配置的接口

bool UserLexDef::readUserSettings(QString langTagName)
{
	//自定义语言格式。
	//mz:ndd
	//name:xxx
	//mother:xxx none/cpp/html 就三种
	//keword:xxx

	QString userLangFile = QString("notepad/userlang/%1").arg(langTagName);
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, userLangFile);
	qs.setIniCodec("UTF-8");

	if (!qs.contains(QString("mz")))
	{
		//必须包含Mz为Ndd的键，否则错误
		return false;
	}
	if (qs.value("mz").toString() != QString("ndd"))
	{
		return false;
	}

	m_motherLang = qs.value("mother").toString();
	m_keyword = qs.value("keword").toString().toUtf8();
	/*m_keyword.append("\0");*/

	return true;
	
}

void UserLexDef::setKeyword(QString words)
{
	m_keyword = words.toUtf8();
	if (!m_keyword.endsWith('\0'))
	{
		m_keyword.append('\0');
	}
}

void UserLexDef::setExtFileTypes(QString extType)
{
	m_extTypes = extType;
}

QString UserLangMotherToName(UserLangMother words)
{
	QString name;

	switch (words)
	{
	case MOTHER_NONE:
		name = "None";
		break;
	case MOTHER_CPP:
		name = "Cpp";
		break;
	default:
		name = "None";
		break;
	}
	return name;
}

void UserLexDef::setMotherLang(UserLangMother words)
{
	m_motherLang = UserLangMotherToName(words);
}

bool UserLexDef::writeUserSettings(QString langTagName)
{
	//自定义语言格式。
	//mz:ndd
	//name:xxx
	//mother:xxx none/cpp/html 就三种
	//ext:xx xx xx 文件关联后缀名
	//keyword:xxx

	QString userLangFile = QString("notepad/userlang/%1").arg(langTagName);
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, userLangFile);
	qs.setIniCodec("UTF-8");
	qs.clear();

	qs.setValue("mz", langTagName);
	qs.setValue("mother", m_motherLang); 
	qs.setValue("ext", m_extTypes);
	qs.setValue("keyword", m_keyword.data());

	return true;
}

const char * UserLexDef::keywords(int set) const
{
	if (m_keyword.isEmpty())
	{
		return nullptr;
	}

	return m_keyword.data();
}

 
