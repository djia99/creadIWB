#pragma once


#define WRITE_BUFF_SIZE 1000

class CCreadFile
{
public:
	CCreadFile();
	~CCreadFile();


	

	BOOL				CreateNewFile(TCHAR* pFileName, TCHAR* pPath);
	BOOL				CreateRefFile(TCHAR* pFileName, TCHAR* pPath);
	void				CloseFile();
	void				SetPath(TCHAR* pFileName, TCHAR* path);
	void				SetRefPath(TCHAR* pFileName, TCHAR* path);
	void				MakeDirectory(TCHAR* full_path);
	void				SaveDat(void* pBuff, size_t bufSize);
	void				SaveBlock(double* pBuff, int bufSize);
	void				SaveBlockByte(BYTE* pBuff, int bufSize);

	CString				GetDay() { return m_strDay; }
	CString				GetYearMonth() { return m_strYearMonth; }
	char*				GetFileName() { return m_strFileName; }
	char*				GetFilePath() { return m_strFilePath; }
	unsigned long long GetWrittenSize() { return m_nWrittenSize; }
	BOOL				IsSafeHandle() { if (m_pFile == NULL) return FALSE; else return TRUE; }



	BOOL				OpenFile(TCHAR* pFilePath);
	size_t				ReadBlock(double* pBuff, int bufSize);
	void				SeekSet() { if (m_pFile != NULL) fseek(m_pFile, 0L, SEEK_SET); }
	BOOL				IsSeekEnd() { return m_bSeekEnd; }

private:
	FILE*				m_pFile;
	CString				m_strDay;
	CString				m_strYearMonth;
	char				m_strFileName[MAX_PATH];
	char				m_strFilePath[MAX_PATH];
	unsigned long long	m_nWrittenSize;
	int					m_nDay;
	BOOL				m_bSeekEnd;
};
