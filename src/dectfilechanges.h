#pragma once

#ifdef WIN32

#include <stdio.h>

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

//下面NOMINMAX保证std::min不重复定义
#define NOMINMAX

#include <windows.h>
#include <QtGlobal> 

class DectFileChanges
{
public:
	DectFileChanges();
	~DectFileChanges();
	void AddFile(LPCTSTR szDirectory, DWORD dwNotifyFilter);
	BOOL DetectChanges();
	void Terminate();

	void getDiffFileSize(quint64& lastSize, quint64& curSize);

private:
	quint64 getFileSize(WIN32_FILE_ATTRIBUTE_DATA& data);

private:
	LPCTSTR _szFile = nullptr;
	DWORD _dwNotifyFilter = 0;
	WIN32_FILE_ATTRIBUTE_DATA _lastFileInfo = {};

	quint64 m_lastFileSize;
	quint64 m_curFileSize;

};

#endif
