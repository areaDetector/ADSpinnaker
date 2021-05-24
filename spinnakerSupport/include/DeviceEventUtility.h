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

#ifndef FLIR_SPINNAKER_DEVICE_EVENT_UTILITY_H
#define FLIR_SPINNAKER_DEVICE_EVENT_UTILITY_H

#include "SpinnakerDefs.h"

namespace Spinnaker
{
    class SPINNAKER_API DeviceEventUtility
    {
      public:
        /**
         * Parse the EventInference device event payload data
         *
         * @param[in] payloadData Event payload data
         * @param[in] payloadSize Event payload data size
         * @param[out] eventData Parsed EventInference payload data
         *
         * @see DeviceEventInferenceData
         */
        static void ParseDeviceEventInference(
            const uint8_t* payloadData,
            const size_t payloadSize,
            DeviceEventInferenceData& eventData);

        /**
         * Parse the EventExposureEnd device event payload data
         *
         * @param[in] payloadData Event payload data
         * @param[in] payloadSize Event payload data size
         * @param[out] eventData Parsed ExposureEnd payload data returned by reference
         *
         * @see DeviceEventExposureEndData
         */
        static void ParseDeviceEventExposureEnd(
            const uint8_t* payloadData,
            const size_t payloadSize,
            DeviceEventExposureEndData& eventData);
    };

    /*@}*/

    /*@}*/
} // namespace Spinnaker

#endif // FLIR_SPINNAKER_DEVICE_EVENT_UTILITY_H
