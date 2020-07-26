//=============================================================================
// Copyright (c) 2001-2020 FLIR Systems, Inc. All Rights Reserved.
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
	quickSpinIntegerNode StreamBufferCountManual;
	quickSpinIntegerNode StreamBufferCountResult;
	quickSpinIntegerNode StreamBufferCountMax;
	quickSpinEnumerationNode StreamBufferCountMode;
	quickSpinEnumerationNode StreamBufferHandlingMode;
	quickSpinIntegerNode StreamAnnounceBufferMinimum;
	quickSpinIntegerNode StreamAnnouncedBufferCount;
	quickSpinIntegerNode StreamStartedFrameCount;
	quickSpinIntegerNode StreamDeliveredFrameCount;
	quickSpinIntegerNode StreamLostFrameCount;
	quickSpinIntegerNode StreamInputBufferCount;
	quickSpinIntegerNode StreamOutputBufferCount;
	quickSpinBooleanNode StreamCRCCheckEnable;
	quickSpinBooleanNode GevPacketResendMode;
	quickSpinIntegerNode GevMaximumNumberResendRequests;
	quickSpinIntegerNode GevPacketResendTimeout;
	quickSpinBooleanNode StreamIsGrabbing;
	quickSpinIntegerNode StreamChunkCountMaximum;
	quickSpinIntegerNode StreamBufferAlignment;
	quickSpinIntegerNode GevTotalPacketCount;
	quickSpinIntegerNode GevFailedPacketCount;
	quickSpinIntegerNode GevResendPacketCount;
	quickSpinIntegerNode StreamFailedBufferCount;
	quickSpinIntegerNode GevResendRequestCount;
	quickSpinIntegerNode StreamBlockTransferSize;
} quickSpinTLStream;

/*@}*/

#endif // FLIR_SPINNAKERC_TLSTREAM_H