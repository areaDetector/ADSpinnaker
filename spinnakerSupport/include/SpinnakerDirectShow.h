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

#ifndef FLIR_SPINNAKER_DIRECTSHOW_H
#define FLIR_SPINNAKER_DIRECTSHOW_H

#include "Spinnaker.h"

const unsigned int MAX_LENGTH = 256;

/**
 *  @defgroup SpinnakerHeaders Spinnaker Headers
 */

/*@{*/

/**
 *  @defgroup SpinnakerDirectShow_h SpinnakerDirectShow.h
 *
 *  Global header file for Spinnaker DirectShow library.
 *
 *  By including this file, all required header files for full Spinnaker
 *  DirectShow operation will be included automatically. It is recommended that
 *  this file be used instead of manually including individual header files.
 *
 *  We welcome your bug reports, suggestions, and comments:
 *  https://www.flir.com/support-center/rma/iis-support-request
 */

/*@{*/

/**
 * This is the Interface that allows users to get and set device node
 * properties on the camera. You can query the SpinnakerDirectShow Capture Filter
 * for IID_ISpinnakerInterface and it will return a pointer to the ISpinnakerInterface interface.
 * The GUID is equivalent to {5143AD36-51D1-422D-9B17-81DD3DB0600E}.
 */

static const GUID IID_ISpinnakerInterface = {0x5143ad36,
                                             0x51d1,
                                             0x422d,
                                             {0x9b, 0x17, 0x81, 0xdd, 0x3d, 0xb0, 0x60, 0xe}};

interface ISpinDevice
{
  public:
    /**
     * @name Spinnaker Interface and Camera Functions
     *
     * These functions deal with querying/setting generic interface and camera info.
     */
    /*@{*/

    /**
     * Retrieves the number of cameras available for this Spinnaker DirectShow interface.
     *
     * @param numCameras An integer that represents the number of cameras discovered
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(GetNumCameras)(unsigned int* numCameras) = 0;

    /**
     * Retrieves an integer index to the currently selected camera.
     *
     * @param selectedIndex An integer that represents the index of the selected camera
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(GetSelectedCameraIndex)(unsigned int* selectedIndex) = 0;

    /**
     * Sets the currently selected camera to the index specified. This function
     * will do nothing if the selected index is out of range.
     *
     * @param index The index at which to retrieve the camera object
     * @param needsRelease Specifies whether to release the previously selected camera object
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(SetSelectedCameraIndex)(unsigned int index, bool needsRelease = true) = 0;

    /**
     * Retrieves general information about the camera at the specified index.
     *
     * @param index The index at which to retrieve the camera information
     * @param model Model description of the camera
     * @param type Transport layer description of the camera
     * @param serial Serial number of the camera
     * @param bufferSize Size of the individual provided model, sensor, serial buffer in bytes
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(GetCameraInfo)(unsigned int index, char* model, char* type, char* serial, size_t bufferSize) = 0;

    /**
     * Checks if the selected camera is currently streaming
     *
     * @param isStreaming Flag indicating whether camera is streaming or not
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(IsStreaming)(bool* isStreaming) = 0;

    /**
     * Starts the image acquisition engine for the currently selected camera. Nothing occurs
     * if the camera is already streaming.
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(BeginAcquisition)() = 0;

    /**
     * Stops the image acquisition engine for the currently selected camera. Nothing occurs if
     * there were no prior call to BeginAcquisition(). Note that EndAcquisition() needs to be
     * called before selecting a new camera.
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(EndAcquisition)() = 0;

    /*@}*/

    /**
     * @name Spinnaker GenAPI Functions
     *
     * These functions deal with Spinnaker GenAPI NodeMap and Node Accesses for
     * setting camera properties.
     */
    /*@{*/

    /**
     * Retrieves the number of nodes available in the node map for the currently selected camera.
     *
     * @param numNodes Number of nodes available for the currently selected device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(NodeMapGetNumNodes)(size_t * numNodes) = 0;

    /**
     * Retrieves the string representation of the node at the specified index
     *
     * @param index Node index in the NodeMap
     * @param nodeName GenICam node name at specified index to be populated
     * @param bufferSize Size of the provided nodeName buffer in bytes
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(NodeMapGetNodeAtIndex)(size_t index, char* nodeName, size_t bufferSize) = 0;

    /**
     * Checks if a node is implemented
     *
     * @param nodeName GenICam device feature name
     * @param isImplemented Flag indicating whether node is implemented or not
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(NodeIsImplemented)(const char* nodeName, bool* isImplemented) = 0;

    /**
     * Checks if a node is available
     *
     * @param nodeName GenICam device feature name
     * @param isAvailable Flag indicating whether node is available or not
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(NodeIsAvailable)(const char* nodeName, bool* isAvailable) = 0;

    /**
     * Checks if a node is readable
     *
     * @param nodeName GenICam device feature name
     * @param isReadable Flag indicating whether node is readable or not
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(NodeIsReadable)(const char* nodeName, bool* isReadable) = 0;

    /**
     * Checks if a node is writable
     *
     * @param nodeName GenICam device feature name
     * @param isWritable Flag indicating whether node is writable or not
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(NodeIsWritable)(const char* nodeName, bool* isWritable) = 0;

    /**
     * Gets the type of the node
     *
     * @param nodeName GenICam device feature name
     * @param typeName The type of the node. Available type names are:
     *                 Integer, Float, Boolean, String, Enumeration, Entry, Command and Other
     * @param bufferSize Size of the provided typeName buffer in bytes
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(NodeGetType)(const char* nodeName, char* typeName, size_t bufferSize) = 0;

    /**
     * Gets content of the node as string
     *
     * @param nodeName GenICam device feature name
     * @param valueAsString String representation of the node value
     * @param bufferSize Size of the provided valueAsString buffer in bytes
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(NodeToString)(const char* nodeName, char* valueAsString, size_t bufferSize) = 0;

    /**
     * Gets a name string for display
     *
     * @param nodeName GenICam device feature name
     * @param displayName Display name of the device feature
     * @param bufferSize Size of the provided displayName buffer in bytes
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(NodeGetDisplayName)(const char* nodeName, char* displayName, size_t bufferSize) = 0;

    /**
     * @name Integer Node Functions
     *
     * These functions deal with Spinnaker GenAPI Integer Node Accesses
     */
    /*@{*/

    /**
     * Gets integer node value for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param value The value read from the device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(IntegerGetValue)(const char* nodeName, int64_t* value) = 0;

    /**
     * Sets integer node value for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param value The value to set to device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(IntegerSetValue)(const char* nodeName, int64_t value) = 0;

    /**
     * Gets maximum integer value allowed for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param maxValue The maximum allowed value read from the device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(IntegerGetMax)(const char* nodeName, int64_t* maxValue) = 0;

    /**
     * Gets minimum integer value allowed for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param minValue The minimum allowed value read from the device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(IntegerGetMin)(const char* nodeName, int64_t* minValue) = 0;

    /**
     * Gets integer increment mode for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param incMode Spinnaker GenAPI increment mode for the specified feature
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(IntegerGetIncMode)(const char* nodeName, Spinnaker::GenApi::EIncMode* incMode) = 0;

    /**
     * Gets integer step increment for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param increment The step increment value read from the device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(IntegerGetInc)(const char* nodeName, int64_t* increment) = 0;

    /*@}*/

    /**
     * @name Float Node Functions
     *
     * These functions deal with Spinnaker GenAPI Float Node Accesses
     */
    /*@{*/

    /**
     * Gets float node value for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param value The value read from the device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(FloatGetValue)(const char* nodeName, double* value) = 0;

    /**
     * Sets float node value for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param value The value to set to device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(FloatSetValue)(const char* nodeName, double value) = 0;

    /**
     * Gets maximum float value allowed for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param floatMax The maximum allowed value read from the device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(FloatGetMax)(const char* nodeName, double* floatMax) = 0;

    /**
     * Gets minimum float value allowed for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param floatMin The minimum allowed value read from the device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(FloatGetMin)(const char* nodeName, double* floatMin) = 0;

    /**
     * Gets float increment mode for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param incMode Spinnaker GenAPI increment mode for the specified feature
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(FloatGetIncMode)(const char* nodeName, Spinnaker::GenApi::EIncMode* incMode) = 0;

    /**
     * Gets float step increment for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param increment The step increment value read from the device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(FloatGetInc)(const char* nodeName, double* increment) = 0;

    /*@}*/

    /**
     * @name Boolean Node Functions
     *
     * These functions deal with Spinnaker GenAPI Boolean Node Accesses
     */
    /*@{*/

    /**
     * Gets boolean node value for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param value The value read from the device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(BooleanGetValue)(const char* nodeName, bool* value) = 0;

    /**
     * Sets boolean node value for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param value The value to set to device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(BooleanSetValue)(const char* nodeName, bool value) = 0;

    /*@}*/

    /**
     * @name String Node Functions
     *
     * These functions deal with Spinnaker GenAPI String Node Accesses
     */
    /*@{*/

    /**
     * Gets string node value for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param value The value read from the device
     * @param bufferSize Size of the provided value buffer in bytes
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(StringGetValue)(const char* nodeName, char* value, size_t bufferSize) = 0;

    /**
     * Sets string node value for the specified feature
     *
     * @param nodeName GenICam device feature name
     * @param value The value to set to device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(StringSetValue)(const char* nodeName, const char* value) = 0;

    /*@}*/

    /**
     * @name Enumeration Node Functions
     *
     * These functions deal with Spinnaker GenAPI Enumeration and EnumeratEntry Node Accesses
     */
    /*@{*/

    /**
     * Gets enumeration entry string for the specified enumeration feature
     *
     * @param nodeName GenICam device feature name
     * @param value The enumeration entry string read from the device
     * @param bufferSize Size of the provided value buffer in bytes
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(EnumerationGetEntry)(const char* nodeName, char* value, size_t bufferSize) = 0;

    /**
     * Sets enumeration entry for the specified enumeration feature
     *
     * @param nodeName GenICam device feature name
     * @param value The enumeration entry string to set to device
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(EnumerationSetEntry)(const char* nodeName, const char* value) = 0;

    /**
     * Checks if an enumeration entry exists for the specified enumeration feature
     *
     * @param enumerationName GenICam device enumeration feature name
     * @param enumerationEntryName GenICam device enumeration entry name to check
     * @param entryExist Flag indicating whether the specified entry node exists for the enumeration node
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(EnumerationEntryExists)
    (const char* enumerationName, const char* enumerationEntryName, bool* entryExist) = 0;

    /**
     * Retrieves the number of enumeration entry nodes available for the specified enumeration feature
     *
     * @param enumerationName GenICam device enumeration feature name
     * @param numEntries Number of nodes available for the specified enumeration feature
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(EnumerationGetNumEntries)(const char* enumerationName, size_t* numEntries) = 0;

    /**
     * Gets enumeration entry string for the specified enumeration feature at the specified index
     *
     * @param enumerationName GenICam device enumeration feature name
     * @param entryIndex Enumeration entry index
     * @param enumerationEntryName GenICam device enumeration entry name at specified entryIndex to be populated
     * @param bufferSize Size of the provided enumerationEntryName buffer in bytes
     *
     * @return An HRESULT indicating the success or failure of the function.
     */
    STDMETHOD(EnumerationGetEntryAtIndex)
    (const char* enumerationName, unsigned int entryIndex, char* enumerationEntryName, size_t bufferSize) = 0;

    /*@}*/

    /**
     * @name Command Node Functions
     *
     * These functions deal with Spinnaker GenAPI Command Node Accesses
     */
    /*@{*/

    /**
     * Executes the command for the specified feature
     *
     * @param nodeName GenICam device feature name
     *
     * @return An HRESULT error code indicating the success or failure of the function.
     */
    STDMETHOD(CommandExecute)(const char* nodeName) = 0;

    /*@}*/

  protected:
    /**
     *  @cond This section is used internally by SpinnakerDirectShow and is not intended for public use
     */

    friend class SpinSourceStream;

    STDMETHOD(FillBuffer)(IMediaSample * pms, Spinnaker::PixelFormatEnums pixelFormat) = 0;

    /* @endcond */
};

interface ISpinnakerInterface : public IUnknown
{
    /**
     * Returns a ISpinDevice pointer to the currently selected spinnaker device
     *
     * @return ISpinDevice pointer to the currently selected device
     */
    virtual ISpinDevice* GetDevice() = 0;
};

    /*@}*/

    /*@}*/

#endif // FLIR_SPINNAKER_DIRECTSHOW_H