//=============================================================================
// Copyright (c) 2001-2023 FLIR Systems, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of FLIR
// Integrated Imaging Solutions, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with FLIR Integrated Imaging Solutions, Inc. (FLIR).
//
// FLIR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. FLIR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================

/* Auto-generated file. Do not modify. */

#include "QuickSpinC.h"

#ifndef FLIR_SPINNAKERC_TLSTREAM_H
#define FLIR_SPINNAKERC_TLSTREAM_H

/**	
* @defgroup TLStreamC_h TLStream Structures
* @ingroup CQuickSpin
*/
/*@{*/

typedef struct _quickSpinTLStream
{
	quickSpinStringNode StreamID;
	quickSpinEnumerationNode StreamType;
	quickSpinEnumerationNode StreamMode;
	quickSpinIntegerNode StreamBufferCountManual;
	quickSpinIntegerNode StreamBufferCountResult;
	quickSpinIntegerNode StreamBufferCountMax;
	quickSpinEnumerationNode StreamBufferCountMode;
	quickSpinEnumerationNode StreamBufferHandlingMode;
	quickSpinIntegerNode StreamAnnounceBufferMinimum;
	quickSpinIntegerNode StreamAnnouncedBufferCount;
	quickSpinIntegerNode StreamStartedFrameCount;
	quickSpinIntegerNode StreamDeliveredFrameCount;
	quickSpinIntegerNode StreamReceivedFrameCount;
	quickSpinIntegerNode StreamIncompleteFrameCount;
	quickSpinIntegerNode StreamLostFrameCount;
	quickSpinIntegerNode StreamDroppedFrameCount;
	quickSpinIntegerNode StreamInputBufferCount;
	quickSpinIntegerNode StreamOutputBufferCount;
	quickSpinBooleanNode StreamIsGrabbing;
	quickSpinIntegerNode StreamChunkCountMaximum;
	quickSpinIntegerNode StreamBufferAlignment;
	quickSpinBooleanNode StreamCRCCheckEnable;
	quickSpinIntegerNode StreamReceivedPacketCount;
	quickSpinIntegerNode StreamMissedPacketCount;
	quickSpinBooleanNode StreamPacketResendEnable;
	quickSpinIntegerNode StreamPacketResendTimeout;
	quickSpinIntegerNode StreamPacketResendMaxRequests;
	quickSpinIntegerNode StreamPacketResendRequestCount;
	quickSpinIntegerNode StreamPacketResendRequestSuccessCount;
	quickSpinIntegerNode StreamPacketResendRequestedPacketCount;
	quickSpinIntegerNode StreamPacketResendReceivedPacketCount;
	quickSpinIntegerNode StreamBlockTransferSize;
} quickSpinTLStream;

/*@}*/

#endif // FLIR_SPINNAKERC_TLSTREAM_H