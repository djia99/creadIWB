//===================================================================================
// Copyright (c) Microsoft Corporation.  All rights reserved.                        
//                                                                                   
// THIS CODE AND INFORMATION IS PROVIDED 'AS IS' WITHOUT WARRANTY                    
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT                       
// LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND                          
// FITNESS FOR A PARTICULAR PURPOSE.                                                 
//===================================================================================


#include "StdAfx.h"
#include "AnimationUtility.h"

static ComPtr<IUIAnimationManager> m_animationManager;
static ComPtr<IUIAnimationTimer> m_animationTimer;
static ComPtr<IUIAnimationTransitionLibrary> m_transitionLibrary;
//static ComPtr<IUIAnimationManager> m_animationManagerList;
//static ComPtr<IUIAnimationTimer> m_animationTimerList;
//static ComPtr<IUIAnimationTransitionLibrary> m_transitionLibraryList;


AnimationUtility::AnimationUtility()
{
}

AnimationUtility::~AnimationUtility()
{
	//m_animationManagerEdit = nullptr;
	//m_animationTimerEdit = nullptr;
	//m_transitionLibraryEdit = nullptr;
	//m_animationManagerList = nullptr;
	//m_animationTimerList = nullptr;
	//m_transitionLibraryList = nullptr;
}

HRESULT AnimationUtility::GetAnimationTimer(UINT type, IUIAnimationTimer **animationTimer)
{
    HRESULT hr = Initialize();
    if (SUCCEEDED(hr))
    {
		//if (type == PAGE_ITEM) {
		//	hr = AssignToOutputPointer(animationTimer, g_global.m_animationTimerEdit);
		//}
		//else if (type == PAGE_LIST) {
		//	hr = AssignToOutputPointer(animationTimer, g_global.m_animationTimerList);
		//}
		AssignToOutputPointer(animationTimer, m_animationTimer);
    }

    return hr;
}

HRESULT AnimationUtility::GetAnimationManager(UINT type, IUIAnimationManager **animationManager)
{
    HRESULT hr = Initialize();

    if (SUCCEEDED(hr))
    {
		//if (type == PAGE_ITEM) {
		//	hr = AssignToOutputPointer(animationManager, g_global.m_animationManagerEdit);
		//}
		//else if (type == PAGE_LIST) {
		//	hr = AssignToOutputPointer(animationManager, g_global.m_animationManagerList);
		//}
		hr = AssignToOutputPointer(animationManager, m_animationManager);
    }

    return hr;
}

HRESULT AnimationUtility::GetTransitionLibrary(UINT type, IUIAnimationTransitionLibrary **transitionLibrary)
{
    HRESULT hr = Initialize();
    if (SUCCEEDED(hr))
    {
		//if (type == PAGE_ITEM) {
		//	hr = AssignToOutputPointer(transitionLibrary, g_global.m_transitionLibraryEdit);
		//}
		//else if (type == PAGE_LIST) {
		//	hr = AssignToOutputPointer(transitionLibrary, g_global.m_transitionLibraryList);
		//}
		AssignToOutputPointer(transitionLibrary, m_transitionLibrary);
    }

    return hr;
}

HRESULT AnimationUtility::GetAnimationTimerTime(UINT type, UI_ANIMATION_SECONDS* animationSeconds)
{
    HRESULT hr = Initialize();

    if (SUCCEEDED(hr))
    {
		//if (type == PAGE_ITEM) {
		//	hr = g_global.m_animationTimerEdit->GetTime(animationSeconds);
		//}
		//else if (type == PAGE_LIST) {
		//	hr = g_global.m_animationTimerList->GetTime(animationSeconds);
		//}
		hr = m_animationTimer->GetTime(animationSeconds);
    }

    return hr;
}

HRESULT AnimationUtility::UpdateAnimationManagerTime(UINT type)
{
    UI_ANIMATION_SECONDS secondsNow = 0;

    HRESULT hr = Initialize();
    if (SUCCEEDED(hr))
    {
		//if (type == PAGE_ITEM) {
		//	hr = g_global.m_animationTimerEdit->GetTime(&secondsNow);
		//}
		//else if (type == PAGE_LIST) {
		//	hr = g_global.m_animationTimerList->GetTime(&secondsNow);
		//}
		hr = m_animationTimer->GetTime(&secondsNow);
    }

    if (SUCCEEDED(hr))
    {
		//if (type == PAGE_ITEM) {
		//	hr = g_global.m_animationManagerEdit->Update(secondsNow);
		//}
		//else if (type == PAGE_LIST) {
		//	hr = g_global.m_animationManagerList->Update(secondsNow);
		//}
		hr = m_animationManager->Update(secondsNow);
    }


    return hr;
}

HRESULT AnimationUtility::Initialize()
{
    // Create Animation Manager
    HRESULT hr = S_OK;
    
	if (!m_animationManager)
	{
		hr = CoCreateInstance(
			CLSID_UIAnimationManager,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_animationManager));
	}

	if (SUCCEEDED(hr) && !m_animationTimer)
	{
		// Create Animation Timer
		hr = CoCreateInstance(
			CLSID_UIAnimationTimer,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_animationTimer));
	}

	if (SUCCEEDED(hr) && !m_transitionLibrary)
	{
		// Create Animation Transition Library
		hr = CoCreateInstance(
			CLSID_UIAnimationTransitionLibrary,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_transitionLibrary));
	}

 //   if (!g_global.m_animationManagerEdit)
 //   {
 //       hr = CoCreateInstance(
 //           CLSID_UIAnimationManager,
 //           nullptr,
 //           CLSCTX_INPROC_SERVER,
 //           IID_PPV_ARGS(&g_global.m_animationManagerEdit));
 //   }

 //   if (SUCCEEDED(hr) && !g_global.m_animationTimerEdit)
 //   {
 //       // Create Animation Timer
 //       hr = CoCreateInstance(
 //           CLSID_UIAnimationTimer,
 //           nullptr,
 //           CLSCTX_INPROC_SERVER,
 //           IID_PPV_ARGS(&g_global.m_animationTimerEdit));
 //   }

 //   if (SUCCEEDED(hr) && !g_global.m_transitionLibraryEdit)
 //   {
 //       // Create Animation Transition Library
 //       hr = CoCreateInstance(
 //           CLSID_UIAnimationTransitionLibrary,
 //           nullptr,
 //           CLSCTX_INPROC_SERVER,
 //           IID_PPV_ARGS(&g_global.m_transitionLibraryEdit));
 //   }
	//if (!g_global.m_animationManagerList)
	//{
	//	hr = CoCreateInstance(
	//		CLSID_UIAnimationManager,
	//		nullptr,
	//		CLSCTX_INPROC_SERVER,
	//		IID_PPV_ARGS(&g_global.m_animationManagerList));
	//}

	//if (SUCCEEDED(hr) && !g_global.m_animationTimerList)
	//{
	//	// Create Animation Timer
	//	hr = CoCreateInstance(
	//		CLSID_UIAnimationTimer,
	//		nullptr,
	//		CLSCTX_INPROC_SERVER,
	//		IID_PPV_ARGS(&g_global.m_animationTimerList));
	//}

	//if (SUCCEEDED(hr) && !g_global.m_transitionLibraryList)
	//{
	//	// Create Animation Transition Library
	//	hr = CoCreateInstance(
	//		CLSID_UIAnimationTransitionLibrary,
	//		nullptr,
	//		CLSCTX_INPROC_SERVER,
	//		IID_PPV_ARGS(&g_global.m_transitionLibraryList));
	//}

    return hr;
}

HRESULT AnimationUtility::ScheduleStoryboard(UINT type, IUIAnimationStoryboard* storyboard)
{
    UI_ANIMATION_SECONDS secondsNow = static_cast<UI_ANIMATION_SECONDS>(0);

    HRESULT hr = Initialize();
    if (SUCCEEDED(hr))
    {
		//if (type == PAGE_ITEM) {
		//	hr = g_global.m_animationTimerEdit->GetTime(&secondsNow);
		//}
		//else if (type == PAGE_LIST) {
		//	hr = g_global.m_animationTimerList->GetTime(&secondsNow);
		//}
		hr = m_animationTimer->GetTime(&secondsNow);
    }

    if (SUCCEEDED(hr))
    {
        hr = storyboard->Schedule(secondsNow);
    }

    return hr;
}

HRESULT AnimationUtility::IsAnimationManagerBusy(UINT type, bool* isBusy)
{
    UI_ANIMATION_MANAGER_STATUS status = UI_ANIMATION_MANAGER_IDLE;

    HRESULT hr = Initialize();
    if (SUCCEEDED(hr))
    {
		//if (type == PAGE_ITEM) {
		//	hr = g_global.m_animationManagerEdit->GetStatus(&status);
		//}
		//else if (type == PAGE_LIST) {
		//	hr = g_global.m_animationManagerList->GetStatus(&status);
		//}
		hr = m_animationManager->GetStatus(&status);
    }

    if (SUCCEEDED(hr))
    {
        *isBusy = (status == UI_ANIMATION_MANAGER_BUSY);
    }

    return hr;
}



HRESULT AnimationUtility::AbandonAllStoryboards(UINT type)
{
	UI_ANIMATION_SECONDS secondsNow = 0;

	HRESULT hr = Initialize();
	if (SUCCEEDED(hr))
	{

		//if (type == PAGE_ITEM) {
		//	hr = g_global.m_animationManagerEdit->AbandonAllStoryboards();
		//}
		//else if (type == PAGE_LIST) {
		//	hr = g_global.m_animationManagerList->AbandonAllStoryboards();
		//}

		hr = m_animationManager->AbandonAllStoryboards();

	}
	return hr;
}