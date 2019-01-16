#include "stdafx.h"
#include "CDocumentAccessor.h"


CDocumentAccessor::CDocumentAccessor() :
	m_pApp(NULL),
	m_pDocument(NULL),
	m_pActiveDocument(NULL),
	m_bGetActiveObject(false),
	m_pPageSetup(NULL),
	m_hr(S_OK)
{
}


CDocumentAccessor::~CDocumentAccessor()
{
}


const bool CDocumentAccessor::IsEnableGetActiveObject() const
{
	return m_bGetActiveObject;
}

void CDocumentAccessor::EnableGetActiveObject(const bool bEnable /*= true*/)
{
	m_bGetActiveObject = bEnable;
}

HRESULT CDocumentAccessor::GetLastHRESULT() const
{
	return m_hr;
}



HRESULT CDocumentAccessor::Init(const wchar_t* lpcwszApp, const wchar_t* lpcwszDocumentName)
{
	if (NULL == lpcwszApp || 0 >= wcslen(lpcwszApp) || NULL == lpcwszDocumentName || 0 >= wcslen(lpcwszDocumentName))
		return E_INVALIDARG;

	if (true == m_bGetActiveObject)
	{
		//LOGER_WRITE(eLog_INFO, _T("Get Active Object"));
		HRESULT hr = getActiveObject(lpcwszApp, &m_pApp);
		if (FAILED(hr))
		{
			m_hr = hr;
			return m_hr;
		}
	}
	else
	{
		CLSID clsid;
		m_hr = CLSIDFromProgID(lpcwszApp, &clsid);

		if (SUCCEEDED(m_hr))
		{
			//LOGER_WRITE(eLog_INFO, _T("Create Instance"));
			m_hr = m_pApp.CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER);
			if (FAILED(m_hr))
				m_pApp = NULL;
		}
	}

	// get document
	if (SUCCEEDED(m_hr))
	{
		//LOGER_WRITE(eLog_INFO, _T("Get Document"));
		VARIANT result;
		VariantInit(&result);
		m_hr = Do_OLEMethod(DISPATCH_PROPERTYGET, &result, m_pApp, _bstr_t(lpcwszDocumentName), 0);

		if (FAILED(m_hr))
			return m_hr;

		m_pDocument = result.pdispVal;

		VariantClear(&result);
	}

	//LOGER_WRITE(eLog_INFO, _T("End Accessor Init"));

	return m_hr;
}