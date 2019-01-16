#pragma once

#define BUFFER_SIZE 100

class CCreadPipe
{
public:
	CCreadPipe();
	~CCreadPipe();


	void InitPipe();
	HRESULT CreatePipe();
	void ReadPipe();
	void WritePipe();


	void Start(LPVOID lParamThread);
	void End();

	void csLock()		{ m_cs.Lock(); }
	void csUnlock()		{ m_cs.Unlock(); }
	BOOL IsWorking()	{ return m_bWorking; }


private:
	HANDLE					m_hPipe;
	HANDLE					m_hPipeFile;
	TCHAR*					m_pipeName;
	TCHAR					m_readBuffer[BUFFER_SIZE];
	TCHAR					m_writeBuffer[BUFFER_SIZE];
	BOOL					m_bConnected;


	CWinThread*				m_pThd;
	CCriticalSection		m_cs;
	BOOL					m_bWorking;
};

