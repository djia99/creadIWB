#include "stdafx.h"
#include "CHwpAccessor.h"

static const wchar_t* PROGID_FOR_HWP = L"HWPFrame.HwpObject";
static const wchar_t* HWP_DOCUMENT_NAME = L"XHwpDocuments";

CHwpAccessor::CHwpAccessor() : 
	m_pWindows(NULL),
	m_pWindow(NULL)
{
	memset(m_tszKeyName, 0, sizeof(m_tszKeyName));
}


CHwpAccessor::~CHwpAccessor()
{
}


const bool CHwpAccessor::Initialize()
{
	if (FAILED(Init(PROGID_FOR_HWP, HWP_DOCUMENT_NAME)))
		return false;

	//LOGER_WRITE(eLog_INFO, _T("Register Module"));

	wcscpy(m_tszKeyName, _T("FilePathCheckerModuleExample"));

	if (_tcslen(m_tszKeyName) > 0)
	{
		m_hr = Do_OLEMethod(DISPATCH_METHOD, NULL, m_pApp, L"RegisterModule", 2,
			_variant_t(_T("FilePathCheckerModuleExample")),
			_variant_t(_T("FilePathCheckDLL")));

		if (FAILED(m_hr))
			return false;
	}

	//LOGER_WRITE(eLog_INFO, _T("GET Hwp Windows"));
	_variant_t pOutVal;
	m_hr = m_pApp.GetPropertyByName(_bstr_t(_T("XHwpWindows")), &pOutVal);

	if (FAILED(m_hr))
		return false;

	m_pWindows = pOutVal.pdispVal;

	VariantClear(&pOutVal);
	VariantInit(&pOutVal);

	//LOGER_WRITE(eLog_INFO, _T("GET Active Hwp Windows"));
	m_hr = m_pWindows.GetPropertyByName(_bstr_t(_T("Active_XHwpWindow")), &pOutVal);

	if (FAILED(m_hr))
		return false;

	m_pWindow = pOutVal.pdispVal;

	return true;
}

const bool CHwpAccessor::Open(LPCTSTR lpctszFileName, const bool bShowWindow /*= false*/)
{
	if (NULL == m_pApp || NULL == m_pDocument || NULL == m_pWindow)
	{
		m_hr = E_FAIL;
		return false;
	}

	if (NULL == lpctszFileName || 0 >= _tcslen(lpctszFileName))
	{
		m_hr = E_INVALIDARG;
		return false;
	}

	m_hr = m_pWindow.PutPropertyByName(_bstr_t(_T("Visible")), &_variant_t(bShowWindow));
	if (FAILED(m_hr))
		return false;


	m_hr = Do_OLEMethod(DISPATCH_METHOD, NULL, m_pApp, L"Open", 3,
		_variant_t(_T("lock:true;forceopen:true")),		// 3 Arguments
		_variant_t(_T("")),
		_variant_t(lpctszFileName));

	if (FAILED(m_hr))
		return false;

	_variant_t pOutVal;
	m_hr = m_pDocument.GetPropertyByName(_bstr_t(_T("Active_XHwpDocument")), &pOutVal);
	if (FAILED(m_hr))
		return false;


	//OutputDebugString(lpctszFileName);

	m_pActiveDocument = pOutVal.pdispVal;
	_variant_t pOutVal2;
	m_hr = m_pApp.GetPropertyByName(_bstr_t(_T("PageCount")), &pOutVal2);

	TRACE(_T("\n>>>>>> page count (%d) <<<<<<<<<<<\n"), pOutVal2.lVal);


	return true;
}

const bool CHwpAccessor::Close()
{
	if (NULL == m_pWindows)
	{
		m_hr = E_FAIL;
		return false;
	}

	m_hr = m_pWindows.Invoke1(_bstr_t(_T("Close")), &_variant_t(FALSE));

	if (FAILED(m_hr))
		return false;

	m_hr = m_pApp.Invoke0(_bstr_t(_T("Quit")));

	if (FAILED(m_hr))
		return false;

	return true;
}

const int CHwpAccessor::Print(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName)
{
	if (NULL == m_pApp)
	{
		m_hr = E_FAIL;
		return -1;
	}

	int nReturn = 1;

	// 각 파라미터를 얻어온다.

	CComPtr<IDispatch> HAction;
	CComPtr<IDispatch> HParameterSet;
	CComPtr<IDispatch> HPrint;
	CComPtr<IDispatch> HSet;

	CComPtr<IDispatch> HSecDef;
	CComPtr<IDispatch> HSecDefSet;
	CComPtr<IDispatch> HPageDef;


	_variant_t pOutVal;
	m_hr = m_pApp.GetPropertyByName(_bstr_t(_T("HAction")), &pOutVal);

	if (FAILED(m_hr))
		return -1;

	HAction = pOutVal.pdispVal;

	VariantClear(&pOutVal);
	VariantInit(&pOutVal);


	m_hr = m_pApp.GetPropertyByName(_bstr_t(_T("HParameterSet")), &pOutVal);

	if (FAILED(m_hr))
		return false;

	HParameterSet = pOutVal.pdispVal;

	VariantClear(&pOutVal);
	VariantInit(&pOutVal);


	m_hr = HParameterSet.GetPropertyByName(_bstr_t(_T("HSecDef")), &pOutVal);

	if (FAILED(m_hr))
		return -1;

	HSecDef = pOutVal.pdispVal;

	VariantClear(&pOutVal);
	VariantInit(&pOutVal);


	m_hr = HSecDef.GetPropertyByName(_bstr_t(_T("HSet")), &pOutVal);

	if (FAILED(m_hr))
		return -1;

	HSecDefSet = pOutVal.pdispVal;

	VariantClear(&pOutVal);
	VariantInit(&pOutVal);



	m_hr = HSecDef.GetPropertyByName(_bstr_t(_T("PageDef")), &pOutVal);

	if (FAILED(m_hr))
		return -1;

	HPageDef = pOutVal.pdispVal;

	VariantClear(&pOutVal);
	VariantInit(&pOutVal);






	m_hr = HAction.Invoke2(_bstr_t(_T("GetDefault")), &_variant_t(_T("PageSetup")), &_variant_t(HSecDefSet), NULL);

	if (FAILED(m_hr))
		return -1;



	m_hr = HPageDef.GetPropertyByName(_bstr_t(_T("Landscape")), &pOutVal);


	if (FAILED(m_hr))
		return -1;


	int nLandscape = (int)pOutVal;

	if (nLandscape == 1) nReturn = 2;

	//CString str;
	//str.Format(_T("방향 : %d"), nLandscape);
	//OutputDebugString(str);



	VariantClear(&pOutVal);
	VariantInit(&pOutVal);



	m_hr = HParameterSet.GetPropertyByName(_bstr_t(_T("HPrint")), &pOutVal);

	if (FAILED(m_hr))
		return -1;





	HPrint = pOutVal.pdispVal;

	VariantClear(&pOutVal);
	VariantInit(&pOutVal);

	m_hr = HPrint.GetPropertyByName(_bstr_t(_T("HSet")), &pOutVal);

	if (FAILED(m_hr))
		return -1;

	HSet = pOutVal.pdispVal;

	VariantClear(&pOutVal);
	VariantInit(&pOutVal);

	m_hr = HAction.Invoke2(_bstr_t(_T("GetDefault")), &_variant_t(_T("Print")), &_variant_t(HSet), NULL);

	if (FAILED(m_hr))
		return -1;

	// 최종적으로 Print에 대한 Default값을 얻어온 후, 필요한 Property만 변경한다.

	m_hr = HPrint.PutPropertyByName(_bstr_t(_T("PrinterName")), &_variant_t(lpctszPrintName));

	if (FAILED(m_hr))
		return -1;

	m_hr = HPrint.PutPropertyByName(_bstr_t(_T("PrintToFile")), &_variant_t(0));

	if (FAILED(m_hr))
		return -1;



	m_hr = HPrint.PutPropertyByName(_bstr_t(_T("Filename")), &_variant_t(lpctszOutFileName));

	if (FAILED(m_hr))
		return -1;

	// Print 진행

	m_hr = HAction.Invoke2(_bstr_t(_T("Execute")), &_variant_t(_T("Print")), &_variant_t(HSet), NULL);

	if (FAILED(m_hr))
		return -1;

	return nReturn;
}


void CHwpAccessor::SetSecurityDLLKey(LPCTSTR lpctszKeyName)
{
	if (NULL == lpctszKeyName || 0 >= _tcslen(lpctszKeyName))
		return;

	_tcscpy_s(m_tszKeyName, _tcslen(m_tszKeyName)+1, lpctszKeyName);

}






const bool CHwpAccessor::SaveAs(LPCTSTR lpctszOutFileName, int nFormat)
{
	if (NULL == m_pApp || NULL == m_pActiveDocument || NULL == m_pWindow)
	{
		m_hr = E_FAIL;
		return false;
	}

	VARIANTARG varFormat, varArg;
	VariantInit(&varFormat);
	varFormat.vt = VT_BSTR;
	varFormat.bstrVal = L"PDF";
	VariantInit(&varArg);
	varArg.vt = VT_BSTR;
	varArg.bstrVal = L"";

	m_hr = Do_OLEMethod(DISPATCH_METHOD, NULL, m_pApp, L"SaveAs", 3,
		_variant_t(_T("")),
		varFormat,
		_variant_t(lpctszOutFileName));


	if (FAILED(m_hr)) {
		OutputDebugString(_T("biz:한글 저장 실패"));
		//return false;
	}
	else {
		OutputDebugString(_T("biz:한글 저장 성공"));
	}

	return true;
}


const bool CHwpAccessor::PrintToImage(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName)
{
	if (NULL == m_pApp)
	{
		m_hr = E_FAIL;
		return false;
	}

	// 각 파라미터를 얻어온다.

	CComPtr<IDispatch> HAction;
	CComPtr<IDispatch> HParameterSet;
	CComPtr<IDispatch> HPrintToImage;
	CComPtr<IDispatch> HSet;


	_variant_t pOutVal;
	m_hr = m_pApp.GetPropertyByName(_bstr_t(_T("HAction")), &pOutVal);

	if (FAILED(m_hr))
		return false;

	HAction = pOutVal.pdispVal;

	VariantClear(&pOutVal);
	VariantInit(&pOutVal);


	m_hr = m_pApp.GetPropertyByName(_bstr_t(_T("HParameterSet")), &pOutVal);

	if (FAILED(m_hr))
		return false;

	HParameterSet = pOutVal.pdispVal;

	VariantClear(&pOutVal);
	VariantInit(&pOutVal);




	m_hr = HAction.Invoke2(_bstr_t(_T("GetDefault")), &_variant_t(_T("PrintToImage")), &_variant_t(HParameterSet), NULL);

	if (FAILED(m_hr))
		return false;


	HPrintToImage = pOutVal.pdispVal;


	VariantClear(&pOutVal);
	VariantInit(&pOutVal);



	m_hr = HParameterSet.GetPropertyByName(_bstr_t(_T("HPrintToImage")), &pOutVal);

	if (FAILED(m_hr))
		return false;



	HPrintToImage = pOutVal.pdispVal;

	VariantClear(&pOutVal);
	VariantInit(&pOutVal);

	m_hr = HPrintToImage.GetPropertyByName(_bstr_t(_T("HSet")), &pOutVal);

	if (FAILED(m_hr))
		return false;

	HSet = pOutVal.pdispVal;

	VariantClear(&pOutVal);
	VariantInit(&pOutVal);


	// 최종적으로 Print에 대한 Default값을 얻어온 후, 필요한 Property만 변경한다.

	m_hr = HPrintToImage.PutPropertyByName(_bstr_t(_T("Format")), &_variant_t(_T("JPG")));

	if (FAILED(m_hr))
		return false;



	m_hr = HPrintToImage.PutPropertyByName(_bstr_t(_T("Filename")), &_variant_t(lpctszOutFileName));

	if (FAILED(m_hr))
		return false;


	m_hr = HPrintToImage.PutPropertyByName(_bstr_t(_T("ColorDepth")), &_variant_t(24));

	if (FAILED(m_hr))
		return false;

	m_hr = HPrintToImage.PutPropertyByName(_bstr_t(_T("Resolution")), &_variant_t(300));

	if (FAILED(m_hr))
		return false;

	// Print 진행

	m_hr = HAction.Invoke2(_bstr_t(_T("Execute")), &_variant_t(_T("PrintToImage")), &_variant_t(HSet), NULL);

	if (FAILED(m_hr))
		return false;

	return true;
}
