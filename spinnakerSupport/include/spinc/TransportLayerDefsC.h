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

#ifndef FLIR_SPINNAKERC_TRANSPORTLAYERDEFSC_H
#define FLIR_SPINNAKERC_TRANSPORTLAYERDEFSC_H

/**
* @defgroup CQuickSpin Spinnaker C QuickSpin API
*/
/**@{*/

/**	
* @defgroup TransportLayerDefsC_h Transport Layer Enumerations
*/
/**@{*/

/**
*@brief The enumeration definitions for transport layer nodes.
*/

typedef enum _spinTLStreamTypeEnums	/*!< Stream type of the device.*/
{
	StreamType_GigEVision,/*!< GigE Vision*/
	StreamType_CameraLink,/*!< Camera Link*/
	StreamType_CameraLinkHS,/*!< Camera Link High Speed*/
	StreamType_CoaXPress,/*!< CoaXPress*/
	StreamType_USB3Vision,/*!< USB3 Vision*/
	StreamType_Custom,/*!< Custom transport layer*/
	NUMSTREAMTYPE
} spinTLStreamTypeEnums;

typedef enum _spinTLStreamModeEnums	/*!< Stream mode of the device.*/
{
	StreamMode_Socket,/*!< Socket*/
	StreamMode_LWF,/*!< Light Weight Filter Driver*/
	StreamMode_TeledyneGigeVision,/*!< Teledyne Gige Vision Driver*/
	NUMSTREAMMODE
} spinTLStreamModeEnums;

typedef enum _spinTLStreamBufferCountModeEnums	/*!< Controls access to setting the number of buffers used for the stream.*/
{
	StreamBufferCountMode_Manual,/*!< The number of buffers used for the stream is set by the user.*/
	NUMSTREAMBUFFERCOUNTMODE
} spinTLStreamBufferCountModeEnums;

typedef enum _spinTLStreamBufferHandlingModeEnums	/*!< Available buffer handling modes of this data stream:*/
{
	StreamBufferHandlingMode_OldestFirst,/*!< The application always gets the buffer from the head of the output buffer queue (thus, the oldest available one). If the output buffer queue is empty, the application waits for a newly acquired buffer until the timeout expires.*/
	StreamBufferHandlingMode_OldestFirstOverwrite,/*!< The application always gets the buffer from the head of the output buffer queue (thus, the oldest available one). If the output buffer queue is empty, the application waits for a newly acquired buffer until the timeout expires. If a new buffer arrives it will overwrite the existing buffer from the head of the queue (behaves like a circular buffer).*/
	StreamBufferHandlingMode_NewestOnly,/*!< The application always gets the latest completed buffer (the newest one).  If the Output Buffer Queue is empty, the application waits for a newly acquired buffer until the timeout expires.  This buffer handling mode is typically used in a live display GUI where it is important that there is no lag between camera and display.*/
	StreamBufferHandlingMode_NewestFirst,/*!< The application always gets the buffer from the tail of the output buffer queue (thus, the newest available one). If the output buffer queue is empty, the application waits for a newly acquired buffer until the timeout expires.*/
	NUMSTREAMBUFFERHANDLINGMODE
} spinTLStreamBufferHandlingModeEnums;

typedef enum _spinTLDeviceTypeEnums	/*!< Transport layer type of the device.*/
{
	DeviceType_GigEVision,/*!< GigE Vision*/
	DeviceType_CameraLink,/*!< Camera Link*/
	DeviceType_CameraLinkHS,/*!< Camera Link High Speed*/
	DeviceType_CoaXPress,/*!< CoaXPress*/
	DeviceType_USB3Vision,/*!< USB3 Vision*/
	DeviceType_Custom,/*!< Custom transport layer*/
	NUMDEVICETYPE
} spinTLDeviceTypeEnums;

typedef enum _spinTLDeviceAccessStatusEnums	/*!< Gets the access status the transport layer Producer has on the device.*/
{
	DeviceAccessStatus_Unknown,/*!< Not known to producer.*/
	DeviceAccessStatus_ReadWrite,/*!< Full access*/
	DeviceAccessStatus_ReadOnly,/*!< Read-only access*/
	DeviceAccessStatus_NoAccess,/*!< Not available to connect*/
	DeviceAccessStatus_Busy,/*!< The device is already opened by another entity*/
	DeviceAccessStatus_OpenReadWrite,/*!< Open in Read/Write mode by this GenTL host*/
	DeviceAccessStatus_OpenReadOnly,/*!< Open in Read access mode by this GenTL host*/
	NUMDEVICEACCESSSTATUS
} spinTLDeviceAccessStatusEnums;

typedef enum _spinTLGenICamXMLLocationEnums	/*!< Sets the location to load GenICam XML.*/
{
	GenICamXMLLocation_Device,/*!< Load GenICam XML from device*/
	GenICamXMLLocation_Host,/*!< Load GenICam XML from host*/
	NUMGENICAMXMLLOCATION
} spinTLGenICamXMLLocationEnums;

typedef enum _spinTLGUIXMLLocationEnums	/*!< Sets the location to load GUI XML.*/
{
	GUIXMLLocation_Device,/*!< Load XML from device*/
	GUIXMLLocation_Host,/*!< Load XML from host*/
	NUMGUIXMLLOCATION
} spinTLGUIXMLLocationEnums;

typedef enum _spinTLGevCCPEnums	/*!< Controls the device access privilege of an application.*/
{
	GevCCP_EnumEntry_GevCCP_OpenAccess,/*!< Open access privilege.*/
	GevCCP_EnumEntry_GevCCP_ExclusiveAccess,/*!< Exclusive access privilege.*/
	GevCCP_EnumEntry_GevCCP_ControlAccess,/*!< Control access privilege.*/
	NUMGEVCCP
} spinTLGevCCPEnums;

typedef enum _spinTLDeviceEndianessMechanismEnums	/*!< Identifies the endianness handling mode.*/
{
	DeviceEndianessMechanism_Legacy,/*!< Handling the device endianness according to GenICam Schema 1.0*/
	DeviceEndianessMechanism_Standard,/*!< Handling the device endianness according to GenICam Schema 1.1 and later*/
	NUMDEVICEENDIANESSMECHANISM
} spinTLDeviceEndianessMechanismEnums;

typedef enum _spinTLDeviceCurrentSpeedEnums	/*!< The USB Speed that the device is currently operating at.*/
{
	DeviceCurrentSpeed_UnknownSpeed,/*!< Unknown-Speed.*/
	DeviceCurrentSpeed_LowSpeed,/*!< Low-Speed.*/
	DeviceCurrentSpeed_FullSpeed,/*!< Full-Speed.*/
	DeviceCurrentSpeed_HighSpeed,/*!< High-Speed.*/
	DeviceCurrentSpeed_SuperSpeed,/*!< Super-Speed.*/
	NUMDEVICECURRENTSPEED
} spinTLDeviceCurrentSpeedEnums;

typedef enum _spinTLInterfaceTypeEnums	/*!< Transport layer type of the interface.*/
{
	InterfaceType_GigEVision,/*!< GigE Vision*/
	InterfaceType_CameraLink,/*!< Camera Link*/
	InterfaceType_CameraLinkHS,/*!< Camera Link High Speed*/
	InterfaceType_CoaXPress,/*!< CoaXPress*/
	InterfaceType_USB3Vision,/*!< USB3 Vision*/
	InterfaceType_Custom,/*!< Custom transport layer*/
	NUMINTERFACETYPE
} spinTLInterfaceTypeEnums;

typedef enum _spinTLPOEStatusEnums	/*!< Reports and controls the interface's power over Ethernet status.*/
{
	POEStatus_NotSupported,/*!< Not Supported*/
	POEStatus_PowerOff,/*!< Power is Off*/
	POEStatus_PowerOn,/*!< Power is On*/
	NUMPOESTATUS
} spinTLPOEStatusEnums;

typedef enum _spinTLFLIRFilterDriverStatusEnums	/*!< Reports whether FLIR Light Weight Filter Driver is enabled, disabled, or not installed.*/
{
	FLIRFilterDriverStatus_NotSupported,/*!< Not Installed*/
	FLIRFilterDriverStatus_Disabled,/*!< FLIR Light Weight Filter Driver is disabled across all interfaces*/
	FLIRFilterDriverStatus_Enabled,/*!< FLIR Light Weight Filter Driver is enabled*/
	NUMFLIRFILTERDRIVERSTATUS
} spinTLFLIRFilterDriverStatusEnums;

typedef enum _spinTLTeledyneGigeVisionFilterDriverStatusEnums	/*!< Reports whether Teledyne Gige Vision Filter Driver is enabled, disabled, or not installed.*/
{
	TeledyneGigeVisionFilterDriverStatus_NotSupported,/*!< Not Installed*/
	TeledyneGigeVisionFilterDriverStatus_Disabled,/*!< Teledyne Gige Vision Filter Driver is disabled across all interfaces*/
	TeledyneGigeVisionFilterDriverStatus_Enabled,/*!< Teledyne Gige Vision Filter Driver is enabled*/
	NUMTELEDYNEGIGEVISIONFILTERDRIVERSTATUS
} spinTLTeledyneGigeVisionFilterDriverStatusEnums;

typedef enum _spinTLTLTypeEnums	/*!< Transport layer type of the GenTL Producer implementation.*/
{
	TLType_GigEVision,/*!< GigE Vision*/
	TLType_CameraLink,/*!< Camera Link*/
	TLType_CameraLinkHS,/*!< Camera Link High Speed*/
	TLType_CoaXPress,/*!< CoaXPress*/
	TLType_USB3Vision,/*!< USB3 Vision*/
	TLType_Mixed,/*!< Different Interface modules of the GenTL Producer are of different types*/
	TLType_Custom,/*!< Custom transport layer*/
	NUMTLTYPE
} spinTLTLTypeEnums;

/**@}*/

/**@}*/

#endif // FLIR_SPINNAKER_TRANSPORTLAYERDEFSC_H