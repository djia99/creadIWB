#pragma once
#include "CDocumentAccessor.h"

class CHwpAccessor : public CDocumentAccessor
{
public:
	CHwpAccessor();
	virtual ~CHwpAccessor();

	virtual const bool Initialize();
	virtual const bool Open(LPCTSTR lpctszFileName, const bool bShowWindow = false);
	virtual const bool Close();
	virtual const int Print(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName);
	virtual const bool PrintToImage(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName);
	virtual const bool SaveAs(LPCTSTR lpctszOutFileName, int nFormat);

	void SetSecurityDLLKey(LPCTSTR lpctszKeyName);
	TCHAR m_tszKeyName[MAX_PATH];

private:
	CComPtr<IDispatch> m_pWindows;
	CComPtr<IDispatch> m_pWindow;

private:
	using CDocumentAccessor::IsEnableGetActiveObject;
	using CDocumentAccessor::EnableGetActiveObject;

};

