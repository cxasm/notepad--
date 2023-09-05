#include "extlexermanager.h"

//专门用来管理用户自定义的Ext 和 词法Lexer关联的类。
//给出一个文件的ext后缀，快速告知该使用什么lexer进行语法高亮

ExtLexerManager* ExtLexerManager::s_instance = nullptr;

ExtLexerManager::ExtLexerManager()
{
}

ExtLexerManager * ExtLexerManager::getInstance()
{
	if (s_instance == nullptr)
	{
		s_instance = new ExtLexerManager();
	}
	return s_instance;
}

ExtLexerManager::~ExtLexerManager()
{
	m_extToLexerIdMap.clear();
}

int ExtLexerManager::size()
{
	return m_extToLexerIdMap.size();
}

bool ExtLexerManager::contains(QString ext)
{
	return m_extToLexerIdMap.contains(ext);
}

void ExtLexerManager::remove(QString ext)
{
	if (m_extToLexerIdMap.contains(ext))
	{
		m_extToLexerIdMap.remove(ext);
	}
}


//ext:文件的后缀名 langTagName:语言的tag名称 
//lexerId 语法的id，如果是用户自定义，则必然是L_USER_TXT,L_USER_CPP,L_USER_HTML,L_USER_JS,L_USER_XML, L_USER_INI 中的一个。
//langTagName:只有在用户定义语言下，才需要tagName。因为非用户定义的lexer，其tagname是固定的。
void ExtLexerManager::addNewExtType(QString ext, LangType lexerId, QString langTagName)
{
	LexerInfo value(lexerId, langTagName);
	m_extToLexerIdMap.insert(ext,value);
}

bool ExtLexerManager::getLexerTypeByExt(QString ext, LexerInfo& lexer)
{
	if (m_extToLexerIdMap.contains(ext))
	{
		lexer = m_extToLexerIdMap[ext];
		return true;
	}
	return false;
}

//列出tag语言下面的所有关联的文件后缀列表
void ExtLexerManager::getExtlistByLangTag(QString tag, QStringList& extList)
{
	for (QMap<QString, LexerInfo>::iterator it = m_extToLexerIdMap.begin(); it != m_extToLexerIdMap.end(); ++it)
	{
		LexerInfo& v = it.value();
		if (v.tagName == tag)
		{
			extList.append(it.key());
		}

	}
}

//列出tag语言下面的所有关联的文件后缀列表
void ExtLexerManager::getExtlistByLangTag(QMap<QString,QStringList>& extLangMap)
{
	for (QMap<QString, LexerInfo>::iterator it = m_extToLexerIdMap.begin(); it != m_extToLexerIdMap.end(); ++it)
	{
		LexerInfo& v = it.value();

		if (extLangMap.contains(v.tagName))
		{
			extLangMap[v.tagName].append(it.key());
		}
		else
		{
			extLangMap[v.tagName] = QStringList(it.key());
		}
	}
}
