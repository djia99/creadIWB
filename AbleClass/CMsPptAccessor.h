#pragma once
#include "CDocumentAccessor.h"

class CMsPptAccessor : public CDocumentAccessor
{
public:
	CMsPptAccessor();
	~CMsPptAccessor();

	virtual const bool Initialize();
	virtual const bool Open(LPCTSTR lpctszFileName, const bool bShowWindow = false);
	virtual const bool Close();
	virtual const int Print(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName);
	virtual const bool PrintToImage(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName);
	virtual const bool SaveAs(LPCTSTR lpctszOutFileName, int nFormat);


	void GetProperty();

};

