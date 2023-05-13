#include "dectfilechanges.h"

#ifdef WIN32

DectFileChanges::DectFileChanges():m_lastFileSize(0), m_curFileSize(0)
{
	_szFile = NULL;
	_dwNotifyFilter = 0;
}


DectFileChanges::~DectFileChanges()
{
}


BOOL DectFileChanges::DetectChanges() {

	WIN32_FILE_ATTRIBUTE_DATA fInfo;
	BOOL rValue = FALSE;
	::GetFileAttributesEx(_szFile, GetFileExInfoStandard, &fInfo);

	if ((_dwNotifyFilter & FILE_NOTIFY_CHANGE_SIZE) && (fInfo.nFileSizeHigh != _lastFileInfo.nFileSizeHigh || fInfo.nFileSizeLow != _lastFileInfo.nFileSizeLow)) {
		rValue = TRUE;
	}

	if ((_dwNotifyFilter & FILE_NOTIFY_CHANGE_LAST_WRITE) && (fInfo.ftLastWriteTime.dwHighDateTime != _lastFileInfo.ftLastWriteTime.dwHighDateTime || fInfo.ftLastWriteTime.dwLowDateTime != _lastFileInfo.ftLastWriteTime.dwLowDateTime)) {
		rValue = TRUE;
	}

	if (rValue)
	{
		m_lastFileSize = getFileSize(_lastFileInfo);
	}
	_lastFileInfo = fInfo;

	if (rValue)
	{
		m_curFileSize = getFileSize(_lastFileInfo);
	}
	return rValue;
}

void DectFileChanges::AddFile(LPCTSTR szFile, DWORD dwNotifyFilter)
{
	_szFile = szFile;
	_dwNotifyFilter = dwNotifyFilter;
	::GetFileAttributesEx(szFile, GetFileExInfoStandard, &_lastFileInfo);
}


void DectFileChanges::Terminate()
{
	_szFile = NULL;
	_dwNotifyFilter = 0;
}
quint64 DectFileChanges::getFileSize(WIN32_FILE_ATTRIBUTE_DATA& data)
{
	quint64 fileSize = data.nFileSizeHigh;
	fileSize = (fileSize << 32);
	fileSize += data.nFileSizeLow;

	return fileSize;
}

void DectFileChanges::getDiffFileSize(quint64& lastSize, quint64& curSize)
{
	lastSize = m_lastFileSize;
	curSize = m_curFileSize;
}

#endif
