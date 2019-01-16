#include "stdafx.h"
#include "CMsWordAccessor.h"


static const wchar_t* PROGID_FOR_WORD = L"Word.Application";
static const wchar_t* WORD_DOCUMENT_NAME = L"Documents";


CMsWordAccessor::CMsWordAccessor()
{
}


CMsWordAccessor::~CMsWordAccessor()
{
}


const bool CMsWordAccessor::Initialize()
{
	if (FAILED(Init(PROGID_FOR_WORD, WORD_DOCUMENT_NAME)))
		return false;

	m_pApp.PutPropertyByName(_bstr_t(_T("DisplayAlerts")), &_variant_t(FALSE));

	return true;
}

const bool CMsWordAccessor::Open(LPCTSTR lpctszFileName, const bool bShowWindow /*= false*/)
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


	//	bShowWindow  = true;
	// word window visible
	m_hr = m_pApp.PutPropertyByName(_bstr_t(_T("Visible")), &_variant_t((long)bShowWindow ? 1 : 0));
	if (FAILED(m_hr))
		return false;

	{
		_variant_t pOutVal;

		_variant_t FileName(lpctszFileName);

		VARIANT vTrue;
		vTrue.vt = VT_BOOL;
		vTrue.boolVal = TRUE;

		VARIANT vFalse;
		vFalse.vt = VT_BOOL;
		vFalse.boolVal = FALSE;

		VARIANT vOptional;
		vOptional.vt = VT_ERROR;
		vOptional.scode = DISP_E_PARAMNOTFOUND;


		m_hr = Do_OLEMethod(DISPATCH_METHOD, &pOutVal, m_pDocument, L"Open", 13,
			vTrue,							// 13 OpenAndRepair
			vTrue,							// 12 document Visible ( if value is vFalse, can't print document )
			vOptional,						// 11 Encoding
			vOptional,						// 10 Format
			vOptional,						//  9 WritePasswordTemplate
			vOptional,						//  8 WritePasswordDocument
			vOptional,						//  7 Revert
			vOptional,						//  6 PasswordTemplate
			vOptional,						//  5 PasswordDocument
			vFalse,							//  4 AddToRecentFiles
			vFalse,							//  3 ReadOnly
			vOptional,						//  2 ConfirmConversions
			FileName);						//  1 FileName

		if (FAILED(m_hr))
			return false;

		m_pActiveDocument = pOutVal.pdispVal;


	
		// get pages
		{
			VARIANTARG varg1, varg2;
			int wdStatisticPages = 2;		//MsWord constant
											//Get ActiveDocument object
			ClearAllArgs();
			AddArgumentInt2(L"Statistic", 0, wdStatisticPages);
			if (WordInvoke(m_pActiveDocument, L"ComputeStatistics", &varg2, DISPATCH_METHOD, 0)) {
				//Find how many lines there
				VARTYPE Type = varg2.vt;
				if (Type != VT_I4)
					return -1;
			}


		}


	}

	return true;
}

const bool CMsWordAccessor::Close()
{
	if (NULL == m_pApp || NULL == m_pActiveDocument)
	{
		m_hr = E_FAIL;
		return false;
	}

	m_hr = m_pActiveDocument.Invoke1(_bstr_t(_T("Close")), &_variant_t((long)0), NULL);

	if (FAILED(m_hr))
		return false;

	m_hr = m_pApp.Invoke1(_bstr_t(_T("Quit")), &_variant_t((long)0), NULL);

	if (FAILED(m_hr))
		return false;

	return true;
}

const int CMsWordAccessor::Print(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName)
{
	if (NULL == m_pApp)
	{
		m_hr = E_FAIL;
		return -1;
	}

	if (NULL == lpctszOutFileName || 0 >= _tcslen(lpctszOutFileName))
	{
		m_hr = E_INVALIDARG;
		return -1;
	}



	// Active Printer를 변경할 때, Default로 바꾸지 않으면 User Prompt가 발생하여
	// 어쩔수 없이 기본 프린터 이름을 받아놨다가 다시 바꾸는 방식으로 변경했다.
	bool bGetActivePrinter = false;

	_variant_t curActivePrinter;
	if (SUCCEEDED(m_pApp.GetPropertyByName(_bstr_t(_T("ActivePrinter")), &curActivePrinter)))
	{
		if (curActivePrinter != _variant_t(lpctszPrintName))
			bGetActivePrinter = true;
	}


	_variant_t pOutVal;
	m_hr = m_pApp.GetPropertyByName(_bstr_t(_T("WordBasic")), &pOutVal);

	if (FAILED(m_hr))
		return -1;

	CComPtr<IDispatch> pWordBasic = pOutVal.pdispVal;

	// Print Setup, Print Dialog Appear, if second param is 1
	m_hr = pWordBasic.Invoke2(_bstr_t(_T("FilePrintSetup")), &_variant_t(lpctszPrintName), &_variant_t((short)0), NULL);

	if (FAILED(m_hr))
		return -1;


	VARIANT vTrue;
	vTrue.vt = VT_BOOL;
	vTrue.boolVal = TRUE;

	VARIANT vFalse;
	vFalse.vt = VT_BOOL;
	vFalse.boolVal = FALSE;

	VARIANT vOptional;
	vOptional.vt = VT_ERROR;
	vOptional.scode = DISP_E_PARAMNOTFOUND;

	m_hr = Do_OLEMethod(DISPATCH_METHOD, NULL, m_pApp, L"PrintOut", 19,
		vOptional,						// 19 PrintZoomPaperHeight
		vOptional,						// 18 PrintZoomPaperWidth
		vOptional,						// 17 PrintZoomRow
		vOptional,						// 16 PrintZoomColumn
		vOptional,						// 15 ManualDuplexPrint
		vOptional,						// 14 ActivePrinterMacGX
		vOptional,						// 13 FileName ( The path and file name of the document to be printed. If this argument is omitted, Word prints the active document. (Available only with the Application object.) )
		vOptional,						// 12 Collate
		vTrue,							// 11 PrintToFile
		vOptional,						// 10 PageType
		vOptional,						//  9 Pages
		vOptional,						//  8 Copies
		vOptional,						//  7 Item
		vOptional,						//  6 To
		vOptional,						//  5 From
		_variant_t(lpctszOutFileName),	//	4 OutputFileName
		vOptional,						//  3 Range
		vFalse,							//  2 Append
		vFalse);						//  1 Background

	if (true == bGetActivePrinter)
		pWordBasic.Invoke2(_bstr_t(_T("FilePrintSetup")), &curActivePrinter, &_variant_t((short)0), NULL);

	if (FAILED(m_hr))
		return -1;

	return 1;
}



const bool CMsWordAccessor::PrintToImage(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName)
{
	if (NULL == m_pApp)
	{
		m_hr = E_FAIL;
		return false;
	}

	if (NULL == lpctszOutFileName || 0 >= _tcslen(lpctszOutFileName))
	{
		m_hr = E_INVALIDARG;
		return false;
	}



	// Active Printer를 변경할 때, Default로 바꾸지 않으면 User Prompt가 발생하여
	// 어쩔수 없이 기본 프린터 이름을 받아놨다가 다시 바꾸는 방식으로 변경했다.
	bool bGetActivePrinter = false;

	_variant_t curActivePrinter;
	if (SUCCEEDED(m_pApp.GetPropertyByName(_bstr_t(_T("ActivePrinter")), &curActivePrinter)))
	{
		if (curActivePrinter != _variant_t(lpctszPrintName))
			bGetActivePrinter = true;
	}


	_variant_t pOutVal;
	m_hr = m_pApp.GetPropertyByName(_bstr_t(_T("WordBasic")), &pOutVal);

	if (FAILED(m_hr))
		return false;

	CComPtr<IDispatch> pWordBasic = pOutVal.pdispVal;

	// Print Setup, Print Dialog Appear, if second param is 1
	m_hr = pWordBasic.Invoke2(_bstr_t(_T("FilePrintSetup")), &_variant_t(lpctszPrintName), &_variant_t((short)0), NULL);

	if (FAILED(m_hr))
		return false;


	VARIANT vTrue;
	vTrue.vt = VT_BOOL;
	vTrue.boolVal = TRUE;

	VARIANT vFalse;
	vFalse.vt = VT_BOOL;
	vFalse.boolVal = FALSE;

	VARIANT vOptional;
	vOptional.vt = VT_ERROR;
	vOptional.scode = DISP_E_PARAMNOTFOUND;

	m_hr = Do_OLEMethod(DISPATCH_METHOD, NULL, m_pApp, L"PrintOut", 19,
		vOptional,						// 19 PrintZoomPaperHeight
		vOptional,						// 18 PrintZoomPaperWidth
		vOptional,						// 17 PrintZoomRow
		vOptional,						// 16 PrintZoomColumn
		vOptional,						// 15 ManualDuplexPrint
		vOptional,						// 14 ActivePrinterMacGX
		vOptional,						// 13 FileName ( The path and file name of the document to be printed. If this argument is omitted, Word prints the active document. (Available only with the Application object.) )
		vOptional,						// 12 Collate
		vTrue,							// 11 PrintToFile
		vOptional,						// 10 PageType
		vOptional,						//  9 Pages
		vOptional,						//  8 Copies
		vOptional,						//  7 Item
		vOptional,						//  6 To
		vOptional,						//  5 From
		_variant_t(lpctszOutFileName),	//	4 OutputFileName
		vOptional,						//  3 Range
		vFalse,							//  2 Append
		vFalse);						//  1 Background

	if (true == bGetActivePrinter)
		pWordBasic.Invoke2(_bstr_t(_T("FilePrintSetup")), &curActivePrinter, &_variant_t((short)0), NULL);

	if (FAILED(m_hr))
		return false;

	return true;
}

const bool CMsWordAccessor::SaveAs(LPCTSTR lpctszOutFileName, int nFormat)
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


	//COleVariant FileName(L"C:\\test\\testok.docx");
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




	//m_hr=Do_OLEMethod(  DISPATCH_METHOD, &result, m_pActiveDocument, L"SaveAs2",1,
	//	FileName);
	m_hr = Do_OLEMethod(DISPATCH_METHOD, &pOutVal, m_pActiveDocument, L"SaveAs", 2,
		//vFalse,							// 13 OpenAndRepair
		//vTrue,							// 12 document Visible ( if value is vFalse, can't print document )
		//vOptional,						// 11 Encoding
		//vOptional,						// 10 Format
		//vOptional,						//  9 WritePasswordTemplate
		//vOptional,						//  8 WritePasswordDocument
		//vOptional,						//  7 Revert
		//vOptional,						//  6 PasswordTemplate
		//vOptional,						//  5 PasswordDocument
		//vFalse,							//  4 AddToRecentFiles
		//vFalse,							//  3 ReadOnly
		//vOptional,						//  2 ConfirmConversions
		COleVariant((short)nFormat),
		FileName);						//  1 FileName


	if (FAILED(m_hr)) {
		return false;
	}
	else {
	}



	return true;
}


void CMsWordAccessor::ClearVariant(VARIANTARG *pvarg)
{
	pvarg->vt = VT_EMPTY;
	pvarg->wReserved1 = 0;
	pvarg->wReserved2 = 0;
	pvarg->wReserved3 = 0;
	pvarg->lVal = 0;

}

void CMsWordAccessor::ClearAllArgs()
{
	int i;

	for (i = 0; i < m_iArgCount; i++)
	{
		if (m_awFlags[i] & DISPARG_NOFREEVARIANT)
			// free the variant's contents based on type
			ClearVariant(&m_aVargs[i]);
		else
			ReleaseVariant(&m_aVargs[i]);
	}

	m_iArgCount = 0;
	m_iNamedArgCount = 0;

}


void CMsWordAccessor::ReleaseVariant(VARIANTARG *pvarg)
{
	VARTYPE vt;
	VARIANTARG *pvargArray;
	long lLBound, lUBound, l;

	vt = pvarg->vt & 0xfff;		// mask off flags

								// check if an array.  If so, free its contents, then the array itself.
	if (V_ISARRAY(pvarg))
	{
		// variant arrays are all this routine currently knows about.  Since a
		// variant can contain anything (even other arrays), call ourselves
		// recursively.
		if (vt == VT_VARIANT)
		{
			SafeArrayGetLBound(pvarg->parray, 1, &lLBound);
			SafeArrayGetUBound(pvarg->parray, 1, &lUBound);

			if (lUBound > lLBound)
			{
				lUBound -= lLBound;

				SafeArrayAccessData(pvarg->parray, (void**)&pvargArray);

				for (l = 0; l < lUBound; l++)
				{
					ReleaseVariant(pvargArray);
					pvargArray++;
				}

				SafeArrayUnaccessData(pvarg->parray);
			}
		}
		else
		{
		}

		// Free the array itself.
		SafeArrayDestroy(pvarg->parray);
	}
	else
	{
		switch (vt)
		{
		case VT_DISPATCH:
			//(*(pvarg->pdispVal->lpVtbl->Release))(pvarg->pdispVal);
			pvarg->pdispVal->Release();
			break;

		case VT_BSTR:
			SysFreeString(pvarg->bstrVal);
			break;

		case VT_I2:
		case VT_BOOL:
		case VT_R8:
		case VT_ERROR:		// to avoid erroring on an error return from Excel
							// no work for these types
			break;

		default:
			break;
		}
	}

	ClearVariant(pvarg);

}


void CMsWordAccessor::AddArgumentCommon(LPOLESTR lpszArgName, WORD wFlags, VARTYPE vt)
{
	ClearVariant(&m_aVargs[m_iArgCount]);

	m_aVargs[m_iArgCount].vt = vt;
	m_awFlags[m_iArgCount] = wFlags;

	if (lpszArgName != NULL)
	{
		m_alpszArgNames[m_iNamedArgCount + 1] = lpszArgName;
		m_iNamedArgCount++;
	}
}


BOOL CMsWordAccessor::AddArgumentDispatch(LPOLESTR lpszArgName, WORD wFlags, IDispatch * pdisp)
{
	AddArgumentCommon(lpszArgName, wFlags, VT_DISPATCH);
	m_aVargs[m_iArgCount++].pdispVal = pdisp;
	return TRUE;
}


BOOL CMsWordAccessor::AddArgumentInt2(LPOLESTR lpszArgName, WORD wFlags, int i)
{
	AddArgumentCommon(lpszArgName, wFlags, VT_I2);
	m_aVargs[m_iArgCount++].iVal = i;
	return TRUE;
}


BOOL CMsWordAccessor::WordInvoke(IDispatch *pdisp, LPOLESTR szMember, VARIANTARG * pvargReturn,
	WORD wInvokeAction, WORD wFlags)
{
	HRESULT hr;
	DISPPARAMS dispparams;
	unsigned int uiArgErr;
	EXCEPINFO excep;

	// Get the IDs for the member and its arguments.  GetIDsOfNames expects the
	// member name as the first name, followed by argument names (if any).
	m_alpszArgNames[0] = szMember;
	hr = pdisp->GetIDsOfNames(IID_NULL, m_alpszArgNames,
		1 + m_iNamedArgCount, LOCALE_SYSTEM_DEFAULT, m_aDispIds);
	if (FAILED(hr))
	{
		return FALSE;
	}

	if (pvargReturn != NULL)
		ClearVariant(pvargReturn);

	// if doing a property put(ref), we need to adjust the first argument to have a
	// named arg of DISPID_PROPERTYPUT.
	if (wInvokeAction & (DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF))
	{
		m_iNamedArgCount = 1;
		m_aDispIds[1] = DISPID_PROPERTYPUT;
		pvargReturn = NULL;
	}

	dispparams.rgdispidNamedArgs = m_aDispIds + 1;
	dispparams.rgvarg = m_aVargs;
	dispparams.cArgs = m_iArgCount;
	dispparams.cNamedArgs = m_iNamedArgCount;

	excep.pfnDeferredFillIn = NULL;

	hr = pdisp->Invoke(m_aDispIds[0], IID_NULL, LOCALE_SYSTEM_DEFAULT,
		wInvokeAction, &dispparams, pvargReturn, &excep, &uiArgErr);

	if (wFlags & DISP_FREEARGS)
		ClearAllArgs();

	if (FAILED(hr))
	{

		// free exception structure information
		SysFreeString(excep.bstrSource);
		SysFreeString(excep.bstrDescription);
		SysFreeString(excep.bstrHelpFile);

		return FALSE;
	}
	return TRUE;
}
