#pragma once
#include "CDocumentAccessor.h"

#define MAX_DISP_ARGS 10
#define DISPARG_NOFREEVARIANT 0x01
#define DISP_FREEARGS 0x02
#define DISP_NOSHOWEXCEPTIONS 0x03

class CMsWordAccessor : public CDocumentAccessor
{
public:
	CMsWordAccessor();
	virtual ~CMsWordAccessor();


	virtual const bool Initialize();
	virtual const bool Open(LPCTSTR lpctszFileName, const bool bShowWindow = false);
	virtual const bool Close();
	virtual const int Print(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName);
	virtual const bool PrintToImage(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName);
	virtual const bool SaveAs(LPCTSTR lpctszOutFileName, int nFormat);



	int			m_iArgCount;
	int			m_iNamedArgCount;
	VARIANTARG	m_aVargs[MAX_DISP_ARGS];
	DISPID		m_aDispIds[MAX_DISP_ARGS + 1];		// one extra for the member name
	LPOLESTR	m_alpszArgNames[MAX_DISP_ARGS + 1];	// used to hold the argnames for GetIDs
	WORD		m_awFlags[MAX_DISP_ARGS];


	void ReleaseVariant(VARIANTARG *pvarg);
	void ClearAllArgs();
	void ClearVariant(VARIANTARG *pvarg);
	BOOL AddArgumentInt2(LPOLESTR lpszArgName, WORD wFlags, int i);
	BOOL AddArgumentDispatch(LPOLESTR lpszArgName, WORD wFlags, IDispatch * pdisp);
	void AddArgumentCommon(LPOLESTR lpszArgName, WORD wFlags, VARTYPE vt);
	BOOL WordInvoke(IDispatch *pdisp, LPOLESTR szMember, VARIANTARG * pvargReturn,
		WORD wInvokeAction, WORD wFlags);
};

