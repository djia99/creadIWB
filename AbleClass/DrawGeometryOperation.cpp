//===================================================================================
// Copyright (c) Microsoft Corporation.  All rights reserved.                        
//                                                                                   
// THIS CODE AND INFORMATION IS PROVIDED 'AS IS' WITHOUT WARRANTY                    
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT                       
// LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND                          
// FITNESS FOR A PARTICULAR PURPOSE.                                                 
//===================================================================================


#include "StdAfx.h"
#include "DrawGeometryOperation.h"
#include "direct2dutility.h"
#include <D2d1_3.h>

#define INK_MODE


//#define PRESSURE_TYPE 4096
#define PRESSURE_TYPE 2048



DrawGeometryOperation::DrawGeometryOperation() : m_strokeSize(1), m_brushAlpha(1.0f), m_bIsClearOperation(false), /*m_cur(0), */m_bPressure(TRUE), m_penStyleValueH(1.), m_penStyleValueV(1.), m_id(0), m_lastDrawedPointIndex(0), m_drawingRect(D2D1::RectF(9999,9999,-9999, -9999))
{
	m_bOn = TRUE;
	//memset(m_pointBuffer, 0, sizeof(m_pointBuffer));
	//m_pathGeometry = nullptr;
	m_geometry = nullptr;
	//m_geometryOri = nullptr;
	//m_pointsPtr = 0;

}

DrawGeometryOperation::~DrawGeometryOperation()
{
}


void DrawGeometryOperation::SetPenStyleValue(PENTIP_STYLE ps, bool pressure) {
	if (pressure == true) {
		m_bPressure = TRUE;
		m_penStyleValueH = 1.;
		m_penStyleValueV = 1.;
	}
	else {
		m_bPressure = FALSE;
		if (ps == PS_NORMAL) {
			m_penStyleValueH = 1.;
			m_penStyleValueV = 1.;
		}
		else if (ps == PS_H_EXPAND) {
			m_penStyleValueH = 1.6;
			m_penStyleValueV = 1.;
		}
		else if (ps == PS_V_EXPAND) {
			m_penStyleValueH = 1.;
			m_penStyleValueV = 1.6;
		}
	}
}




//HRESULT DrawGeometryOperation::DrawToRenderTargetInk(__in ID2D1RenderTarget* renderTarget, __in float alpha)
//{
//	ComPtr<ID2D1Factory> factory;
//	renderTarget->GetFactory(&factory);
//	CComQIPtr<ID2D1DeviceContext2> spDeviceContext = renderTarget;
//
//	HRESULT hr = S_OK;
//
//	if (!m_brush && SUCCEEDED(hr))
//	{
//		m_brushColor.a = m_brushAlpha;
//		hr = renderTarget->CreateSolidColorBrush(m_brushColor, &m_brush);
//	}
//
//	if (!m_inkStyle) {
//		spDeviceContext->GetTransform(&m_transMatrix);
//		D2D1_MATRIX_3X2_F nibTransform = D2D1::Matrix3x2F::Scale(m_penStyleValueH, m_penStyleValueV);
//		hr = spDeviceContext->CreateInkStyle(
//			D2D1::InkStyleProperties(
//				D2D1_INK_NIB_SHAPE_ROUND,
//				nibTransform), &m_inkStyle);
//	}
//
//	if (m_ink) {
//		spDeviceContext->DrawInk(m_ink, m_brush, m_inkStyle);
//	}
//
//	return hr;
//}


//HRESULT DrawGeometryOperation::AppendPointInk(__in ID2D1RenderTarget* renderTarget, __in D2D1_POINT_2F point, LONG pressure)
//{
//	HRESULT hr = S_OK;
//	float presValue;
//	if (pressure < 1) {
//		presValue = m_strokeSize * 0.5 * -1;
//	}
//	//else if (pressure == 2048) {
//	else if (pressure == PRESSURE_TYPE / 2) {
//		presValue = 0;
//	}
//	else {
//		//presValue = m_strokeSize * (pressure - 2048) / 4096;
//		presValue = m_strokeSize * (pressure - PRESSURE_TYPE / 2) / PRESSURE_TYPE;
//	}
//
//	if (m_bPressure == FALSE) {
//		presValue = 0;
//	}
//	D2D1_INK_POINT inkPoint;
//	inkPoint.x = point.x;
//	inkPoint.y = point.y;
//	inkPoint.radius = m_strokeSize + presValue/*+ pressure * 0.001*/;
//	m_inkPoints.push_back(inkPoint);
//
//	if (m_ink == nullptr) {
//		CComQIPtr<ID2D1DeviceContext2> spDeviceContext = renderTarget;
//		hr = spDeviceContext->CreateInk(inkPoint, &m_ink);
//	}
//
//	if (m_ink) {
//		D2D1_INK_BEZIER_SEGMENT	segment;
//		segment.point1 = inkPoint;
//		segment.point2 = inkPoint;
//		segment.point3 = inkPoint;
//		m_ink->AddSegments(&segment, 1);
//	}
//
//	return hr;
//}



HRESULT DrawGeometryOperation::DrawToRenderTarget(__in ID2D1RenderTarget* renderTarget/*, D2D1_RECT_F imageRect*/, __in float alpha)
{
	DynamicDrawToRenderTarget(renderTarget, alpha);
	return S_OK;
	//TRACE(L"\n>>> DrawTo Start <<<<<<<\n");

	ComPtr<ID2D1Factory> factory;
	renderTarget->GetFactory(&factory);
	//CComQIPtr<ID2D1DeviceContext2> spDeviceContext = renderTarget;

	//float strokeSize = Direct2DUtility::ScaleValueForCurrentDPI(m_strokeSize) - Direct2DUtility::ScaleValueForCurrentDPI(m_strokeSize) * 0.24;
	//float strokeSize = Direct2DUtility::ScaleValueForCurrentDPI(m_strokeSize) * 2;// +Direct2DUtility::ScaleValueForCurrentDPI(m_strokeSize) * Direct2DUtility::ScaleValueForCurrentDPI(0.1);
	float strokeSize = Direct2DUtility::ScaleValueForCurrentDPI(m_strokeSize) * 2.0;// 

	HRESULT hr = S_OK;

	//if (!m_inkStyle) {
	//	spDeviceContext->GetTransform(&m_transMatrix);
	//	D2D1_MATRIX_3X2_F nibTransform = D2D1::Matrix3x2F::Scale(m_penStyleValueH, m_penStyleValueV);
	//	hr = spDeviceContext->CreateInkStyle(
	//		D2D1::InkStyleProperties(
	//			D2D1_INK_NIB_SHAPE_ROUND,
	//			nibTransform), &m_inkStyle);
	//}


	/*if (!m_brush && SUCCEEDED(hr))
	{
		m_brushColor.a = m_brushAlpha;
		hr = renderTarget->CreateSolidColorBrush(m_brushColor, &m_brush);
	}*/


	//m_ink = nullptr;


	//if (m_ink == nullptr) {
	//	hr = spDeviceContext->CreateInk(&m_inkPoints[0], &m_ink);
	//}
	//if (m_ink) {

	//	//D2D1_INK_BEZIER_SEGMENT
	//	//m_ink->AddSegments(

	//	std::vector<D2D1_INK_POINT> pointTemp = m_inkPoints;
	//	for (int i = 0; i < 3; ++i) {
	//		pointTemp = ComputeSmooth(pointTemp);
	//	}

	//	D2D1_INK_BEZIER_SEGMENT inkB;
	//	std::vector<D2D1_INK_BEZIER_SEGMENT> inkBs;
	//	for (int item = 0; item < pointTemp.size()-2; ) {
	//		inkB.point1 = pointTemp[item];
	//		inkB.point2 = pointTemp[item + 1];
	//		inkB.point3 = pointTemp[item + 2];
	//		inkBs.push_back(inkB);
	//		item = item + 3;
	//	}

	//	if (inkBs.size() > 0) {
	//		m_ink->SetStartPoint(pointTemp[0]);
	//		m_ink->AddSegments(&inkBs[0], inkBs.size());
	//		spDeviceContext->DrawInk(m_ink, m_brush, m_inkStyle);

	//	}
	//}



	if (!m_strokeStyle)
	{
		hr = factory->CreateStrokeStyle(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_ROUND,
				D2D1_CAP_STYLE_ROUND,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_ROUND),
			nullptr, 0, &m_strokeStyle);
	}

	if (!m_brush && SUCCEEDED(hr))
	{
		m_brushColor.a = m_brushAlpha;
		hr = renderTarget->CreateSolidColorBrush(m_brushColor, &m_brush);
	}

	std::vector<D2D1_POINT_2F> pointTemp = m_points;

	if (SUCCEEDED(hr))
	{



		ComPtr<ID2D1PathGeometry>	pathGeometry;
		CComPtr<ID2D1GeometrySink>	spSink;
		HRESULT hr = factory->CreatePathGeometry(&pathGeometry);
		if (FAILED(hr))
			return hr;
		

		hr = pathGeometry->Open(&spSink);
		if (SUCCEEDED(hr))
		{
			bool bStart = false;

			if (m_points.size() > 1) {
				//for (int i = 0; i < 1; ++i) {
				//	pointTemp = ComputeSmoothCenter(pointTemp);
				//}
				for (int i = 0; i < 4; ++i) {
					pointTemp = ComputeSmooth(pointTemp);
				}

				spSink->SetSegmentFlags(D2D1_PATH_SEGMENT_FORCE_ROUND_LINE_JOIN);
				spSink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);
				spSink->BeginFigure(pointTemp[0], D2D1_FIGURE_BEGIN_HOLLOW);
				spSink->AddLines(&pointTemp[0], pointTemp.size());

				spSink->EndFigure(D2D1_FIGURE_END_OPEN);

				if (m_geometry) {
					m_geometry = nullptr;
				}
				m_geometry = pathGeometry;
			}
			spSink->Close();
		}

			

		if (m_geometry != nullptr) {
			renderTarget->GetTransform(&m_transMatrix);
			renderTarget->DrawGeometry(m_geometry, m_brush, strokeSize, m_strokeStyle);
		}
	}



	// test 용
	// 포인터 점 출력
	//D2D1_COLOR_F color = m_brush->GetColor();
	//if (m_bOn^=1) {
	//	m_brush->SetColor(D2D1::ColorF(D2D1::ColorF::GreenYellow));
	//	D2D1_ELLIPSE ellipse;
	//	for (int i = 0; i < pointTemp.size(); i++) {
	//		ellipse = D2D1::Ellipse(D2D1::Point2F(pointTemp[i].x, pointTemp[i].y), 1, 1);
	//		renderTarget->DrawEllipse(ellipse, m_brush);
	//		//renderTarget->FillEllipse(ellipse, m_brush);
	//	}

	//	m_brush->SetColor(D2D1::ColorF(D2D1::ColorF::Blue));

	//	for (int i = 0; i < m_points.size(); i++) {
	//		ellipse = D2D1::Ellipse(D2D1::Point2F(m_points[i].x, m_points[i].y), 0.5, 0.5);
	//		renderTarget->FillEllipse(ellipse, m_brush);
	//	}
	//	m_brush->SetColor(color);
	//}

	//TRACE(L"\n>>> DrawTo End <<<<<<<\n");

	return hr;
}



HRESULT DrawGeometryOperation::AppendPoint(__in ID2D1RenderTarget* renderTarget, __in D2D1_POINT_2F point, LONG pressure)
{
// ink mode
#ifdef INK_MODE
	HRESULT hr = S_OK;
	float presValue = m_strokeSize;


	D2D1_INK_POINT inkPoint3;
	inkPoint3.x = point.x;
	inkPoint3.y = point.y;
	inkPoint3.radius = m_strokeSize;// +presValue;
	m_inkPoints.push_back(inkPoint3);

	//TRACE(L"\n>>> append count (%d) <<<<<\n", m_inkPoints.size());
	D2D1_INK_BEZIER_SEGMENT LastSegment;
	D2D1_INK_BEZIER_SEGMENT segment;

	segment.point1 = inkPoint3;
	segment.point2 = inkPoint3;
	segment.point3 = inkPoint3;

	if (m_inkSegments.size() < 1) {
		LastSegment = segment;
	}
	else {
		LastSegment = m_inkSegments[m_inkSegments.size() - 1];
		m_inkSegments.pop_back();
	}

	LastSegment = ComputeSmoothCenter(LastSegment, segment);

	m_inkSegments.push_back(LastSegment);
	m_inkSegments.push_back(segment);

	if (m_ink == nullptr) {
		CComQIPtr<ID2D1DeviceContext2> spDeviceContext = renderTarget;
		hr = spDeviceContext->CreateInk(inkPoint3, &m_ink);
	}

	if (m_ink) {
		m_ink->RemoveSegmentsAtEnd(m_ink->GetSegmentCount());
		m_ink->AddSegments(&m_inkSegments[0], m_inkSegments.size());
	}

#else
	HRESULT hr = S_OK;
	m_points.push_back(point);

#endif


	return hr;
}

HRESULT DrawGeometryOperation::EndGeometry(__in ID2D1RenderTarget* renderTarget, __in D2D1_POINT_2F point, LONG pressure)
{
	return S_OK;// UpdateGeometry(renderTarget);
}



void DrawGeometryOperation::SetPenId(__in UINT id)
{
	m_id = id;

	return;
}

HRESULT DrawGeometryOperation::SetBrushColor(__in D2D1_COLOR_F brushColor)
{
    m_brush = nullptr;
    m_brushColor = brushColor;

    return S_OK;
}

HRESULT DrawGeometryOperation::SetStrokeSize(__in float strokeSize)
{
    m_strokeSize = strokeSize;
    return S_OK;
}

HRESULT DrawGeometryOperation::SetBrushAlpha(__in float alpha)
{
	m_brushAlpha = alpha;
	return S_OK;
}

HRESULT DrawGeometryOperation::SetClearOperation(__in bool bClear) {
	m_bIsClearOperation = bClear;
	return S_OK;
}



bool DrawGeometryOperation::pointOnLine(D2D1_POINT_2F point, D2D1_POINT_2F linePt1, D2D1_POINT_2F linePt2) {


	float widthLine = sqrt((linePt2.x - linePt1.x)*(linePt2.x - linePt1.x) + (linePt2.y - linePt1.y)*(linePt2.y - linePt1.y));
	float widthStartToPoint = sqrt((point.x - linePt1.x)*(point.x - linePt1.x) + (point.y - linePt1.y)*(point.y - linePt1.y));
	float widthPointToEnd = sqrt((linePt2.x - point.x)*(linePt2.x - point.x) + (linePt2.y - point.y)*(linePt2.y - point.y));
	float distance = widthLine - (widthStartToPoint + widthPointToEnd);

	//TRACE(L"\n>>> (%.02f, %.02f) (%.02f, %.02f) (%.02f, %.02f)  <<<<\m", point.x, point.y, linePt1.x, linePt1.y, linePt2.x, linePt2.y);

	if (distance < 2 && distance > -2) {
		return true;
	}
	else {
		return false;
	}
}


BOOL DrawGeometryOperation::IsContaion(__in D2D1_POINT_2F point) {
	BOOL contains = FALSE;

// ink mode
#ifdef INK_MODE
	if (m_ink && m_inkStyle) {
		for (int i = 0; i < m_inkPoints.size()-1; i++) {
			D2D1_POINT_2F pt1 = D2D1::Point2F(m_inkPoints[i].x, m_inkPoints[i].y);
			D2D1_POINT_2F pt2 = D2D1::Point2F(m_inkPoints[i+1].x, m_inkPoints[i+1].y);

			if (pointOnLine(point, pt1, pt2) == true) {
				contains = TRUE;
				break;
			}
		}
	}
	return contains;

#else
	if (m_geometry) {
		m_geometry->StrokeContainsPoint(point, m_strokeSize, m_strokeStyle, m_transMatrix, &contains);
}
	else {
	}

	return contains;
#endif


}

bool DrawGeometryOperation::IsClearOperation() {
	return m_bIsClearOperation;
}

//HRESULT DrawGeometryOperation::UpdateGeometry(ID2D1RenderTarget* renderTarget)
//{
	//if (m_inkPoints.size() == 1) {
	//	CComQIPtr<ID2D1DeviceContext2> spDeviceContext = renderTarget;
	//	spDeviceContext->CreateInk(m_inkPoints[0], &m_ink);
	//}
	//else if (m_inkPoints.size() > 1) {
	//	int n = 6;

	//	D2D1_INK_POINT point1;
	//	D2D1_INK_POINT point2;
	//	D2D1_INK_POINT point3;
	//	D2D1_INK_BEZIER_SEGMENT* segment = new D2D1_INK_BEZIER_SEGMENT[6];
	//	int j = 0;
	//	int i = 0;
	//	for (int i = 0; i < n; ++i, ++j) {
	//		point1.x = 113 * i;
	//		point1.y = 50 * i;
	//		point1.radius = 0;
	//		point2.x = 120 * i;
	//		point2.y = 70 * i;
	//		point2.radius = 0;
	//		point3.x = 153 * i;
	//		point3.y = 90 * i;
	//		point3.radius = 0;
	//		segment[i].point1 = point1;
	//		segment[i].point2 = point2;
	//		segment[i].point3 = point3;
	//	}

	//	m_ink->AddSegments(segment, i);
	//	delete[] segment;
	//	
	//}
	//return S_OK;
//}


//// original
//HRESULT DrawGeometryOperation::UpdateGeometry(ID2D1RenderTarget* renderTarget)
//{
//	// Use the render target's factory
//	HRESULT hr = S_OK;
//	//return hr;
//
//
//	ComPtr<ID2D1PathGeometry>		m_pathGeometry;
//	//if (!m_pathGeometry) {
//		ComPtr<ID2D1Factory> factory;
//		renderTarget->GetFactory(&factory);
//		hr = factory->CreatePathGeometry(&m_pathGeometry);
//		
//	//}
//
//	if (SUCCEEDED(hr))
//	{
//		ComPtr<ID2D1GeometrySink> m_sink;
//		hr = m_pathGeometry->Open(&m_sink);
//		int n = m_points.size();
//
//
//
//		if (n > 1) {
//			//std::vector<D2D1_POINT_2F> nwPoint;
//			//drawCardinalSpline(m_points, nwPoint, 100, 0.5);
//
//			//std::vector<D2D1_POINT_2F> firstCtrlPt(n - 1);
//			//std::vector<D2D1_POINT_2F> secondCtrlPt(n - 1);
//			//GetCurveControlPoints(m_points, firstCtrlPt, secondCtrlPt);
//
//			m_sink->SetSegmentFlags(D2D1_PATH_SEGMENT_FORCE_ROUND_LINE_JOIN);
//			m_sink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);
//			m_sink->BeginFigure(m_points[0], D2D1_FIGURE_BEGIN_FILLED);
//
//			//int count = nwPoint.size();
//			//for (size_t i = 1; i < count; i++)
//			//{
//
//			//	m_sink->AddBezier(D2D1::BezierSegment(firstCtrlPt[i - 1], secondCtrlPt[i - 1], m_points[i]));
//			//	////if (i % 2 == 0) {
//			//	//	// Smoothing
//			//	//if (i > 1 && i < m_points.size() - 1)
//			//	//{
//			//	//	D2D1_POINT_2F point1;
//			//	//	D2D1_POINT_2F point2;
//			//	//	GetSmoothingPoints(static_cast<int>(i), &point1, &point2);
//			//	//	sink->AddBezier(
//			//	//		D2D1::BezierSegment(
//			//	//			point1,
//			//	//			point2,
//			//	//			m_points[i]));
//			//	//	//sink->AddLine(m_points[i]);
//			//	//}
//			//	//else
//			//	//{
//			//	m_sink->AddLine(m_points[i]);
//				m_sink->AddLines(&m_points[1], m_points.size()-1);
//			//	//}
//			//	////}
//				//m_sink->AddLines(&nwPoint[0], nwPoint.size());
//
//			//}
//			m_sink->EndFigure(D2D1_FIGURE_END_OPEN);
//		}
//		if (SUCCEEDED(hr))
//		{
//			hr = m_sink->Close();
//		}
//	}
//
//
//	//if (SUCCEEDED(hr))
//	//{
//	//	m_geometry = m_pathGeometry;
//	//}
//
//	return hr;
//}
//
//
//void DrawGeometryOperation::GetSmoothingPoints(int i, D2D1_POINT_2F* point1, D2D1_POINT_2F* point2)
//{
//    static const float smoothing = 0.2; // 0 = no smoothing
//
//    // Calculate distance between previous point and current point
//    float dx = m_points[i].x - m_points[i - 1].x;
//    float dy = m_points[i].y - m_points[i - 1].y;
//    float currentDelta = std::sqrt(dx * dx + dy * dy); 
//
//    // Calculate distance between two points back and current point
//    float prevDx = m_points[i].x - m_points[i - 2].x;
//    float prevDy = m_points[i].y - m_points[i - 2].y;    
//    float prevDelta = std::sqrt(prevDx * prevDx + prevDy * prevDy);
//
//    // Calculate distance between previous point and the next point
//    float nextDx = m_points[i + 1].x - m_points[i - 1].x;
//    float nextDy = m_points[i + 1].y - m_points[i - 1].y;    
//    float nextDelta = std::sqrt(nextDx * nextDx + nextDy * nextDy); 
//
//    // Store point 1
//    *point1 = D2D1::Point2F(
//        m_points[i - 1].x + (prevDx == 0 ? 0 : (smoothing * currentDelta * prevDx / prevDelta)),
//        m_points[i - 1].y + (prevDy == 0 ? 0 : (smoothing * currentDelta * prevDy / prevDelta))
//        );
//
//    // Store point 2
//    *point2 = D2D1::Point2F(
//        m_points[i].x - (nextDx == 0 ? 0 : (smoothing * currentDelta * nextDx / nextDelta)),
//        m_points[i].y - (nextDy == 0 ? 0 : (smoothing * currentDelta * nextDy / nextDelta))
//        );
//}
//
//void DrawGeometryOperation::GetSmoothPoint(D2D1_POINT_2F point, D2D1_POINT_2F* point2) {
//
//	float x = 0.f;
//	float y = 0.f;
//
//	m_pointBuffer[m_cur++ % BUF_SIZE] = point;
//
//	int i = 0;
//	for (i = 0; i < BUF_SIZE && i < m_cur; ++i) {
//		x += m_pointBuffer[i].x;
//		y += m_pointBuffer[i].y;
//	}
//
//
//	*point2 = D2D1::Point2F(x / i, y / i);
//
//	return;
//}


//void DrawGeometryOperation::GetFirstControlPoints(
//	__in const std::vector<FLOAT>& rhs,
//	__out std::vector<FLOAT>& x)
//{
//	ATLASSERT(rhs.size() == x.size());
//	int n = rhs.size();
//	std::vector<FLOAT> tmp(n);    // Temp workspace.
//
//	FLOAT b = 2.0f;
//	x[0] = rhs[0] / b;
//	for (int i = 1; i < n; i++) // Decomposition and forward substitution.
//	{
//		tmp[i] = 1 / b;
//		b = (i < n - 1 ? 4.0f : 3.5f) - tmp[i];
//		x[i] = (rhs[i] - x[i - 1]) / b;
//	}
//	for (int i = 1; i < n; i++)
//	{
//		x[n - i - 1] -= tmp[n - i] * x[n - i]; // Back substitution.
//	}
//}
//
//void DrawGeometryOperation::GetCurveControlPoints(
//	__in const std::vector<D2D1_POINT_2F>& knots,
//	__out std::vector<D2D1_POINT_2F>& firstCtrlPt,
//	__out std::vector<D2D1_POINT_2F>& secondCtrlPt)
//{
//	ATLASSERT((firstCtrlPt.size() == secondCtrlPt.size())
//		&& (knots.size() == firstCtrlPt.size() + 1));
//
//	int n = knots.size() - 1;
//	ATLASSERT(n >= 1);
//
//	if (n == 1)
//	{
//		firstCtrlPt[0].x = (2 * knots[0].x + knots[1].x) / 3.0f;
//		firstCtrlPt[0].y = (2 * knots[0].y + knots[1].y) / 3.0f;
//
//		secondCtrlPt[0].x = 2 * firstCtrlPt[0].x - knots[0].x;
//		secondCtrlPt[0].y = 2 * firstCtrlPt[0].y - knots[0].y;
//		return;
//	}
//
//	std::vector<FLOAT> rhs(n);
//
//	for (int i = 1; i < (n - 1); ++i)
//	{
//		rhs[i] = 4 * knots[i].x + 2 * knots[i + 1].x;
//	}
//	rhs[0] = knots[0].x + 2 * knots[1].x;
//	rhs[n - 1] = (8 * knots[n - 1].x + knots[n].x) / 2.0f;
//	// Get first control points X-values
//	std::vector<FLOAT> x(n);
//	GetFirstControlPoints(rhs, x);
//
//	// Set right hand side Y values
//	for (int i = 1; i < (n - 1); ++i)
//	{
//		rhs[i] = 4 * knots[i].y + 2 * knots[i + 1].y;
//	}
//	rhs[0] = knots[0].y + 2 * knots[1].y;
//	rhs[n - 1] = (8 * knots[n - 1].y + knots[n].y) / 2.0f;
//	// Get first control points Y-values
//	std::vector<FLOAT> y(n);
//	GetFirstControlPoints(rhs, y);
//
//	// Fill output arrays.
//	for (int i = 0; i < n; ++i)
//	{
//		// First control point
//		firstCtrlPt[i] = D2D1::Point2F(x[i], y[i]);
//		// Second control point
//		if (i < (n - 1))
//		{
//			secondCtrlPt[i] = D2D1::Point2F(2 * knots[i + 1].x - x[i + 1], 2 * knots[i + 1].y - y[i + 1]);
//		}
//		else
//		{
//			secondCtrlPt[i] = D2D1::Point2F((knots[n].x + x[n - 1]) / 2, (knots[n].y + y[n - 1]) / 2);
//		}
//	}
//}


//
//void DrawGeometryOperation::drawCardinalSpline(
//	__in const std::vector<D2D1_POINT_2F>& points,
//	__out std::vector<D2D1_POINT_2F>& output,
//	int segs,                             // num segments
//	double c)                             // tension
//{
//	int n = points.size();   // Number of control points
//
//	if (n <= 2)
//	{
//		if (n == 1)
//		{
//			//glBegin(GL_POINTS);
//			//glVertex2dv(&v[0][0]);
//			//glEnd();
//		}
//		else if (n == 2)
//		{
//			//glBegin(GL_LINES);
//			//glVertex2dv(&v[0][0]);
//			//glVertex2dv(&v[1][0]);
//			//glEnd();
//		}
//		return;
//	}
//
//	// n >= 3
//
//	double xa, xb, xc, xd;  // Coefficients for x-coord polynomial
//	double ya, yb, yc, yd;  // Coefficients for y-coord polynomial
//
//	// Draw 1st segment
//	xa = (1. - c) / 2 * points[0].x
//		- 1. / 2 * points[1].x
//		+ c / 2 * points[2].x;
//	xb = 0.;
//	xc = (c - 3.) / 2 * points[0].x
//		+ 3. / 2 * points[1].x
//		+ -c / 2 * points[2].x;
//	xd = points[0].x;
//	ya = (1. - c) / 2 * points[0].y
//		- 1. / 2 * points[1].y
//		+ c / 2 * points[2].y;
//	yb = 0.;
//	yc = (c - 3.) / 2 * points[0].y
//		+ 3. / 2 * points[1].y
//		+ -c / 2 * points[2].y;
//	yd = points[0].y;
//	for (int k = 1; k <= segs; ++k)
//	{
//		double t = double(k) / segs;  // parameter
//		double x = xa * t*t*t + xb * t*t + xc * t + xd;
//		double y = ya * t*t*t + yb * t*t + yc * t + yd;
//		output.push_back(D2D1::Point2F(x, y));
//
//	}
//
//	// Draw intermediate segments
//	output.push_back(D2D1::Point2F(points[1].x, points[1].y));
//
//
//	for (int j = 0; j < n - 3; ++j)
//	{
//		xa = -c * points[j].x
//			+ (2. - c)*points[j + 1].x
//			+ (c - 2.)*points[j + 2].x
//			+ c * points[j + 3].x;
//		xb = 2.*c*points[j].x
//			+ (c - 3.)*points[j + 1].x
//			+ (3. - 2.*c)*points[j + 2].x
//			- c * points[j + 3].x;
//		xc = -c * points[j].x
//			+ c * points[j + 2].x;
//		xd = points[j + 1].x;
//		ya = -c * points[j].y
//			+ (2. - c)*points[j + 1].y
//			+ (c - 2.)*points[j + 2].y
//			+ c * points[j + 3].y;
//		yb = 2.*c*points[j].y
//			+ (c - 3.)*points[j + 1].y
//			+ (3. - 2.*c)*points[j + 2].y
//			- c * points[j + 3].y;
//		yc = -c * points[j].y
//			+ c * points[j + 2].y;
//		yd = points[j + 1].y;
//		for (int k = 1; k <= segs; ++k)
//		{
//			double t = double(k) / segs;  // parameter
//			double x = xa * t*t*t + xb * t*t + xc * t + xd;
//			double y = ya * t*t*t + yb * t*t + yc * t + yd;
//			output.push_back(D2D1::Point2F(x, y));
//		}
//	}
//
//	// Draw last segment
//	xa = -c / 2 * points[n - 3].x
//		+ 1. / 2 * points[n - 2].x
//		+ (c - 1.) / 2 * points[n - 1].x;
//	xb = 3.*c / 2 * points[n - 3].x
//		- 3. / 2 * points[n - 2].x
//		+ (3. - 3.*c) / 2 * points[n - 1].x;
//	xc = -c * points[n - 3].x
//		+ c * points[n - 1].x;
//	xd = points[n - 2].x;
//	ya = -c / 2 * points[n - 3].y
//		+ 1. / 2 * points[n - 2].y
//		+ (c - 1.) / 2 * points[n - 1].y;
//	yb = 3.*c / 2 * points[n - 3].y
//		- 3. / 2 * points[n - 2].y
//		+ (3. - 3.*c) / 2 * points[n - 1].y;
//	yc = -c * points[n - 3].y
//		+ c * points[n - 1].y;
//	yd = points[n - 2].y;
//	for (int k = 1; k <= segs; ++k)
//	{
//		double t = double(k) / segs;  // parameter
//		double x = xa * t*t*t + xb * t*t + xc * t + xd;
//		double y = ya * t*t*t + yb * t*t + yc * t + yd;
//		output.push_back(D2D1::Point2F(x, y));
//	}
//}
//
//
//
//
//
//HRESULT DrawGeometryOperation::MyUpdateGeometryOri(ID2D1RenderTarget* renderTarget, std::vector<D2D1_POINT_2F>& points)
//{
//	HRESULT hr = S_OK;
//
//	ComPtr<ID2D1PathGeometry>		m_pathGeometry;
//	ComPtr<ID2D1Factory> factory;
//	renderTarget->GetFactory(&factory);
//	hr = factory->CreatePathGeometry(&m_pathGeometry);
//
//	if (SUCCEEDED(hr))
//	{
//		ComPtr<ID2D1GeometrySink> m_sink;
//		hr = m_pathGeometry->Open(&m_sink);
//		int n = points.size();
//
//
//
//		if (n > 1) {
//			m_sink->SetSegmentFlags(D2D1_PATH_SEGMENT_FORCE_ROUND_LINE_JOIN);
//			m_sink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);
//			m_sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
//			m_sink->AddLines(&points[1], points.size() - 1);
//			m_sink->EndFigure(D2D1_FIGURE_END_OPEN);
//		}
//		if (SUCCEEDED(hr))
//		{
//			hr = m_sink->Close();
//		}
//
//
//		if (SUCCEEDED(hr))
//		{
//			m_geometryOri = m_pathGeometry;
//		}
//	}
//
//	return hr;
//}
//
//
//
//HRESULT DrawGeometryOperation::MyUpdateGeometry(ID2D1RenderTarget* renderTarget, std::vector<D2D1_POINT_2F>& points)
//{
//	HRESULT hr = S_OK;
//
//	ComPtr<ID2D1PathGeometry>		m_pathGeometry;
//	ComPtr<ID2D1Factory> factory;
//	renderTarget->GetFactory(&factory);
//	hr = factory->CreatePathGeometry(&m_pathGeometry);
//
//	if (SUCCEEDED(hr))
//	{
//		ComPtr<ID2D1GeometrySink> m_sink;
//		hr = m_pathGeometry->Open(&m_sink);
//		int n = points.size();
//
//
//
//		if (n > 1) {
//			m_sink->SetSegmentFlags(D2D1_PATH_SEGMENT_FORCE_ROUND_LINE_JOIN);
//			m_sink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);
//			m_sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
//			m_sink->AddLines(&points[1], points.size() - 1);
//			m_sink->EndFigure(D2D1_FIGURE_END_OPEN);
//		}
//		if (SUCCEEDED(hr))
//		{
//			hr = m_sink->Close();
//		}
//
//
//		if (SUCCEEDED(hr))
//		{
//			m_geometry = m_pathGeometry;
//		}
//	}
//
//	return hr;
//}
//
//
//
//void DrawGeometryOperation::GetFirstControlPoints(
//	__in const std::vector<FLOAT>& rhs,
//	__out std::vector<FLOAT>& x)
//{
//	ATLASSERT(rhs.size() == x.size());
//	int n = rhs.size();
//	std::vector<FLOAT> tmp(n);    // Temp workspace.
//
//	FLOAT b = 2.0f;
//	x[0] = rhs[0] / b;
//	for (int i = 1; i < n; i++) // Decomposition and forward substitution.
//	{
//		tmp[i] = 1 / b;
//		b = (i < n - 1 ? 4.0f : 3.5f) - tmp[i];
//		x[i] = (rhs[i] - x[i - 1]) / b;
//	}
//	for (int i = 1; i < n; i++)
//	{
//		x[n - i - 1] -= tmp[n - i] * x[n - i]; // Back substitution.
//	}
//}
//
//void DrawGeometryOperation::GetCurveControlPoints(
//	__in const std::vector<D2D1_POINT_2F>& knots,
//	__out std::vector<D2D1_POINT_2F>& firstCtrlPt,
//	__out std::vector<D2D1_POINT_2F>& secondCtrlPt)
//{
//	ATLASSERT((firstCtrlPt.size() == secondCtrlPt.size())
//		&& (knots.size() == firstCtrlPt.size() + 1));
//
//	int n = knots.size() - 1;
//	ATLASSERT(n >= 1);
//
//	if (n == 1)
//	{
//		firstCtrlPt[0].x = (2 * knots[0].x + knots[1].x) / 3.0f;
//		firstCtrlPt[0].y = (2 * knots[0].y + knots[1].y) / 3.0f;
//
//		secondCtrlPt[0].x = 2 * firstCtrlPt[0].x - knots[0].x;
//		secondCtrlPt[0].y = 2 * firstCtrlPt[0].y - knots[0].y;
//		return;
//	}
//
//	// Calculate first Bezier control points
//	// Right hand side vector
//	std::vector<FLOAT> rhs(n);
//
//	// Set right hand side X values
//	for (int i = 1; i < (n - 1); ++i)
//	{
//		rhs[i] = 4 * knots[i].x + 2 * knots[i + 1].x;
//	}
//	rhs[0] = knots[0].x + 2 * knots[1].x;
//	rhs[n - 1] = (8 * knots[n - 1].x + knots[n].x) / 2.0f;
//	// Get first control points X-values
//	std::vector<FLOAT> x(n);
//	GetFirstControlPoints(rhs, x);
//
//	// Set right hand side Y values
//	for (int i = 1; i < (n - 1); ++i)
//	{
//		rhs[i] = 4 * knots[i].y + 2 * knots[i + 1].y;
//	}
//	rhs[0] = knots[0].y + 2 * knots[1].y;
//	rhs[n - 1] = (8 * knots[n - 1].y + knots[n].y) / 2.0f;
//	// Get first control points Y-values
//	std::vector<FLOAT> y(n);
//	GetFirstControlPoints(rhs, y);
//
//	// Fill output arrays.
//	for (int i = 0; i < n; ++i)
//	{
//		// First control point
//		firstCtrlPt[i] = D2D1::Point2F(x[i], y[i]);
//		// Second control point
//		if (i < (n - 1))
//		{
//			secondCtrlPt[i] = D2D1::Point2F(2 * knots[i + 1].x - x[i + 1], 2 * knots[i + 1].y - y[i + 1]);
//		}
//		else
//		{
//			secondCtrlPt[i] = D2D1::Point2F((knots[n].x + x[n - 1]) / 2, (knots[n].y + y[n - 1]) / 2);
//		}
//	}
//}
//
//HRESULT DrawGeometryOperation::CreateBezierSpline(
//	__in ID2D1Factory* pD2dFactory,
//	__in const std::vector<D2D1_POINT_2F>& points,
//	__out ID2D1PathGeometry** ppPathGeometry)
//{
//
//	int n = points.size();
//	std::vector<D2D1_POINT_2F> firstCtrlPt(n - 1);
//	std::vector<D2D1_POINT_2F> secondCtrlPt(n - 1);
//	GetCurveControlPoints(points, firstCtrlPt, secondCtrlPt);
//
//	HRESULT hr = pD2dFactory->CreatePathGeometry(ppPathGeometry);
//	if (FAILED(hr))
//		return hr;
//
//	CComPtr<ID2D1GeometrySink> spSink;
//	hr = (*ppPathGeometry)->Open(&spSink);
//	if (SUCCEEDED(hr))
//	{
//		//m_sink->SetSegmentFlags(D2D1_PATH_SEGMENT_FORCE_ROUND_LINE_JOIN);
//		//m_sink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);
//		//m_sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
//		//m_sink->AddLines(&points[1], points.size() - 1);
//		//m_sink->EndFigure(D2D1_FIGURE_END_OPEN);
//
//
//		spSink->SetSegmentFlags(D2D1_PATH_SEGMENT_FORCE_ROUND_LINE_JOIN);
//		spSink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);
//		spSink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
//		for (int i = 1; i < n; i++) {
//			spSink->AddBezier(D2D1::BezierSegment(firstCtrlPt[i - 1], secondCtrlPt[i - 1], points[i]));
//			//m_pointsBz.push_back(firstCtrlPt[i-1]);
//			//m_pointsBz.push_back(secondCtrlPt[i - 1]);
//		}
//			
//		spSink->EndFigure(D2D1_FIGURE_END_OPEN);
//		spSink->Close();
//	}
//	return hr;
//}
//
//
//HRESULT DrawGeometryOperation::CreateBezierSpline(__in const std::vector<D2D1_POINT_2F>& points)
//{
//
//	HRESULT hr = S_OK;
//	int n = points.size();
//
//
//	if (n < 2) return hr;
//
//	std::vector<D2D1_POINT_2F> firstCtrlPt(n - 1);
//	std::vector<D2D1_POINT_2F> secondCtrlPt(n - 1);
//	GetCurveControlPoints(points, firstCtrlPt, secondCtrlPt);
//
//	CBezierSegmentPack				segmentPack;
//	D2D1_INK_POINT inkPoint;
//	inkPoint.radius = m_strokeSize;
//
//
//	for (int i = 1; i < n; i++) {
//
//		//spSink->AddBezier(D2D1::BezierSegment(firstCtrlPt[i - 1], secondCtrlPt[i - 1], points[i]));
//
//		inkPoint.x = firstCtrlPt[i - 1].x;
//		inkPoint.y = firstCtrlPt[i - 1].y;
//		segmentPack.Add(inkPoint);
//		inkPoint.x = secondCtrlPt[i - 1].x;
//		inkPoint.y = secondCtrlPt[i - 1].y;
//		segmentPack.Add(inkPoint);
//		inkPoint.x = points[i].x;
//		inkPoint.y = points[i].y;
//		segmentPack.Add(inkPoint);
//		m_ink->AddSegments(&segmentPack.Get(), 1);
//	}
//
//	
//
//	return hr;
//}
//
//
//
std::vector<D2D1_POINT_2F> DrawGeometryOperation::ComputeSmooth(std::vector<D2D1_POINT_2F>& points) {
	if (points.size() < 2) {
		return points;
	}

	double cutdist = 0.05 + (0.8 * 0.4);
	//double cutdist = 0.5;
	std::vector<D2D1_POINT_2F> newPoints;
	newPoints.push_back(points[0]);


	double pt1x;
	double pt1y;
	double pt2x;
	double pt2y;
	double distance;

	double ptPrevx = points[0].x;
	double ptPrevy = points[0].y;


	for (int i = 0; i < points.size() - 2; i++) {
		pt1x = (1 - cutdist)*points[i].x + cutdist * points[i + 1].x;
		pt1y = (1 - cutdist)*points[i].y + cutdist * points[i + 1].y;
		pt2x = cutdist * points[i].x + (1 - cutdist)*points[i + 1].x;
		pt2y = cutdist * points[i].y + (1 - cutdist)*points[i + 1].y;


		//distance = sqrt(pow(ptPrevx - pt1x, 2) + pow(ptPrevy - pt1y, 2));
		//if (distance > 1.f) {
		//	ptPrevx = pt1x;
		//	ptPrevy = pt1y;
			newPoints.push_back(D2D1::Point2F(pt1x+ 0.15, pt1y + 0.15));
		//}

		//distance = sqrt(pow(ptPrevx - pt2x, 2) + pow(ptPrevy - pt2y, 2));
		//if (distance > 1.f) {
		//	ptPrevx = pt2x;
		//	ptPrevy = pt2y;
			newPoints.push_back(D2D1::Point2F(pt2x + 0.15, pt2y + 0.15));
		//}
	}

	newPoints.push_back(points[points.size() - 1]);

	return newPoints;
}


std::vector<D2D1_INK_BEZIER_SEGMENT> DrawGeometryOperation::ComputeSmooth(std::vector<D2D1_INK_BEZIER_SEGMENT>& points) {
	//if (points.size() < 2) {
		return points;
	//}
	//double cutdist = 0.05 + (0.8 * 0.4);
	//double pt1x;
	//double pt1y;
	//double pt2x;
	//double pt2y;
	//double distance;

	//D2D1_INK_POINT inkPoint1;
	//inkPoint1.x = pt1x;
	//inkPoint1.y = pt1y;
	//inkPoint1.radius = m_strokeSize;// +presValue;

	//D2D1_INK_POINT inkPoint2;
	//inkPoint2.x = pt2x;
	//inkPoint2.y = pt2y;
	//inkPoint2.radius = m_strokeSize;// +presValue;


	//double ptPrevx = points[0].x;
	//double ptPrevy = points[0].y;


	//for (int i = 0; i < points.size() - 2; i++) {
	//	pt1x = (1 - cutdist)*points[i].x + cutdist * points[i + 1].x;
	//	pt1y = (1 - cutdist)*points[i].y + cutdist * points[i + 1].y;
	//	pt2x = cutdist * points[i].x + (1 - cutdist)*points[i + 1].x;
	//	pt2y = cutdist * points[i].y + (1 - cutdist)*points[i + 1].y;


	//	//distance = sqrt(pow(ptPrevx - pt1x, 2) + pow(ptPrevy - pt1y, 2));
	//	//if (distance > 1.f) {
	//	//	ptPrevx = pt1x;
	//	//	ptPrevy = pt1y;
	//	newPoints.push_back(D2D1::Point2F(pt1x + 0.15, pt1y + 0.15));
	//	//}

	//	//distance = sqrt(pow(ptPrevx - pt2x, 2) + pow(ptPrevy - pt2y, 2));
	//	//if (distance > 1.f) {
	//	//	ptPrevx = pt2x;
	//	//	ptPrevy = pt2y;
	//	newPoints.push_back(D2D1::Point2F(pt2x + 0.15, pt2y + 0.15));
	//	//}
	//}

	//newPoints.push_back(points[points.size() - 1]);

	//return newPoints;
}

//
//
//std::vector<D2D1_INK_POINT> DrawGeometryOperation::ComputeSmooth(std::vector<D2D1_INK_POINT>& points) {
//	if (points.size() < 2) {
//		return points;
//	}
//
//	double cutdist = 0.05 + (0.6 * 0.4);
//	std::vector<D2D1_INK_POINT> newPoints;
//	newPoints.push_back(points[0]);
//
//	//D2D1_POINT_2F point = D2D1::Point2F(points.size(), points.size());
//	//points.push_back(point);
//
//	double pt1x;
//	double pt1y;
//	double pt2x;
//	double pt2y;
//	FLOAT radius1;
//	FLOAT radius2;
//
//	for (int i = 0; i < points.size() - 2; i++) {
//		pt1x = (1 - cutdist)*points[i].x + cutdist * points[i + 1].x;
//		pt1y = (1 - cutdist)*points[i].y + cutdist * points[i + 1].y;
//		pt2x = cutdist * points[i].x + (1 - cutdist)*points[i + 1].x;
//		pt2y = cutdist * points[i].y + (1 - cutdist)*points[i + 1].y;
//		radius1 = points[i].radius;
//		radius2 = points[i + 1].radius;
//
//		newPoints.push_back(D2D1::InkPoint(D2D1::Point2F(pt1x, pt1y), radius1));
//		newPoints.push_back(D2D1::InkPoint(D2D1::Point2F(pt2x, pt2y), radius2));
//	}
//
//	newPoints.push_back(points[points.size() - 1]);
//
//	return newPoints;
//}
//
//



std::vector<D2D1_POINT_2F> DrawGeometryOperation::ComputeSmoothCenter(std::vector<D2D1_POINT_2F>& points) {
	if (points.size() < 2) {
		return points;
	}

	double cutdist = 0.5;
	std::vector<D2D1_POINT_2F> newPoints;
	newPoints.push_back(points[0]);

	double pt1x;
	double pt1y;
	double pt2x;
	double pt2y;
	double ptCenterx;
	double ptCentery;
	double distance;


	double ptPrevx = points[0].x;
	double ptPrevy = points[0].y;


	for (int i = 0; i < points.size() - 2; i++) {
		pt1x = (1 - cutdist)*points[i].x + cutdist * points[i + 1].x;
		pt1y = (1 - cutdist)*points[i].y + cutdist * points[i + 1].y;

		ptCenterx = (1 - cutdist)*ptPrevx + cutdist * pt1x;
		ptCentery = (1 - cutdist)*ptPrevy + cutdist * pt1y;
		ptPrevx = ptCenterx;
		ptPrevy = ptCentery;

		newPoints.push_back(D2D1::Point2F(ptCenterx, ptCentery));

	}

	newPoints.push_back(points[points.size() - 1]);

	return newPoints;
}



D2D1_INK_BEZIER_SEGMENT DrawGeometryOperation::ComputeSmoothCenter(D2D1_INK_BEZIER_SEGMENT point1, D2D1_INK_BEZIER_SEGMENT point2) {

	double cutdist = 0.5;

	double pt1x;
	double pt1y;
	double pt2x;
	double pt2y;
	double ptCenterx;
	double ptCentery;
	double distance;


	double ptPrevx = point1.point1.x;
	double ptPrevy = point1.point1.y;


	pt1x = (1 - cutdist)*point1.point3.x + cutdist * point2.point3.x;
	pt1y = (1 - cutdist)*point1.point3.y + cutdist * point2.point3.y;

	ptCenterx = (1 - cutdist)*ptPrevx + cutdist * pt1x;
	ptCentery = (1 - cutdist)*ptPrevy + cutdist * pt1y;

	D2D1_INK_BEZIER_SEGMENT newSegment;
	newSegment.point1.x = ptCenterx;
	newSegment.point1.y = ptCentery;
	newSegment.point1.radius = point2.point1.radius;
	newSegment.point2 = newSegment.point1;
	newSegment.point3 = newSegment.point1;
	return newSegment;
}




HRESULT DrawGeometryOperation::DynamicDrawToRenderTarget(__in ID2D1RenderTarget* renderTarget/*, D2D1_RECT_F imageRect*/, __in float alpha)
{
	HRESULT hr = S_OK;
	ComPtr<ID2D1Factory> factory;
	renderTarget->GetFactory(&factory);
	CComQIPtr<ID2D1DeviceContext2> spDeviceContext = renderTarget;


	if (!m_brush && SUCCEEDED(hr))
	{
		m_brushColor.a = m_brushAlpha;
		hr = renderTarget->CreateSolidColorBrush(m_brushColor, &m_brush);
	}

	if (!m_inkStyle) {
		spDeviceContext->GetTransform(&m_transMatrix);
		D2D1_MATRIX_3X2_F nibTransform = D2D1::Matrix3x2F::Scale(m_penStyleValueH, m_penStyleValueV);
		hr = spDeviceContext->CreateInkStyle(
			D2D1::InkStyleProperties(
				D2D1_INK_NIB_SHAPE_ROUND,
				nibTransform), &m_inkStyle);
	}

	if (m_ink) {
		spDeviceContext->DrawInk(m_ink, m_brush, m_inkStyle);
	}
	return hr;
}



//void DrawGeometryOperation::GetDrawingRect(__out D2D1_RECT_F* rect) {
//	*rect = m_drawingRect;
//	return;
//}