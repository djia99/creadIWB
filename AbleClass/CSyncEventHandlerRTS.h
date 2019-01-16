#pragma once

#include "CWhiteBoardDlg.h"
#include <msinkaut.h>
#include <msinkaut_i.c>

//#define PRESSURE_TYPE 2048
#define PRESSURE_TYPE 1024


class CSyncEventHandlerRTS : public IStylusSyncPlugin
{
public:
	CSyncEventHandlerRTS();
	virtual ~CSyncEventHandlerRTS();

public:
	// Factory method
	static IStylusSyncPlugin* Create(IRealTimeStylus* pRealTimeStylus, IDynamicRenderer* pDynamicRenderer, /*IStrokeBuilder* strokeBuilder, */CWhiteBoardDlg* pTargetWindow);
	void SetDynamicRenderer(IDynamicRenderer* pDynamicRenderer);

	// IStylusSyncPlugin methods

	// Handled IStylusSyncPlugin methods, they require nontrivial implementation
	STDMETHOD(StylusDown)(IRealTimeStylus* piSrcRtp, const StylusInfo* pStylusInfo, ULONG cPropCountPerPkt, LONG* pPacket, LONG** ppInOutPkt);
	STDMETHOD(StylusUp)(IRealTimeStylus* piSrcRtp, const StylusInfo* pStylusInfo, ULONG cPropCountPerPkt, LONG* pPacket, LONG** ppInOutPkt);
	STDMETHOD(Packets)(IRealTimeStylus* piSrcRtp, const StylusInfo* pStylusInfo, ULONG cPktCount, ULONG cPktBuffLength, LONG* pPackets, ULONG* pcInOutPkts, LONG** ppInOutPkts);
	STDMETHOD(DataInterest)(RealTimeStylusDataInterest* pEventInterest);

	// IStylusSyncPlugin methods with trivial inline implementation, they all return S_OK
	STDMETHOD(RealTimeStylusEnabled)(IRealTimeStylus*, ULONG, const TABLET_CONTEXT_ID*);// { return S_OK; }
	STDMETHOD(RealTimeStylusDisabled)(IRealTimeStylus*, ULONG, const TABLET_CONTEXT_ID*) { return S_OK; }
	STDMETHOD(StylusInRange)(IRealTimeStylus*, TABLET_CONTEXT_ID, STYLUS_ID) { return S_OK; }
	STDMETHOD(StylusOutOfRange)(IRealTimeStylus*, TABLET_CONTEXT_ID, STYLUS_ID) { return S_OK; }
	STDMETHOD(InAirPackets)(IRealTimeStylus*, const StylusInfo*, ULONG, ULONG, LONG*, ULONG*, LONG**) { return S_OK; }
	STDMETHOD(StylusButtonUp)(IRealTimeStylus*, STYLUS_ID, const GUID*, POINT*) { return S_OK; }
	STDMETHOD(StylusButtonDown)(IRealTimeStylus*, STYLUS_ID, const GUID*, POINT*) { return S_OK; }
	STDMETHOD(SystemEvent)(IRealTimeStylus*, TABLET_CONTEXT_ID, STYLUS_ID, SYSTEM_EVENT, SYSTEM_EVENT_DATA) { return S_OK; }
	STDMETHOD(TabletAdded)(IRealTimeStylus*, IInkTablet*) { return S_OK; }
	STDMETHOD(TabletRemoved)(IRealTimeStylus*, LONG) { return S_OK; }
	STDMETHOD(CustomStylusDataAdded)(IRealTimeStylus*, const GUID*, ULONG, const BYTE*) { return S_OK; }
	STDMETHOD(Error)(IRealTimeStylus*, IStylusPlugin*, RealTimeStylusDataInterest, HRESULT, LONG_PTR*) { return S_OK; }
	STDMETHOD(UpdateMapping)(IRealTimeStylus*) { return S_OK; }

	// IUnknown methods
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObj);

private:
	LONG m_cRefCount;                   // COM object reference count
	IUnknown* m_punkFTMarshaller;       // free-threaded marshaller
	int m_nContacts;                    // number of fingers currently in the contact with the touch digitizer
	IDynamicRenderer*	m_pDynamicRenderer;
	CWhiteBoardDlg*		m_pTargetWindow;
	//IStrokeBuilder*		m_pStrokeBuilder;
	//IInkStrokeDisp*		m_piStroke;
	
	int m_nPixelsPerInchX;    // Pixels per logical inch along width
	int m_nPixelsPerInchY;    // Pixels per logical inch along height

	BOOL m_bPressure;


	void	HiMetricToPixel(LONG* pfX, LONG* pfY);
	
};

