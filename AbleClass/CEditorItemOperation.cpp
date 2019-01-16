#include "stdafx.h"
#include "CEditorItemOperation.h"


CEditorItemOperation::CEditorItemOperation() :
	m_drawingRect(D2D1::RectF(0,0,0,0)),
	m_strFile(L""),
	m_type(EDITOR_NONE),
	m_nid(0),
	m_selected(FALSE),
	m_visible(SW_SHOW),
	m_colorBg(0),
	m_colorBorder(0)
{
}


CEditorItemOperation::~CEditorItemOperation()
{
}

HRESULT CEditorItemOperation::DiscardResources()
{
	if (m_bitmap) m_bitmap = nullptr;
	return S_OK;
}



HRESULT CEditorItemOperation::SetSelected(BOOL bSelect)
{
	HRESULT hr = S_OK;
	m_selected = bSelect;
	return hr;
}

BOOL CEditorItemOperation::GetSelected()
{
	return m_selected;
}


HRESULT CEditorItemOperation::SetDataId(UINT id)
{
	HRESULT hr = S_OK;
	m_nid = id;
	return hr;
}
UINT CEditorItemOperation::GetDataId()
{
	return m_nid;
}


HRESULT CEditorItemOperation::SetType(eItemType type)
{
	HRESULT hr = S_OK;
	m_type = type;
	return hr;
}

eItemType CEditorItemOperation::GetType()
{
	return m_type;
}


HRESULT CEditorItemOperation::SetRect(__in ID2D1RenderTarget* renderTarget, const D2D1_RECT_F& rect)
{
	HRESULT hr = S_OK;
	m_drawingRect = rect;
	return hr;
}

HRESULT CEditorItemOperation::SetBitmap(__in ID2D1RenderTarget* renderTarget)
{
	HRESULT hr = S_OK;
	hr = Direct2DUtility::LoadBitmapFromFile(renderTarget, L"./images/edit_menu/clip_video_o.png", 130, 130, &m_bitmap);
	return hr;
}

HRESULT CEditorItemOperation::SetFile(__in std::wstring strFile) {
	HRESULT hr = S_OK;
	m_strFile = strFile;
	return hr;
}

HRESULT CEditorItemOperation::SetColor(__in COLORREF bg, __in COLORREF border) {
	HRESULT hr = S_OK;
	m_colorBg = bg;
	m_colorBorder = border;
	return hr;
}

std::wstring CEditorItemOperation::GetFile() {
	return m_strFile;
}


HRESULT CEditorItemOperation::SetVisible(UINT visible) {
	HRESULT hr = S_OK;
	m_visible = visible;
	return hr;
}


HRESULT CEditorItemOperation::DrawToRenderTarget(__in ID2D1RenderTarget* renderTarget/*, D2D1_RECT_F imageRect*/, __in float alpha)
{
	HRESULT hr = S_OK;
	ComPtr<ID2D1Factory> factory;
	renderTarget->GetFactory(&factory);


	if (m_visible == SW_HIDE)
		return hr;

	if (m_type == EDITOR_VIDEO || m_type == EDITOR_AUDIO) {
		if (m_bitmap) {
			renderTarget->DrawBitmap(m_bitmap, m_drawingRect, alpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
		}

		if (m_selected) {
			ComPtr<ID2D1SolidColorBrush> brushSelect;

			renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::YellowGreen), &brushSelect);
			renderTarget->DrawRectangle(m_drawingRect, brushSelect, 4.f);
			brushSelect = nullptr;
		}

	}
	else if (m_type == EDITOR_BLANK) {
		ComPtr<ID2D1SolidColorBrush> brush;
		renderTarget->CreateSolidColorBrush(D2D1::ColorF(RGB(GetBValue(m_colorBg), GetGValue(m_colorBg), GetRValue(m_colorBg)), alpha), &brush);
		renderTarget->FillRectangle(m_drawingRect, brush);
		brush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
		brush->SetOpacity(alpha);

		ComPtr<IDWriteFactory> writeFactory;
		Direct2DUtility::GetDWriteFactory(&writeFactory);

		float fontSize = /*Direct2DUtility::ScaleValueForNoDPI*/(m_drawingRect.bottom - m_drawingRect.top) * 0.7;

		ComPtr<IDWriteTextFormat> textFormat;
		hr = writeFactory->CreateTextFormat(
			L"Tahoma",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			L"", //locale
			&textFormat
		);
		// Center the text horizontally and vertically.
		textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		textFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_DEFAULT, 0, 0);
		textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
		
		renderTarget->DrawText(L"(\n", 2, textFormat, m_drawingRect, brush);
		textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		renderTarget->DrawText(L")", 2, textFormat, m_drawingRect, brush);

	
		brush = nullptr;


		if (m_selected) {
			ComPtr<ID2D1SolidColorBrush> brushSelect;
			renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::YellowGreen), &brushSelect);
			renderTarget->DrawRectangle(m_drawingRect, brushSelect, 4.f);
			brushSelect = nullptr;
		}

	}

	
	return hr;
}


BOOL CEditorItemOperation::IsContaion(__in D2D1_POINT_2F point) {
	BOOL contains = FALSE;
	if (m_drawingRect.left <= point.x && m_drawingRect.right >= point.x && m_drawingRect.top <= point.y && m_drawingRect.bottom >= point.y) {
		contains = TRUE;
	}


	return contains;
}