#pragma once

#include "ImageOperation.h"



[uuid("17A76C82-5618-451F-A3D3-CEED1D503749")]
__interface IEditorItemOperation : public IUnknown
{
	HRESULT __stdcall SetVisible(UINT visible);
	HRESULT __stdcall SetSelected(BOOL bSelect);
	BOOL	__stdcall GetSelected();
	HRESULT __stdcall SetDataId(UINT id);
	HRESULT __stdcall SetType(eItemType type);
	eItemType __stdcall GetType();
	
	HRESULT __stdcall SetRect(__in ID2D1RenderTarget* renderTarget, __in const D2D1_RECT_F& rect);
	HRESULT __stdcall SetBitmap(__in ID2D1RenderTarget* renderTarget);
	HRESULT __stdcall SetFile(__in std::wstring strFile);
	HRESULT __stdcall SetColor(__in COLORREF bg, __in COLORREF border);
	UINT	__stdcall GetDataId();
	std::wstring __stdcall GetFile();
	HRESULT __stdcall DrawToRenderTarget(__in ID2D1RenderTarget* renderTarget, __in float alpha=1.f);
	HRESULT __stdcall DiscardResources();
	BOOL	__stdcall IsContaion(__in D2D1_POINT_2F point);

};


class CEditorItemOperation : public IImageOperation, public IEditorItemOperation
{
public:
	CEditorItemOperation();
	virtual ~CEditorItemOperation();

	// Interface helper
	bool QueryInterfaceHelper(const IID &iid, void **object)
	{
		return CastHelper<IImageOperation>::CastTo(iid, this, object) ||
			CastHelper<IEditorItemOperation>::CastTo(iid, this, object);
	}

	UINT					m_nid;
	ComPtr<ID2D1Bitmap>		m_bitmap;
	D2D1_RECT_F				m_drawingRect;
	std::wstring			m_strFile;
	eItemType				m_type;
	BOOL					m_selected;
	UINT					m_visible;
	COLORREF				m_colorBg;
	COLORREF				m_colorBorder;


	HRESULT __stdcall SetVisible(UINT visible);
	HRESULT __stdcall SetSelected(BOOL bSelect);
	BOOL	__stdcall GetSelected();
	HRESULT __stdcall SetDataId(UINT id);
	HRESULT __stdcall SetType(eItemType type);
	eItemType __stdcall GetType();
	HRESULT __stdcall SetRect(__in ID2D1RenderTarget* renderTarget, __in const D2D1_RECT_F& rect);
	HRESULT __stdcall SetBitmap(__in ID2D1RenderTarget* renderTarget);
	HRESULT __stdcall SetFile(__in std::wstring strFile);
	HRESULT __stdcall SetColor(__in COLORREF bg, __in COLORREF border);
	UINT	__stdcall GetDataId();
	std::wstring __stdcall GetFile();
	HRESULT __stdcall DrawToRenderTarget(__in ID2D1RenderTarget* renderTarget/*, D2D1_RECT_F imageRect*/, __in float alpha=1.f);
	HRESULT __stdcall DiscardResources();
	BOOL	__stdcall IsContaion(__in D2D1_POINT_2F point);
};

