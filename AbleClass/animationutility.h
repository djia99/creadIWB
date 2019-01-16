//===================================================================================
// Copyright (c) Microsoft Corporation.  All rights reserved.                        
//                                                                                   
// THIS CODE AND INFORMATION IS PROVIDED 'AS IS' WITHOUT WARRANTY                    
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT                       
// LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND                          
// FITNESS FOR A PARTICULAR PURPOSE.                                                 
//===================================================================================


#pragma once

#include "UIAnimation.h"

		//
		// This utility class provides several utility functions
		// to work with Window Animation manager, including the
		// Animation manager, timer and transitions libraty.
		//
class AnimationUtility
{
private:
	AnimationUtility();
	virtual ~AnimationUtility();
	static HRESULT Initialize();

public:
	static HRESULT GetAnimationManager(__in UINT type, __out IUIAnimationManager **animationManager);
	static HRESULT GetAnimationTimer(__in UINT type, __out IUIAnimationTimer **animationTimer);
	static HRESULT GetTransitionLibrary(__in UINT type, __out IUIAnimationTransitionLibrary **transitionLibrary);
	static HRESULT GetAnimationTimerTime(__in UINT type, __out UI_ANIMATION_SECONDS* animationSeconds);
	static HRESULT UpdateAnimationManagerTime(__in UINT type);
	static HRESULT ScheduleStoryboard(__in UINT type, IUIAnimationStoryboard* storyboard);
	static HRESULT IsAnimationManagerBusy(__in UINT type, __out bool* isBusy);
	static HRESULT AbandonAllStoryboards(__in UINT type);
};

