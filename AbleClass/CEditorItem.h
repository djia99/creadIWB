#pragma once



class CEditorItem
{

public:
	CEditorItem();
	virtual ~CEditorItem();

private:
	RECT			rect;
	//eItemType		type;
	int				type;
	std::wstring	strSource;
};

