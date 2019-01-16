//===================================================================================
// Copyright (c) Microsoft Corporation.  All rights reserved.                        
//                                                                                   
// THIS CODE AND INFORMATION IS PROVIDED 'AS IS' WITHOUT WARRANTY                    
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT                       
// LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND                          
// FITNESS FOR A PARTICULAR PURPOSE.                                                 
//===================================================================================

#pragma once

#include "stdafx.h"
#include "ThumbnailLayoutInterface.h"



enum AnimationType
{
	FlyIn,
	MoveAround
};

struct AnimatedThumbnailCell
{
	ThumbnailCell cell;
	D2D1_POINT_2F start;
	D2D1_POINT_2F end;
	D2D1_POINT_2F center;
	bool isCurrent;

	AnimatedThumbnailCell(ThumbnailCell thumbnailCell, D2D1_POINT_2F startPoint, D2D1_POINT_2F endPoint, D2D1_POINT_2F centerPoint, bool isCurrentThumbnailCell) :
		cell(thumbnailCell),
		start(startPoint),
		end(endPoint),
		center(centerPoint),
		isCurrent(isCurrentThumbnailCell)
	{
	}
};



[uuid("F45A0AEF-698A-4F42-A6A0-653FED496EE6")]
__interface IPointAnimation : public IUnknown
{
public:
    HRESULT __stdcall GetCurrentPoint(__out D2D1_POINT_2F* point, FLOAT* pAlpha);
    HRESULT __stdcall Setup(D2D1_POINT_2F targetPoint, FLOAT alpha, double duration);
	HRESULT __stdcall SetupHover(D2D1_POINT_2F targetPoint, FLOAT alpha, double duration);
};


[uuid("5BD2C0FA-B047-426D-A40F-6AD0BCBA4C47")]
__interface IMediaPaneAnimation : public IUnknown
{
public:
	HRESULT __stdcall Setup(const std::vector<ThumbnailCell>& prevThumbnailCells, const std::vector<ThumbnailCell>& currentThumbnailCells, D2D1_SIZE_F viewSize);
	HRESULT __stdcall GetAnimationPosition(__in IThumbnail* const control, __out D2D1_POINT_2F* point);
	HRESULT __stdcall GetAnimatedThumbnailCells(std::vector<AnimatedThumbnailCell>& animatedCells);
};

[uuid("E77D43BE-3CDA-474A-9791-81138F610EB6")]
__interface IPanAnimation : public IUnknown
{
public:
	HRESULT __stdcall GetValue(__out double *currentPosition);
	HRESULT __stdcall Setup(__in const double startPosition, __in const double endPosition, __in const double duration);
};