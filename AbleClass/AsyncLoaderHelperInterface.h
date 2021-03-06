//===================================================================================
// Copyright (c) Microsoft Corporation.  All rights reserved.                        
//                                                                                   
// THIS CODE AND INFORMATION IS PROVIDED 'AS IS' WITHOUT WARRANTY                    
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT                       
// LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND                          
// FITNESS FOR A PARTICULAR PURPOSE.                                                 
//===================================================================================


#pragma once
#include "AsyncLoader\AsyncLoaderInterfaces.h"

namespace AsyncLoaderSpace
{
	//
	// interface IAsyncLoaderHelper
	//
	// This interface aggregates the common AsyncLoader functionality into
	// one interface so that it is easier to use by clients.
	//
	[uuid("D9D3A0A6-0CED-4364-A3E8-399A23A80999")]
	__interface IAsyncLoaderHelper : public IUnknown
	{
		// 재로딩 추가
		HRESULT __stdcall StartFullImageLoad(LoadSize type, unsigned int index);
		HRESULT __stdcall SetLoadType(LOADTYPE loadType);
		BOOL	__stdcall CheckListLoadDone();					// 리스트 항목 로딩 완료 체크

		HRESULT __stdcall StartBackgroundLoading();
		HRESULT __stdcall PauseBackgroundLoading();
		HRESULT __stdcall ResumeBackgroundLoading();
		HRESULT __stdcall Shutdown();

		HRESULT __stdcall GetAsyncLoader(IAsyncLoader** asyncLoader);

		// Window manipulation.
		HRESULT __stdcall SetPageItemCount(unsigned int count);
		HRESULT __stdcall SetCurrentPagePivot(unsigned int pivot);
		HRESULT __stdcall Scroll(int scrollBy);
		HRESULT __stdcall Zoom(float factor);
		HRESULT __stdcall ConnectClient(CWnd* client);
		HRESULT __stdcall ConnectItem(IUnknown* newItem, int location);
		HRESULT __stdcall ClearItems();
	};
}

