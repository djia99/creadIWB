#include "stdafx.h"
#include "CMsPptAccessor.h"

static const wchar_t* PROGID_FOR_PPT = L"PowerPoint.Application";
static const wchar_t* PPT_DOCUMENT_NAME = L"Presentations";

CMsPptAccessor::CMsPptAccessor()
{
}


CMsPptAccessor::~CMsPptAccessor()
{
}


const bool CMsPptAccessor::Initialize()
{
	if (FAILED(Init(PROGID_FOR_PPT, PPT_DOCUMENT_NAME)))
		return false;

	m_pApp.PutPropertyByName(_bstr_t(_T("DisplayAlerts")), &_variant_t(FALSE));

	return true;
}

const bool CMsPptAccessor::Open(LPCTSTR lpctszFileName, const bool bShowWindow /*= false*/)
{
	if (NULL == m_pDocument)
	{
		m_hr = E_FAIL;
		return false;
	}

	if (NULL == lpctszFileName || 0 >= _tcslen(lpctszFileName))
	{
		m_hr = E_INVALIDARG;
		return false;
	}


	_variant_t pOutVal;

	m_hr = Do_OLEMethod(DISPATCH_METHOD, &pOutVal, m_pDocument, L"Open", 4,
		_variant_t((long)bShowWindow ? -1 : 0),		// WithWindow ( msoFalse )
		_variant_t((long)0),						// Untitle    ( msoFalse )
		_variant_t((long)-1),						// ReadOnly   ( msoTrue )
		_variant_t(lpctszFileName));				// FileName

	if (FAILED(m_hr))
		return false;

	m_pActiveDocument = pOutVal.pdispVal;

	//GetProperty();

	//if (SUCCEEDED(m_hr))
	//{
	//	DISPPARAMS dp = { NULL, NULL, 0, 0 };
	//	VARIANT result;
	//	VariantInit(&result);
	//	m_hr = m_pActiveDocument->Invoke(0x86, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp, &result, NULL, NULL);
	//	VariantClear(&result);
	//}

	return true;
}

const bool CMsPptAccessor::Close()
{
	if (NULL == m_pApp || NULL == m_pActiveDocument)
	{
		m_hr = E_FAIL;
		return false;
	}

	m_hr = m_pActiveDocument.Invoke0(_bstr_t(_T("Close")), NULL);

	if (FAILED(m_hr))
		return false;

	m_hr = m_pApp.Invoke0(_bstr_t(_T("Quit")), NULL);

	if (FAILED(m_hr))
		return false;

	return true;
}

const int CMsPptAccessor::Print(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName)
{
	if (NULL == m_pActiveDocument)
	{
		m_hr = E_FAIL;
		return -1;
	}

	if (NULL == lpctszOutFileName || 0 >= _tcslen(lpctszOutFileName))
	{
		m_hr = E_INVALIDARG;
		return -1;
	}

	_variant_t pOutVal;
	m_hr = m_pActiveDocument.GetPropertyByName(_bstr_t(_T("PrintOptions")), &pOutVal);

	if (FAILED(m_hr))
		return -1;

	CComPtr<IDispatch> pPrintOptions = pOutVal.pdispVal;

	m_hr = pPrintOptions.PutPropertyByName(_bstr_t(_T("ActivePrinter")), &_variant_t(lpctszPrintName)); // Active Print Name
	if (FAILED(m_hr))
		return -1;

	m_hr = pPrintOptions.PutPropertyByName(_bstr_t(_T("PrintInBackground")), &_variant_t((long)0)); // block when printing
	if (FAILED(m_hr))
		return -1;

	m_hr = pPrintOptions.PutPropertyByName(_bstr_t(_T("OutputType")), &_variant_t((long)1)); // print output is slides
	if (FAILED(m_hr))
		return -1;

	m_hr = pPrintOptions.PutPropertyByName(_bstr_t(_T("RangeType")), &_variant_t((long)1)); // All
	if (FAILED(m_hr))
		return -1;


	VARIANT vOptional;
	vOptional.vt = VT_ERROR;
	vOptional.scode = DISP_E_PARAMNOTFOUND;

	m_hr = Do_OLEMethod(DISPATCH_METHOD, NULL, m_pActiveDocument, L"PrintOut", 5,
		vOptional,						// collate
		_variant_t((long)1),			// copy
		_variant_t(lpctszOutFileName),  // output filename
		vOptional,						// to
		vOptional);					// from

	if (FAILED(m_hr))
		return -1;

	return 1;
}


const bool CMsPptAccessor::PrintToImage(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName)
{
	if (NULL == m_pActiveDocument)
	{
		m_hr = E_FAIL;
		return false;
	}

	if (NULL == lpctszOutFileName || 0 >= _tcslen(lpctszOutFileName))
	{
		m_hr = E_INVALIDARG;
		return false;
	}

	_variant_t pOutVal;
	m_hr = m_pActiveDocument.GetPropertyByName(_bstr_t(_T("PrintOptions")), &pOutVal);

	if (FAILED(m_hr))
		return false;

	CComPtr<IDispatch> pPrintOptions = pOutVal.pdispVal;

	m_hr = pPrintOptions.PutPropertyByName(_bstr_t(_T("ActivePrinter")), &_variant_t(lpctszPrintName)); // Active Print Name
	if (FAILED(m_hr))
		return false;

	m_hr = pPrintOptions.PutPropertyByName(_bstr_t(_T("PrintInBackground")), &_variant_t((long)0)); // block when printing
	if (FAILED(m_hr))
		return false;

	m_hr = pPrintOptions.PutPropertyByName(_bstr_t(_T("OutputType")), &_variant_t((long)1)); // print output is slides
	if (FAILED(m_hr))
		return false;

	m_hr = pPrintOptions.PutPropertyByName(_bstr_t(_T("RangeType")), &_variant_t((long)1)); // All
	if (FAILED(m_hr))
		return false;


		


	VARIANT vOptional;
	vOptional.vt = VT_ERROR;
	vOptional.scode = DISP_E_PARAMNOTFOUND;

	m_hr = Do_OLEMethod(DISPATCH_METHOD, NULL, m_pActiveDocument, L"PrintOut", 5,
		vOptional,						// collate
		_variant_t((long)1),			// copy
		_variant_t(lpctszOutFileName),  // output filename
		vOptional,						// to
		vOptional);					// from

	if (FAILED(m_hr))
		return false;

	return true;
}



const bool CMsPptAccessor::SaveAs(LPCTSTR lpctszOutFileName, int nFormat)
{
	if (NULL == m_pApp || NULL == m_pActiveDocument)
	{
		m_hr = E_FAIL;
		return false;
	}

	if (NULL == lpctszOutFileName || 0 >= _tcslen(lpctszOutFileName))
	{
		m_hr = E_INVALIDARG;
		return false;
	}


	_variant_t FileName(lpctszOutFileName);

	VARIANT vTrue;
	vTrue.vt = VT_BOOL;
	vTrue.boolVal = TRUE;

	VARIANT vFalse;
	vFalse.vt = VT_BOOL;
	vFalse.boolVal = FALSE;

	VARIANT vOptional;
	vOptional.vt = VT_ERROR;
	vOptional.scode = DISP_E_PARAMNOTFOUND;

	_variant_t pOutVal;

	VARIANT result;
	VariantInit(&result);

	m_hr = Do_OLEMethod(DISPATCH_METHOD, &pOutVal, m_pActiveDocument, L"SaveAs", 2,
		COleVariant((short)nFormat),
		FileName);						//  1 FileName


	if (FAILED(m_hr)) {
		return false;
	}
	else {
	}

	return true;
}


void CMsPptAccessor::GetProperty()
{
	//TRACE(L"\>>>>>>>>> pptGetProperty <<<<<\n");
	//_variant_t pOutVal;


	//m_hr = m_pApp.GetPropertyByName(_bstr_t(_T("ActivePresentation")), &pOutVal);

	//if (FAILED(m_hr))
	//	return;
	//TRACE(L"\>>>>>>>>> pptGetProperty 111 <<<<<\n");

	//CComPtr<IDispatch> Pres = pOutVal.pdispVal;

	//////m_hr = m_pApp.GetPropertyByName(_bstr_t(_T("ActivePresentation")), &pOutVal);

	//////if (FAILED(m_hr))
	//////	return;

	//////TRACE(L"\>>>>>>>>> pptGetProperty 222 <<<<<\n");

	////CComPtr<IDispatch> Ap = pOutVal.pdispVal;
	//
	//_variant_t vResult;
	//m_hr = Pres.GetPropertyByName(_bstr_t("PageSetup"), &vResult);


	//if (FAILED(m_hr))
	//	return;


	//TRACE(L"\>>>>>>>>> pptGetProperty 444 <<<<<\n");

	//CComPtr<IDispatch> ps = vResult.pdispVal;
	//

	//_variant_t vOrientation;
	//m_hr = ps.GetPropertyByName(_bstr_t("Orientation"), &vOrientation);

	//if (FAILED(m_hr))
	//	return;

	//TRACE(L"\>>>>>>>>> ppt ori (%d) <<<<<\n", vOrientation.intVal);

}