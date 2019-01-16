//===================================================================================
// Copyright (c) Microsoft Corporation.  All rights reserved.                        
//                                                                                   
// THIS CODE AND INFORMATION IS PROVIDED 'AS IS' WITHOUT WARRANTY                    
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT                       
// LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND                          
// FITNESS FOR A PARTICULAR PURPOSE.                                                 
//===================================================================================

#pragma once

#include "ImageOperation.h"
#include <d2d1_3.h>
#include <inkrenderer.h>


class CBezierSegmentPack {
public:
	CBezierSegmentPack() : refIndex(0) {
		memset(&segment, 0, sizeof(segment));
	}
	D2D1_INK_BEZIER_SEGMENT segment;
	int refIndex;
	bool Add(D2D1_INK_POINT _point) {
		bool hr = false;
		if (refIndex == 0) {
			segment.point1 = _point;
			refIndex++;
		}
		else if (refIndex == 1) {
			segment.point2 = _point;
			refIndex++;
		}
		else {
			refIndex = 0;
			segment.point3 = _point;
			hr = true;
		}
		//segment.point1 = _point;
		//segment.point2 = _point;
		//segment.point3 = _point;



		return hr;
	}

	D2D1_INK_BEZIER_SEGMENT Get() {
		return segment;
		//D2D1_INK_BEZIER_SEGMENT segmentClone = segment;
		//segment.point1 = segment.point2;
		//segment.point2 = segment.point3;
		//return segmentClone;
	}
};


[uuid("17A76C82-5618-451F-A3D3-CEEC1A503749")]
__interface IDrawGeometryOperation : public IUnknown
{
	// Ink
	//HRESULT __stdcall DrawToRenderTargetInk(__in ID2D1RenderTarget* renderTarget, __in float alpha = 1.f);
	//HRESULT __stdcall AppendPointInk(__in ID2D1RenderTarget* renderTarget, __in D2D1_POINT_2F point, __in LONG pressure = 1);
	// end Ink

	HRESULT __stdcall DynamicDrawToRenderTarget(__in ID2D1RenderTarget* renderTarget/*, D2D1_RECT_F imageRect*/, __in float alpha = 1.f);
    HRESULT __stdcall DrawToRenderTarget(__in ID2D1RenderTarget* renderTarget/*, D2D1_RECT_F imageRect*/, __in float alpha=1.f);
	HRESULT __stdcall EndGeometry(__in ID2D1RenderTarget* renderTarget, __in D2D1_POINT_2F point, LONG pressure);
    HRESULT __stdcall DiscardResources();

    HRESULT __stdcall AppendPoint(__in ID2D1RenderTarget* renderTarget, __in D2D1_POINT_2F point, __in LONG pressure=1);
    HRESULT __stdcall SetBrushColor(__in D2D1_COLOR_F brushColor);
    HRESULT __stdcall SetStrokeSize(__in float strokeSize);
	HRESULT __stdcall SetBrushAlpha(__in float alpha);
	HRESULT __stdcall SetClearOperation(__in bool bClear);
	bool	__stdcall IsClearOperation();
	BOOL	__stdcall IsContaion(__in D2D1_POINT_2F point);
	void	__stdcall SetPenStyleValue(PENTIP_STYLE ps, bool pressure);
	void	__stdcall SetPenId(__in UINT id);
	//void	__stdcall GetDrawingRect(__out D2D1_RECT_F* rect);
	
};

class DrawGeometryOperation : public IImageOperation, public IDrawGeometryOperation
{
public:
    // IDrawGeometryOperation methods
	// Ink
	//HRESULT __stdcall DrawToRenderTargetInk(__in ID2D1RenderTarget* renderTarget, __in float alpha = 1.f);
	//HRESULT __stdcall AppendPointInk(__in ID2D1RenderTarget* renderTarget, __in D2D1_POINT_2F point, __in LONG pressure = 1);
	// end Ink

	HRESULT __stdcall DynamicDrawToRenderTarget(__in ID2D1RenderTarget* renderTarget/*, D2D1_RECT_F imageRect*/, __in float alpha = 1.f);
    HRESULT __stdcall DrawToRenderTarget(__in ID2D1RenderTarget* renderTarget/*, D2D1_RECT_F imageRect*/, __in float alpha=1.f);
	HRESULT __stdcall EndGeometry(__in ID2D1RenderTarget* renderTarget, __in D2D1_POINT_2F point, LONG pressure);
    HRESULT __stdcall DiscardResources()
    {
        if (m_brush) m_brush = nullptr;
		if (m_strokeStyle) m_strokeStyle = nullptr;
		//if (m_ink) m_ink = nullptr;
		//if (m_inkStyle) m_inkStyle = nullptr;
		//if (m_pathGeometry) m_pathGeometry = nullptr;
		//if (m_geometry!=nullptr) m_geometry = nullptr;
		//if (m_geometryOri != nullptr) m_geometryOri = nullptr;
		
		
        return S_OK;
    }
	void	__stdcall SetPenStyleValue(PENTIP_STYLE ps, bool pressure);


protected:
    DrawGeometryOperation();
    virtual ~DrawGeometryOperation();

    // Interface helper
    bool QueryInterfaceHelper(const IID &iid, void **object)
    {
        return CastHelper<IImageOperation>::CastTo(iid, this, object) ||
            CastHelper<IDrawGeometryOperation>::CastTo(iid, this, object);
    }

    // IGeometryShape
    HRESULT __stdcall AppendPoint(__in ID2D1RenderTarget* renderTarget, __in D2D1_POINT_2F point, __in LONG pressure = 1);
    HRESULT __stdcall SetBrushColor(__in D2D1_COLOR_F brushColor);
    HRESULT __stdcall SetStrokeSize(__in float strokeSize);
	HRESULT __stdcall SetBrushAlpha(__in float alpha);
	HRESULT __stdcall SetClearOperation(__in bool bClear);
	BOOL	__stdcall IsContaion(__in D2D1_POINT_2F point);
	bool	__stdcall IsClearOperation();
	void	__stdcall SetPenId(__in UINT id);
	//void	__stdcall GetDrawingRect(__out D2D1_RECT_F* rect);
private:
	// 
	UINT								m_lastDrawedPointIndex;

	UINT								m_id;

	//ComPtr<ID2D1PathGeometry>			m_pathGeometry;
    ComPtr<ID2D1StrokeStyle>			m_strokeStyle;
    ComPtr<ID2D1Geometry>				m_geometry;
	//ComPtr<ID2D1Geometry>				m_geometryOri;
    ComPtr<ID2D1SolidColorBrush>		m_brush;
	bool								m_bIsClearOperation;
    D2D1_COLOR_F						m_brushColor;
    float								m_strokeSize;
	float								m_brushAlpha;
    std::vector<D2D1_POINT_2F>			m_points;
	std::vector<D2D1_POINT_2F>			m_pointsCompute;
	D2D1_RECT_F							m_drawingRect;
	//std::vector<D2D1_POINT_2F>		m_pointsOri;
	//std::vector<D2D1_POINT_2F>		m_pointsBz;
	D2D_MATRIX_3X2_F					m_transMatrix;
	//int								m_pointsPtr;


	//ComPtr<ID2D1Ink>					m_ink;
	//ComPtr<ID2D1Ink>					m_realTimeInk;
	//ComPtr<ID2D1InkStyle>				m_inkStyle;
	//std::vector<D2D1_INK_POINT>		m_inkPoints;
	//std::queue<D2D1_INK_POINT>		m_inkPointsRealtime;
	//D2D1_INK_POINT					m_lastPoint;
	//D2D1_INK_BEZIER_SEGMENT			m_segment;
	//CBezierSegmentPack				m_segmentPack;
	//CBezierSegmentPack				m_segmentPackRealTime;
	
	//ComPtr<IInkD2DRenderer>			m_inkRender;
	//ComPtr<IInkStrokeDisp>			m_inkStrokeDisp;
	//ComPtr<IInkStrokes>				m_inkStroke;

	



	// 필압 사용 여부
	BOOL							m_bPressure;
	float							m_penStyleValueH;
	float							m_penStyleValueV;
	


	//ComPtr<IStrokeBuilder>			m_inkStrokeBuilder;
	//ComPtr<IInkStrokeDisp>			m_piStroke;

	std::vector<D2D1_POINT_2F> ComputeSmoothCenter(std::vector<D2D1_POINT_2F>& points);
	D2D1_INK_BEZIER_SEGMENT ComputeSmoothCenter(D2D1_INK_BEZIER_SEGMENT point1, D2D1_INK_BEZIER_SEGMENT point2);

	
	std::vector<D2D1_POINT_2F> ComputeSmooth(std::vector<D2D1_POINT_2F>& points);
	std::vector<D2D1_INK_BEZIER_SEGMENT> ComputeSmooth(std::vector<D2D1_INK_BEZIER_SEGMENT>& points);
	//std::vector<D2D1_INK_POINT> ComputeSmooth(std::vector<D2D1_INK_POINT>& points);
	BOOL m_bOn;


	//////////////////////////////////////////////////////////////////////////////////////////////
	// Ink interface
	//////////////////////////////////////////////////////////////////////////////////////////////
	D2D1_INK_POINT m_inkPointPrev;
	ComPtr<ID2D1Ink>		m_ink;
	ComPtr<ID2D1InkStyle>	m_inkStyle;
	std::vector<D2D1_INK_POINT>		m_inkPoints;
	std::vector<D2D1_INK_BEZIER_SEGMENT> m_inkSegments;
	bool pointOnLine(D2D1_POINT_2F p, D2D1_POINT_2F l1, D2D1_POINT_2F l2);
	//HRESULT DrawToRenderTargetInk(__in ID2D1RenderTarget* renderTarget, __in float alpha = 1.f);
	//HRESULT AppendPointInk(__in ID2D1RenderTarget* renderTarget, __in D2D1_POINT_2F point, __in LONG pressure = 1);
	//////////////////////////////////////////////////////////////////////////////////////////////
	// end Ink interface
	//////////////////////////////////////////////////////////////////////////////////////////////




//#define BUF_SIZE 2

	//int								m_cur;
	//D2D1_POINT_2F					m_pointBuffer[BUF_SIZE];
	//void							GetSmoothPoint(D2D1_POINT_2F point, D2D1_POINT_2F* point2);

    //void GetSmoothingPoints(int i, __out D2D1_POINT_2F* point1, __out D2D1_POINT_2F* point2);
 //   HRESULT UpdateGeometry(ID2D1RenderTarget* renderTarget);
	//HRESULT MyUpdateGeometryOri(ID2D1RenderTarget* renderTarget, std::vector<D2D1_POINT_2F>& points);
	//HRESULT MyUpdateGeometry(ID2D1RenderTarget* renderTarget, std::vector<D2D1_POINT_2F>& points);

	//void GetFirstControlPoints(__in const std::vector<FLOAT>& rhs, __out std::vector<FLOAT>& x);
	//void GetCurveControlPoints(
	//	__in const std::vector<D2D1_POINT_2F>& knots,
	//	__out std::vector<D2D1_POINT_2F>& firstCtrlPt,
	//	__out std::vector<D2D1_POINT_2F>& secondCtrlPt);


	//void GetFirstControlPoints(__in const std::vector<FLOAT>& rhs, __out std::vector<FLOAT>& x);
	//void GetCurveControlPoints(__in const std::vector<D2D1_POINT_2F>& knots, __out std::vector<D2D1_POINT_2F>& firstCtrlPt, __out std::vector<D2D1_POINT_2F>& secondCtrlPt);
	//HRESULT CreateBezierSpline(__in ID2D1Factory* pD2dFactory, __in const std::vector<D2D1_POINT_2F>& points, __out ID2D1PathGeometry** ppPathGeometry);
	//HRESULT CreateBezierSpline(__in const std::vector<D2D1_POINT_2F>& points);

	//void drawCardinalSpline(
	//	__in const std::vector<D2D1_POINT_2F>& points,
	//	__out std::vector<D2D1_POINT_2F>& output,
	//	int segs,                             // num segments
	//	double c);                             // tension
};

