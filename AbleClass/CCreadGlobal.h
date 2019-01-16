#pragma once



struct stPenColor {
	COLORREF clr;
	bool b;
};


class CCreadGlobal
{
public:
	CCreadGlobal();
	~CCreadGlobal();


	//ComPtr<IUIAnimationManager>				m_animationManagerEdit;
	//ComPtr<IUIAnimationTimer>				m_animationTimerEdit;
	//ComPtr<IUIAnimationTransitionLibrary>	m_transitionLibraryEdit;
	//ComPtr<IUIAnimationManager>				m_animationManagerList;
	//ComPtr<IUIAnimationTimer>				m_animationTimerList;
	//ComPtr<IUIAnimationTransitionLibrary>	m_transitionLibraryList;


	//ComPtr<IUIAnimationManager>				GetAniManagerEdit() { return m_animationManagerEdit; }
	//ComPtr<IUIAnimationTimer>				GetAniTimerEdit() { return m_animationTimerEdit; }
	//ComPtr<IUIAnimationTransitionLibrary>	GetAniTranLibEdit() { return m_transitionLibraryEdit; }
	//ComPtr<IUIAnimationManager>				GetAniManagerList() { return m_animationManagerList; }
	//ComPtr<IUIAnimationTimer>				GetAniTimerList() { return m_animationTimerList; }
	//ComPtr<IUIAnimationTransitionLibrary>	GetAniTranLibList() { return m_transitionLibraryList; }


	std::string sFormat(const std::string string, ...);
	std::wstring sFormat(const std::wstring string, ...);
	std::wstring s2ws(const std::string& s);
	std::string ws2s(const std::wstring& s);


};

