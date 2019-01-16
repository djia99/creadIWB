#include "stdafx.h"
#include "CreadFile.h"


CCreadFile::CCreadFile() :
	m_nWrittenSize(0)
{
	m_strYearMonth = _T("");
	m_strDay = _T("");
	memset(m_strFileName, 0, MAX_PATH * sizeof(char));
	memset(m_strFilePath, 0, MAX_PATH * sizeof(char));
	m_pFile = NULL;
	m_nDay = 0;
	m_bSeekEnd = FALSE;
}


CCreadFile::~CCreadFile()
{
}

void CCreadFile::MakeDirectory(TCHAR *full_path)
{
	TCHAR temp[MAX_PATH], *sp;
	lstrcpy(temp, full_path);
	sp = temp;

	while ((sp = _tcschr(sp, _T('\\')))) {
		if (sp > temp && *(sp - 1) != ':') {
			*sp = _T('\0');
			WIN32_FIND_DATA wfd;
			memset(&wfd, 0, sizeof(wfd));
			HANDLE hFirstFile = FindFirstFile(temp, (LPWIN32_FIND_DATA)&wfd);
			if (hFirstFile == INVALID_HANDLE_VALUE) {
				_tmkdir(temp);
			}
			else {
				FindClose(hFirstFile);
			}
			*sp = _T('\\');
		}
		sp++;
	}

}

void CCreadFile::SetPath(TCHAR* pFileName, TCHAR* path) {
	USES_CONVERSION;
	COleDateTime timeNow = COleDateTime::GetCurrentTime();
	CString strFileName = _T("");
	CString strFilePath = _T("");
	m_strYearMonth = timeNow.Format(_T("%Y%m"));
	m_strDay = timeNow.Format(_T("%d"));


	strFileName.Format(_T("%s_%s.bin"), timeNow.Format(_T("%Y%m%d_%H%M%S")), pFileName);
	strFilePath.Format(_T("%s\\%s\\%s\\%s"), path, m_strYearMonth, m_strDay, strFileName);

	int nIndex = 0;
	HANDLE hFirstFile;
	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof(wfd));
	hFirstFile = FindFirstFile(strFilePath, (LPWIN32_FIND_DATA)&wfd);
	while (hFirstFile != INVALID_HANDLE_VALUE) {
		FindClose(hFirstFile);
		
		if (nIndex > 10) break;
		strFileName.Format(_T("%s_%s(%d).bin"), timeNow.Format(_T("%Y%m%d_%H%M%S")), pFileName, ++nIndex);
		strFilePath.Format(_T("%s\\%s\\%s\\%s"),
			path, m_strYearMonth, m_strDay, strFileName);

		hFirstFile = FindFirstFile(strFilePath, (LPWIN32_FIND_DATA)&wfd);
	}



	if (timeNow.GetDay() != m_nDay) {
		m_nDay = timeNow.GetDay();
		MakeDirectory((LPWSTR)strFilePath.operator LPCWSTR());
	}
		

	strcpy_s(m_strFileName, W2A(strFileName));
	strcpy_s(m_strFilePath, W2A(strFilePath));

}

void CCreadFile::SetRefPath(TCHAR* pFileName, TCHAR* path) {
	USES_CONVERSION;
	COleDateTime timeNow = COleDateTime::GetCurrentTime();
	CString strFileName = _T("");
	CString strFilePath = _T("");
	m_strYearMonth = timeNow.Format(_T("%Y%m"));
	m_strDay = timeNow.Format(_T("%d"));


	strFileName.Format(_T("%s_%s.bin"), timeNow.Format(_T("%Y%m%d_%H%M%S")), pFileName);
	strFilePath.Format(_T("%s\\reference\\%s"), path, strFileName);

	int nIndex = 0;
	HANDLE hFirstFile;
	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof(wfd));
	hFirstFile = FindFirstFile(strFilePath, (LPWIN32_FIND_DATA)&wfd);
	while (hFirstFile != INVALID_HANDLE_VALUE) {
		FindClose(hFirstFile);

		if (nIndex > 10) break;
		strFileName.Format(_T("%s_%s(%d).bin"), timeNow.Format(_T("%Y%m%d_%H%M%S")), pFileName, ++nIndex);
		strFilePath.Format(_T("%s\\reference\\%s"),
			path, strFileName);

		hFirstFile = FindFirstFile(strFilePath, (LPWIN32_FIND_DATA)&wfd);
	}



	if (timeNow.GetDay() != m_nDay) {
		m_nDay = timeNow.GetDay();
		MakeDirectory((LPWSTR)strFilePath.operator LPCWSTR());
	}


	strcpy_s(m_strFileName, W2A(strFileName));
	strcpy_s(m_strFilePath, W2A(strFilePath));

}


BOOL CCreadFile::CreateNewFile(TCHAR* pFileName, TCHAR* pPath) {
	USES_CONVERSION;


	if (m_pFile != NULL) fclose(m_pFile);

	SetPath(pFileName, pPath);


	//char filename[MAX_PATH];
	//_itoa_s(m_nFileNumber++, filename, 10);
	//strcpy_s(m_strFilePath, W2A(pPath));
	//strcat_s(m_strFilePath, "\\");
	//strcat_s(m_strFilePath, filename);
	//strcpy_s(m_strFileName, filename);

	fopen_s(&m_pFile, m_strFilePath, "wb");
	m_nWrittenSize = 0;
	//fwrite(&m_nWrittenSize, 1, sizeof(unsigned long long), m_pFile);
	if (m_pFile == NULL) return FALSE;

	return TRUE;
}

BOOL CCreadFile::CreateRefFile(TCHAR* pFileName, TCHAR* pPath) {
	USES_CONVERSION;
	if (m_pFile != NULL) fclose(m_pFile);

	SetRefPath(pFileName, pPath);

	fopen_s(&m_pFile, m_strFilePath, "wb");
	m_nWrittenSize = 0;

	if (m_pFile == NULL) return FALSE;

	return TRUE;
}



void CCreadFile::CloseFile() {

	fpos_t filepos = 0;
	//fsetpos(m_pFile, &filepos);
	//fwrite(&m_nWrittenSize, 1, sizeof(unsigned long long), m_pFile);
	if (m_pFile != NULL) fclose(m_pFile);
	//CloseHandle(m_pFile);
	m_pFile = NULL;

}


void CCreadFile::SaveDat(void* pBuff, size_t bufSize) {
	m_nWrittenSize += bufSize;
	fwrite(pBuff, bufSize, bufSize, m_pFile);
	return;
}

void CCreadFile::SaveBlock(double* pBuff, int bufSize) {
	m_nWrittenSize += bufSize;
	fwrite(pBuff, 1, bufSize * sizeof(double), m_pFile);
	return;
}

void CCreadFile::SaveBlockByte(BYTE* pBuff, int bufSize) {
	m_nWrittenSize += bufSize;
	fwrite(pBuff, 1, bufSize * sizeof(BYTE), m_pFile);
	return;
}

BOOL CCreadFile::OpenFile(TCHAR* pFilePath) {
	if (m_pFile != NULL) fclose(m_pFile);

	_tfopen_s(&m_pFile, pFilePath, _T("rb"));
	if (m_pFile == NULL) return FALSE;

	return TRUE;
}

size_t CCreadFile::ReadBlock(double* pBuff, int bufSize) {
	if (m_pFile == NULL) return 0;
	size_t ret = fread_s(pBuff, sizeof(double) * bufSize, sizeof(double), bufSize, m_pFile);

	//if (ret <= sizeof(double) * bufSize) {
	if (ret < (size_t)bufSize) {
		m_bSeekEnd = TRUE;
		SeekSet();
	}
	return ret;
}