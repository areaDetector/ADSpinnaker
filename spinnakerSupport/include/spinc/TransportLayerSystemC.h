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

#ifndef FLIR_SPINNAKERC_TLSYSTEM_H
#define FLIR_SPINNAKERC_TLSYSTEM_H

/**	
* @defgroup TLSystemC_h TLSystem Structures
* @ingroup CQuickSpin
*/
/*@{*/

typedef struct _quickSpinTLSystem
{
	quickSpinStringNode TLID;
	quickSpinStringNode TLVendorName;
	quickSpinStringNode TLModelName;
	quickSpinStringNode TLVersion;
	quickSpinStringNode TLFileName;
	quickSpinStringNode TLDisplayName;
	quickSpinStringNode TLPath;
	quickSpinEnumerationNode TLType;
	quickSpinIntegerNode GenTLVersionMajor;
	quickSpinIntegerNode GenTLVersionMinor;
	quickSpinIntegerNode GenTLSFNCVersionMajor;
	quickSpinIntegerNode GenTLSFNCVersionMinor;
	quickSpinIntegerNode GenTLSFNCVersionSubMinor;
	quickSpinIntegerNode GevVersionMajor;
	quickSpinIntegerNode GevVersionMinor;
	quickSpinCommandNode InterfaceUpdateList;
	quickSpinIntegerNode InterfaceSelector;
	quickSpinStringNode InterfaceID;
	quickSpinStringNode InterfaceDisplayName;
	quickSpinIntegerNode GevInterfaceMACAddress;
	quickSpinIntegerNode GevInterfaceDefaultIPAddress;
	quickSpinIntegerNode GevInterfaceDefaultSubnetMask;
	quickSpinIntegerNode GevInterfaceDefaultGateway;
	quickSpinBooleanNode EnumerateGEVInterfaces;
	quickSpinBooleanNode EnumerateUSBInterfaces;
} quickSpinTLSystem;

/*@}*/

#endif // FLIR_SPINNAKERC_TLSYSTEM_H