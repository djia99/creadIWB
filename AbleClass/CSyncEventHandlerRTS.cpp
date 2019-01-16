#include "stdafx.h"
#include "CSyncEventHandlerRTS.h"

#define HIMETRIC_PER_INCH   2539.5
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)

CSyncEventHandlerRTS::CSyncEventHandlerRTS::CSyncEventHandlerRTS()
	: m_cRefCount(1),
	m_punkFTMarshaller(NULL),
	m_nContacts(0),
	m_bPressure(FALSE)
{
	HDC hDCScreen = GetDC(NULL);
	m_nPixelsPerInchX = GetDeviceCaps(hDCScreen, LOGPIXELSX);
	m_nPixelsPerInchY = GetDeviceCaps(hDCScreen, LOGPIXELSY);
	ReleaseDC(NULL, hDCScreen);

}

// CSyncEventHandlerRTS destructor.
CSyncEventHandlerRTS::~CSyncEventHandlerRTS()
{
	if (m_punkFTMarshaller != NULL)
	{
		m_punkFTMarshaller->Release();
	}
}



// CSyncEventHandlerRTS factory method: creates new CSyncEventHandlerRTS and adds it to the synchronous
// plugin list of the RTS object.
// in:
//      pRealTimeStylus         RTS object
// returns:
//      CSyncEventHandlerRTS object through IStylusSyncPlugin interface, or NULL on failure
IStylusSyncPlugin* CSyncEventHandlerRTS::Create(IRealTimeStylus* pRealTimeStylus, IDynamicRenderer* pDynamicRenderer, /*IStrokeBuilder* strokeBuilder, */CWhiteBoardDlg* pTargetWindow)
{
	// Check input argument
	if (pRealTimeStylus == NULL)
	{
		ASSERT(pRealTimeStylus != NULL && L"CSyncEventHandlerRTS::Create: invalid argument RealTimeStylus");
		return NULL;
	}

	// Instantiate CSyncEventHandlerRTS object
	CSyncEventHandlerRTS* pSyncEventHandlerRTS = new CSyncEventHandlerRTS();
	if (pSyncEventHandlerRTS == NULL)
	{
		//ASSERT(pSyncEventHandlerRTS != NULL && L"CSyncEventHandlerRTS::Create: cannot create instance of CSyncEventHandlerRTS");
		return NULL;
	}

	pSyncEventHandlerRTS->SetDynamicRenderer(pDynamicRenderer);
	pSyncEventHandlerRTS->m_pTargetWindow = pTargetWindow;
	//pSyncEventHandlerRTS->m_pStrokeBuilder = strokeBuilder;

	// Create free-threaded marshaller for this object and aggregate it.
	HRESULT hr = CoCreateFreeThreadedMarshaler(pSyncEventHandlerRTS, &pSyncEventHandlerRTS->m_punkFTMarshaller);
	if (FAILED(hr))
	{
		//ASSERT(SUCCEEDED(hr) && L"CSyncEventHandlerRTS::Create: cannot create free-threaded marshaller");
		pSyncEventHandlerRTS->Release();
		return NULL;
	}

	// Add CSyncEventHandlerRTS object to the list of synchronous plugins in the RTS object.
	hr = pRealTimeStylus->AddStylusSyncPlugin(
		0,                      // insert plugin at position 0 in the sync plugin list
		pSyncEventHandlerRTS);  // plugin to be inserted - event handler CSyncEventHandlerRTS
	if (FAILED(hr))
	{
		//ASSERT(SUCCEEDED(hr) && L"CEventHandlerRTS::Create: failed to add CSyncEventHandlerRTS to the RealTimeStylus plugins");
		pSyncEventHandlerRTS->Release();
		return NULL;
	}

	return pSyncEventHandlerRTS;
}

HRESULT CSyncEventHandlerRTS::RealTimeStylusEnabled(
	/* [in] */ IRealTimeStylus *piRtsSrc,
	/* [in] */ ULONG cTcidCount,
	/* [size_is][in] */ const TABLET_CONTEXT_ID *pTcids)
{
	//CoCreateInstance(CLSID_StrokeBuilder, NULL, CLSCTX_INPROC, IID_IStrokeBuilder, (VOID **)&m_pStrokeBuilder);
	//TRACE(_T("\n>>>>>>>>>>> RealTimeStylusEnabled <<<<<<<<\n "));
	// Create an IStrokeBuilder object
	return S_OK;
}



// Pen-down notification.
// Sets the color for the newly started stroke and increments finger-down counter.
// in:
//      piRtsSrc            RTS object that has sent this event
//      pStylusInfo         StylusInfo struct (context ID, cursor ID, etc)
//      cPropCountPerPkt    number of properties per packet
//      pPacket             packet data (layout depends on packet description set)
// in/out:
//      ppInOutPkt          modified packet data (same layout as pPackets)
// returns:
//      HRESULT error code
HRESULT CSyncEventHandlerRTS::StylusDown(
	IRealTimeStylus* piRtsSrc /* piRtsSrc */,
	const StylusInfo* pStylusInfo/* pStylusInfo */,
	ULONG  cPropCountPerPkt,
	LONG* pPacket,
	LONG** ppInOutPkt /* ppInOutPkt */)
{
	//TRACE(L"\n>>> StylusDown <<<<\n");

	//

	//TABLET_CONTEXT_ID *pTcids;
	//ULONG ulTcidCount;
	//TABLET_CONTEXT_ID tcid;
	//FLOAT fInkToDeviceScaleX;
	//FLOAT fInkToDeviceScaleY;
	//ULONG ulPacketProperties;
	//PACKET_PROPERTY *pPacketProperties;

	////// Get all the tablet context identifiers
	//HRESULT hr = piRtsSrc->GetAllTabletContextIds(&ulTcidCount, &pTcids);

	////// Use the first tablet context identifier in the array
	//tcid = *pTcids;

	////// Get the packet description data
	//hr = piRtsSrc->GetPacketDescriptionData(tcid, &fInkToDeviceScaleX,
	//	&fInkToDeviceScaleY, &ulPacketProperties,
	//	&pPacketProperties);



	//IInkTablet* pInk = NULL;
	//hr = piRtsSrc->GetTabletFromTabletContextId(tcid, &pInk);


	//pPacketProperties->PropertyMetrics.nLogicalMax(;
	


	//IInkTablet* pInk = NULL;
	//IInkTablet2* pInk2 = NULL;
	//TabletDeviceKind kind = TDK_Pen;
	//piRtsSrc->SetAllTabletsMode(FALSE);

	//if (SUCCEEDED(piRtsSrc->SetSingleTabletMode(pInk)))
	//{
	//	TRACE(L"\n>>>>> SetSingleTabletMode <<<<<<<\n");
	//	IInkTablet* pTablet = NULL;

	//	if ((SUCCEEDED(piRtsSrc->GetTablet(&pTablet))) && (NULL != pTablet))
	//	{


	//			HRESULT hr = pInk->QueryInterface(IID_IInkTablet2, (void**)&pInk2);
	//			pInk2->get_DeviceKind(&kind);
	//			if (kind == TDK_Mouse) {
	//				TRACE(L"\n>>>>> ¸¶¿ì½º <<<<<<<\n");
	//			}
	//			else if (kind == TDK_Pen) {
	//				TRACE(L"\n>>>>> TDK_PEN <<<<<<<\n");
	//			}
	//			else if (kind == TDK_Touch) {
	//				TRACE(L"\n>>>>> TDK_touch <<<<<<<\n");
	//			}

	//	}
	//}
	//// Restore all tablets mode.
	//piRtsSrc->SetAllTabletsMode(TRUE);
	



	if (cPropCountPerPkt > 0)
	{
		POINT pt;
		pt.x = pPacket[0];
		pt.y = pPacket[1];
	
		//pt.x = MAP_LOGHIM_TO_PIX(pt.x, m_nPixelsPerInchX);
		//pt.y = MAP_LOGHIM_TO_PIX(pt.y, m_nPixelsPerInchY);
		pt.x = pt.x * m_nPixelsPerInchX / 2539.5;
		pt.y = pt.y * m_nPixelsPerInchY / 2539.5;
		D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(pt.x, pt.y);
		LONG pressure = PRESSURE_TYPE;
		if (cPropCountPerPkt > 2) {
			pressure = pPacket[2];
			m_bPressure = TRUE;
		}
		else {
			m_bPressure = FALSE;
		}


		if (m_pTargetWindow->EnableDynamicDrawing(mousePosition)) {
		//if (m_pTargetWindow->IsOperationTypePen() && m_pTargetWindow->IsImageHit(mousePosition) == Direct2DUtility::HIT_IN) {
			if (m_pTargetWindow->InputProcessDown(mousePosition, pressure) == TRUE) {
				//////////////////////////////////////////////////////////////////////////////////////////
				// dynamicRenderer
				//////////////////////////////////////////////////////////////////////////////////////////
				BOOL bEnable;
				m_pDynamicRenderer->get_Enabled(&bEnable);
				if (bEnable) {
					IInkDrawingAttributes* pDrawingAttributesDynamicRenderer;
					HRESULT hr = m_pDynamicRenderer->get_DrawingAttributes(&pDrawingAttributesDynamicRenderer);

					//float width;
					//pDrawingAttributesDynamicRenderer->get_Width(&width);
					//TRACE(L"\n>>> pen width (%f) <<<<<<<<<<\n", width);

					pDrawingAttributesDynamicRenderer->put_IgnorePressure(VARIANT_TRUE);
					//pDrawingAttributesDynamicRenderer->put_PenTip(IPT_Ball);
					//pDrawingAttributesDynamicRenderer->put_Height(200);
					//pDrawingAttributesDynamicRenderer->put_AntiAliased(VARIANT_TRUE);
					//pDrawingAttributesDynamicRenderer->put_FitToCurve(VARIANT_TRUE);
					pDrawingAttributesDynamicRenderer->put_Width(m_pTargetWindow->GetPenSizeDynamicRender());
					long transparency = (m_pTargetWindow->GetPenType() == 1) ? 100 : 0;
					pDrawingAttributesDynamicRenderer->put_Transparency(transparency);
					hr = pDrawingAttributesDynamicRenderer->put_Color(m_pTargetWindow->GetPenColor());
					pDrawingAttributesDynamicRenderer->Release();
				}
				//////////////////////////////////////////////////////////////////////////////////////////
				// dynamicRenderer end
				//////////////////////////////////////////////////////////////////////////////////////////


				//m_pTargetWindow->PostMessage(MY_WM_BEGINDRAW, mousePosition.x, mousePosition.y);
			}

			


		}
		
	}

	++m_nContacts;  // Increment finger-down counter

	//FLOAT fInkToDeviceScaleX;
	//FLOAT fInkToDeviceScaleY;
	//ULONG cPacketProperties;
	//PACKET_PROPERTY* pPacketProperties;

	//HRESULT hr = piRtsSrc->GetPacketDescriptionData(pStylusInfo->tcid, &fInkToDeviceScaleX, &fInkToDeviceScaleY,
	//	&cPacketProperties, &pPacketProperties);

	

	//if (SUCCEEDED(hr))
	//{
	//	// Start creating the stroke
	//	hr = m_pStrokeBuilder->BeginStroke(pStylusInfo->tcid, pStylusInfo->cid, pPacket, cPropCountPerPkt,
	//		pPacketProperties, fInkToDeviceScaleX, fInkToDeviceScaleY, &m_piStroke);
	//}

	return S_OK;
}

// Pen-up notification.
// Decrements finger-down counter.
// in:
//      piRtsSrc            RTS object that has sent this event
//      pStylusInfo         StylusInfo struct (context ID, cursor ID, etc)
//      cPropCountPerPkt    number of properties per packet
//      pPacket             packet data (layout depends on packet description set)
// in/out:
//      ppInOutPkt          modified packet data (same layout as pPackets)
// returns:
//      HRESULT error code
HRESULT CSyncEventHandlerRTS::StylusUp(
	IRealTimeStylus* piRtsSrc,
	const StylusInfo* pStylusInfo,
	ULONG cPropCountPerPkt,
	LONG* pPacket,
	LONG** ppInOutPkt)
{
	if (cPropCountPerPkt > 0)
	{

		POINT pt;
		pt.x = pPacket[0];
		pt.y = pPacket[1];

		LONG pressure = PRESSURE_TYPE;
		if (cPropCountPerPkt > 2) {
			pressure = pPacket[2];
		}

		pt.x = MAP_LOGHIM_TO_PIX(pt.x, m_nPixelsPerInchX);
		pt.y = MAP_LOGHIM_TO_PIX(pt.y, m_nPixelsPerInchY);
		D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(pt.x, pt.y);

		if (m_pTargetWindow->IsOperationTypePen())
		{
			m_pTargetWindow->InputProcessUp(mousePosition, pressure);
		}
		m_pTargetWindow->PostMessage(MY_WM_BEGINDRAW, mousePosition.x, WM_LBUTTONUP);
	}

	--m_nContacts;  // Decrement finger-down counter
	//return m_pStrokeBuilder->EndStroke(pStylusInfo->tcid, pStylusInfo->cid, &m_piStroke, NULL);
	return S_OK;
}

// Pen-move notification.
// In this case, does nothing, but likely to be used in a more complex application.
// RTS framework does stroke collection and rendering for us.
// in:
//      piRtsRtp            RTS object that has sent this event
//      pStylusInfo         StylusInfo struct (context ID, cursor ID, etc)
//      cPktCount           number of packets
//      cPktBuffLength      pPacket buffer size, in elements, equal to number of packets times number of properties per packet
//      pPackets            packet data (layout depends on packet description set)
// in/out:
//      pcInOutPkts         modified number of packets
//      ppInOutPkts         modified packet data (same layout as pPackets)
// returns:
//      HRESULT error code
HRESULT CSyncEventHandlerRTS::Packets(
	IRealTimeStylus* piSrcRtp /* piSrcRtp */,
	const StylusInfo* pStylusInfo /* pStylusInfo */,
	ULONG cPktCount,
	ULONG cPktBuffLength/* cPktBuffLength */,
	LONG* pPacket,
	ULONG* pcInOutPkts /* pcInOutPkts */,
	LONG** ppInOutPkts /* ppInOutPkts */)
{
	if (cPktCount > 0)
	{
		POINT pt;
		pt.x = pPacket[0];
		pt.y = pPacket[1];

	

		LONG pressure = PRESSURE_TYPE;
		ULONG cPropertyCount = cPktBuffLength / cPktCount;
		if (cPropertyCount > 2) {
			pressure = pPacket[2];
		}


		pt.x = MAP_LOGHIM_TO_PIX(pt.x, m_nPixelsPerInchX);
		pt.y = MAP_LOGHIM_TO_PIX(pt.y, m_nPixelsPerInchY);
		D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(pt.x, pt.y);


		if (m_pTargetWindow->IsOperationTypePen())
		{
			m_pTargetWindow->InputProcessMove(mousePosition, pressure);
		}

		//m_pTargetWindow->PostMessage(MY_WM_BEGINDRAW, mousePosition.x, mousePosition.y);

	}

	//return m_pStrokeBuilder->AppendPackets(pStylusInfo->tcid, pStylusInfo->cid, cPktBuffLength, pPacket);
	return S_OK;
}

// Defines which handlers are called by the framework. We set the flags for pen-down, pen-up and pen-move.
// in/out:
//      pDataInterest       flags that enable/disable notification handlers
// returns:
//      HRESULT error code
HRESULT CSyncEventHandlerRTS::DataInterest(RealTimeStylusDataInterest *pDataInterest)
{
	*pDataInterest = (RealTimeStylusDataInterest)(RTSDI_StylusDown | RTSDI_Packets | RTSDI_StylusUp);

	return S_OK;
}

// Increments reference count of the COM object.
// returns:
//      reference count
ULONG CSyncEventHandlerRTS::AddRef()
{
	return InterlockedIncrement(&m_cRefCount);
}

// Decrements reference count of the COM object, and deletes it
// if there are no more references left.
// returns:
//      reference count
ULONG CSyncEventHandlerRTS::Release()
{
	ULONG cNewRefCount = InterlockedDecrement(&m_cRefCount);
	if (cNewRefCount == 0)
	{
		delete this;
	}
	return cNewRefCount;
}

// Returns a pointer to any interface supported by this object.
// If IID_IMarshal interface is requested, delegate the call to the aggregated
// free-threaded marshaller.
// If a valid pointer is returned, COM object reference count is increased.
// returns:
//      pointer to the interface requested, or NULL if the interface is not supported by this object
HRESULT CSyncEventHandlerRTS::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
	if ((riid == IID_IStylusSyncPlugin) || (riid == IID_IUnknown))
	{
		*ppvObj = this;
		AddRef();
		return S_OK;
	}
	else if ((riid == IID_IMarshal) && (m_punkFTMarshaller != NULL))
	{
		return m_punkFTMarshaller->QueryInterface(riid, ppvObj);
	}

	*ppvObj = NULL;
	return E_NOINTERFACE;
}


void CSyncEventHandlerRTS::HiMetricToPixel(LONG* pfX, LONG* pfY)
{
	int xPixelsPerInch = 96;
	int yPixelsPerInch = 96;
	*pfX = *pfX / ((LONG)(2540.0 / xPixelsPerInch));
	*pfY = *pfY / ((LONG)(2540.0 / yPixelsPerInch));
}


void CSyncEventHandlerRTS::SetDynamicRenderer(IDynamicRenderer* pDynamicRenderer)
{
	m_pDynamicRenderer = pDynamicRenderer;
	return;
}