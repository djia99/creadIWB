#include "stdafx.h"
#include "CCreadPipe.h"

UINT PipeThread(LPVOID pComm);

CCreadPipe::CCreadPipe() :
	m_pThd(nullptr),
	m_bWorking(FALSE),
	m_hPipe(nullptr),
	m_hPipeFile(nullptr),
	m_pipeName(L"\\\\.\\pipe\\ablerec"),
	m_bConnected(FALSE)
{

}


CCreadPipe::~CCreadPipe()
{
	End();

}


void CCreadPipe::Start(LPVOID lParam) {
	if (m_bWorking) {
		return;
	}

	m_cs.Lock();
	m_bWorking = TRUE;
	m_cs.Unlock();

	InitPipe();

	m_pThd = AfxBeginThread(PipeThread, (LPVOID)this);

	if (NULL == m_pThd) {
		return;
	}
}


void CCreadPipe::End() {
	m_cs.Lock();
	m_bWorking = FALSE;
	m_cs.Unlock();
	DWORD dwRet = 0;
	DWORD dwExitCode = 0;


	//if (nullptr != m_pThd) {
	//	dwRet = ::WaitForSingleObject(m_pThd->m_hThread, 10000);

	//	if (dwRet == WAIT_OBJECT_0) {
	//	}
	//	else if (dwRet == WAIT_TIMEOUT) {
	//		::TerminateThread(m_pThd, 0);
	//	}
	//	GetExitCodeThread(m_pThd->m_hThread, &dwExitCode);

	//	//delete m_pThd;
	//	m_pThd = nullptr;
	//}
}




UINT PipeThread(LPVOID pComm)
{
	CCreadPipe* pCls = (CCreadPipe*)pComm;


	while (pCls->IsWorking()) {
		Sleep(1000);
		pCls->csLock();
		if (!pCls->IsWorking()) {
			pCls->csUnlock();
			break;
		}
		else {
			pCls->csUnlock();

			pCls->WritePipe();

		}
	}

	return 0;
}



void CCreadPipe::InitPipe() {
	m_hPipe = ::CreateNamedPipe(m_pipeName,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		1024,
		1024,
		10000,
		NULL);
	if (m_hPipe == INVALID_HANDLE_VALUE)
	{
	}
	else {
		m_bConnected = FALSE;
	}
}


HRESULT CCreadPipe::CreatePipe() {
	if (m_hPipeFile && m_hPipeFile != INVALID_HANDLE_VALUE) {
		return S_OK;
	}

	m_hPipeFile = CreateFile(m_pipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (m_hPipeFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	else {
	}
	return S_OK;
}


void CCreadPipe::ReadPipe() {
	if (m_hPipe != INVALID_HANDLE_VALUE)
	{
		DWORD szRead;
		if (ConnectNamedPipe(m_hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
		{
			if (ReadFile(m_hPipe, m_readBuffer, sizeof(m_readBuffer), &szRead, NULL) != FALSE)
			{
				TRACE(L"\n>>> read buffer[%s] <<<<\n", m_readBuffer);
			}
		}


		DisconnectNamedPipe(m_hPipe);
	}
	else {
		TRACE(L"\n>>> m_hPipe INVALID_HANDLE_VALUE <<<<\n");
	}

	return;
}

void CCreadPipe::WritePipe() {
	if (m_hPipe != INVALID_HANDLE_VALUE)
	{
		if (m_bConnected)
		{
			TRACE(L"\n>>> ConnectNamedPipe <<<<\n");
			DWORD szWrite;
			std::wstring message = L"[record:start]";
			if (WriteFile(m_hPipe, message.c_str(), message.length() * sizeof(TCHAR), &szWrite, NULL))
			{
				if (szWrite > 0) {
				}
			}
			else {
				m_bConnected = FALSE;
			}
			::FlushFileBuffers(m_hPipe);
		}


		if (m_bConnected == FALSE) {
			TRACE(L"\n>>> DisConnectNamedPipe (%d) <<<<\n", m_hPipe);
			DisconnectNamedPipe(m_hPipe);
			//InitPipe();
			m_bConnected = ConnectNamedPipe(m_hPipe, NULL);
		}

		
		//DisconnectNamedPipe(m_hPipe);
	}
	else {
	}
	return;
}