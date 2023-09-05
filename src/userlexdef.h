#pragma once

#include <QObject>
#include "rcglobal.h"

class UserLexDef:public QObject
{
public:
	UserLexDef(QObject *parent);
	virtual ~UserLexDef();

	bool readUserSettings(QString langTagName);

	void setKeyword(QString words);

	void setExtFileTypes(QString extType);

	void setMotherLang(UserLangMother words);

	bool writeUserSettings(QString langTagName);

	const char *keywords(int set=0) const;

private:
	QByteArray m_keyword;
	QString m_motherLang;
	QString m_extTypes;
};

