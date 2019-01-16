#pragma once

class CDocumentHelper
{
public:
	CDocumentHelper(void)
	{
		HRESULT hr = ::CoInitialize(NULL);
		if (FAILED(hr))
			throw hr;
	}

	~CDocumentHelper(void)
	{
		::CoUninitialize();
	}


public:
	HRESULT getActiveObject(const wchar_t* lpcwszApp, IDispatch** ppiDisp)
	{
		if (NULL == lpcwszApp || 0 <= wcslen(lpcwszApp) || NULL == ppiDisp)
			return E_INVALIDARG;

		CLSID clsid;
		HRESULT hr = CLSIDFromProgID(lpcwszApp, &clsid);

		if (FAILED(hr))
			return hr;

		IUnknown* piUnk = NULL;
		*ppiDisp = NULL;

		hr = GetActiveObject(clsid, NULL, (IUnknown**)&piUnk);
		if (FAILED(hr))
			return hr;

		hr = piUnk->QueryInterface(IID_IDispatch, (void**)ppiDisp);
		piUnk->Release();

		if (FAILED(hr))
			return hr;

		return S_OK;
	}

	HRESULT Do_OLEMethod(const int nType, VARIANT* pvResult, IDispatch* piDisp, LPOLESTR lpstrName, int nArgs...)
	{
		if (NULL == piDisp || 0 > nArgs)
			return E_INVALIDARG;

		va_list argList;
		va_start(argList, nArgs);

		DISPPARAMS dp = { NULL, NULL, 0, 0 };
		DISPID dispidNamed = DISPID_PROPERTYPUT;
		DISPID dispID;

		HRESULT hr = piDisp->GetIDsOfNames(IID_NULL, &lpstrName, 1, LOCALE_USER_DEFAULT, &dispID);
		if (FAILED(hr))
			return hr;

		VARIANT *pArgs = NULL;
		try
		{
			pArgs = new VARIANT[nArgs + 1];
		}
		catch (std::bad_alloc)
		{
			return E_OUTOFMEMORY;
		}

		for (int i = 0; i < nArgs; i++)
			pArgs[i] = va_arg(argList, VARIANT);

		dp.cArgs = nArgs;
		dp.rgvarg = pArgs;

		if (nType == DISPATCH_PROPERTYPUT)
		{
			dp.cNamedArgs = 1;
			dp.rgdispidNamedArgs = &dispidNamed;
		}

		hr = piDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, nType, &dp, pvResult, NULL, NULL);
		if (FAILED(hr))
			return hr;

		va_end(argList);

		delete[] pArgs;
		return hr;
	}

};


class CDocumentAccessor : public CDocumentHelper
{
public:
	CDocumentAccessor();
	virtual ~CDocumentAccessor();

	virtual const bool IsEnableGetActiveObject() const;
	virtual void EnableGetActiveObject(const bool bEnable = true);
	virtual const bool Initialize() = 0;
	virtual const bool Open(LPCTSTR lpctszFileName, const bool bShowWindow = false) = 0;
	virtual const bool Close() = 0;
	virtual const int Print(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName) = 0;
	virtual const bool PrintToImage(LPCTSTR lpctszPrintName, LPCTSTR lpctszOutFileName) = 0;
	virtual const bool SaveAs(LPCTSTR lpctszOutFileName, int nFormat) = 0;


	HRESULT GetLastHRESULT() const;
	HRESULT Init(const wchar_t* lpcwszApp, const wchar_t* lpcwszDocumentName);


protected:
	CComPtr<IDispatch>	m_pApp;
	CComPtr<IDispatch>	m_pDocument;
	CComPtr<IDispatch>	m_pActiveDocument;
	CComPtr<IDispatch>	m_pPageSetup;
	HRESULT				m_hr;
	bool				m_bGetActiveObject;
};

