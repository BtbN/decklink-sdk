/* -LICENSE-START-
 ** Copyright (c) 2010 Blackmagic Design
 **
 ** Permission is hereby granted, free of charge, to any person or organization
 ** obtaining a copy of the software and accompanying documentation covered by
 ** this license (the "Software") to use, reproduce, display, distribute,
 ** execute, and transmit the Software, and to prepare derivative works of the
 ** Software, and to permit third-parties to whom the Software is furnished to
 ** do so, all subject to the following:
 ** 
 ** The copyright notices in the Software and this entire statement, including
 ** the above license grant, this restriction and the following disclaimer,
 ** must be included in all copies of the Software, in whole or in part, and
 ** all derivative works of the Software, unless such copies or derivative
 ** works are solely in the form of machine-executable object code generated by
 ** a source language processor.
 ** 
 ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 ** FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 ** SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 ** FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 ** ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 ** DEALINGS IN THE SOFTWARE.
 ** -LICENSE-END-
 */
#ifndef __capture_h__

#include "DeckLinkAPI.h"
#include <pthread.h>

/*
 * CaptureHelper
 * This class captures a video stream from a tape deck, given an in- and out- timecode.
 * The video mode, in- and out-timecodes are specified as macros at the top of
 * the implementation file.
 */
class CaptureHelper : public IDeckLinkDeckControlStatusCallback , public IDeckLinkInputCallback
{
private:
	int32_t						m_refCount;
	IDeckLink *					m_deckLink;
	IDeckLinkDeckControl *		m_deckControl;
	IDeckLinkInput *			m_deckLinkInput;
	
	// The mutex and condition variable are used to wait for
	// - a deck to be connected
	// - the capture to complete
	pthread_mutex_t				m_mutex ;
	pthread_cond_t				m_condition;
	bool						m_waitingForDeckConnected;
	bool						m_waitingForCaptureEnd;
	bool						m_captureStarted;
	
	// video mode
	long						m_width;
	long						m_height;
	BMDTimeScale				m_timeScale;
	BMDTimeValue				m_frameDuration;
	
	// frame count values for the capture in- and out-points
	uint32_t					m_inPointFrameCount;
	uint32_t					m_outPointFrameCount;
	
	virtual			~CaptureHelper();
	
	// setup the IDeckLinkInput interface (video standard, pixel format, callback object, ...)
	bool			setupDeckLinkInput();
	// setup the IDeckLinkDeckControl interface
	bool			setupDeckControl();
	
	void			cleanupDeckControl();
	void			cleanupDeckLinkInput();
	
	
public:
	CaptureHelper(IDeckLink *deckLink);
	
	// init() must be called after the constructor.
	// if init() fails, call the destructor
	bool			init();
	
	// start the capture operation. returns when the operation has completed
	void			doCapture();
	
	// IDeckLinkDeckControlStatusCallback
	virtual HRESULT TimecodeUpdate (/* in */ BMDTimecodeBCD currentTimecode);
    virtual HRESULT VTRControlStateChanged (/* in */ BMDDeckControlVTRControlState newState, /* in */ BMDDeckControlError error);
    virtual HRESULT DeckControlEventReceived (/* in */ BMDDeckControlEvent event, /* in */ BMDDeckControlError error);
    virtual HRESULT DeckControlStatusChanged (/* in */ BMDDeckControlStatusFlags flags, /* in */ uint32_t mask);
	
	// IDeckLinkInputCallback
	virtual HRESULT	VideoInputFormatChanged (BMDVideoInputFormatChangedEvents notificationEvents, IDeckLinkDisplayMode* newDisplayMode, BMDDetectedVideoInputFormatFlags detectedSignalFlags) {return S_OK;};
	virtual HRESULT	VideoInputFrameArrived (IDeckLinkVideoInputFrame* arrivedFrame, IDeckLinkAudioInputPacket*);
	
	// IUnknown
	HRESULT			QueryInterface (REFIID iid, LPVOID *ppv);
	ULONG			AddRef ();
	ULONG			Release ();
};

#endif		// __capture_h__

