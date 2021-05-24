//=============================================================================
// Copyright (c) 2001-2021 FLIR Systems, Inc. All Rights Reserved.
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

#ifndef FLIR_SPINNAKERC_TLINTERFACE_H
#define FLIR_SPINNAKERC_TLINTERFACE_H

/**	
* @defgroup TLInterfaceC_h TLInterface Structures
* @ingroup CQuickSpin
*/
/*@{*/

typedef struct _quickSpinTLInterface
{
	quickSpinStringNode InterfaceID;
	quickSpinStringNode InterfaceDisplayName;
	quickSpinEnumerationNode InterfaceType;
	quickSpinIntegerNode GevInterfaceGatewaySelector;
	quickSpinIntegerNode GevInterfaceGateway;
	quickSpinIntegerNode GevInterfaceMACAddress;
	quickSpinIntegerNode GevInterfaceSubnetSelector;
	quickSpinIntegerNode GevInterfaceSubnetIPAddress;
	quickSpinIntegerNode GevInterfaceSubnetMask;
	quickSpinIntegerNode GevInterfaceTransmitLinkSpeed;
	quickSpinIntegerNode GevInterfaceReceiveLinkSpeed;
	quickSpinIntegerNode GevInterfaceMTU;
	quickSpinEnumerationNode POEStatus;
	quickSpinEnumerationNode FilterDriverStatus;
	quickSpinIntegerNode GevActionDeviceKey;
	quickSpinIntegerNode GevActionGroupKey;
	quickSpinIntegerNode GevActionGroupMask;
	quickSpinIntegerNode GevActionTime;
	quickSpinCommandNode ActionCommand;
	quickSpinStringNode DeviceUnlock;
	quickSpinCommandNode DeviceUpdateList;
	quickSpinIntegerNode DeviceCount;
	quickSpinIntegerNode DeviceSelector;
	quickSpinStringNode DeviceID;
	quickSpinStringNode DeviceVendorName;
	quickSpinStringNode DeviceModelName;
	quickSpinStringNode DeviceSerialNumber;
	quickSpinEnumerationNode DeviceAccessStatus;
	quickSpinIntegerNode GevDeviceIPAddress;
	quickSpinIntegerNode GevDeviceSubnetMask;
	quickSpinIntegerNode GevDeviceGateway;
	quickSpinIntegerNode GevDeviceMACAddress;
	quickSpinIntegerNode IncompatibleDeviceCount;
	quickSpinIntegerNode IncompatibleDeviceSelector;
	quickSpinStringNode IncompatibleDeviceID;
	quickSpinStringNode IncompatibleDeviceVendorName;
	quickSpinStringNode IncompatibleDeviceModelName;
	quickSpinIntegerNode IncompatibleGevDeviceIPAddress;
	quickSpinIntegerNode IncompatibleGevDeviceSubnetMask;
	quickSpinIntegerNode IncompatibleGevDeviceMACAddress;
	quickSpinCommandNode GevDeviceForceIP;
	quickSpinIntegerNode GevDeviceForceIPAddress;
	quickSpinIntegerNode GevDeviceForceSubnetMask;
	quickSpinIntegerNode GevDeviceForceGateway;
	quickSpinCommandNode GevDeviceAutoForceIP;
	quickSpinStringNode HostAdapterName;
	quickSpinStringNode HostAdapterVendor;
	quickSpinStringNode HostAdapterDriverVersion;
} quickSpinTLInterface;

/*@}*/

#endif // FLIR_SPINNAKERC_TLINTERFACE_H