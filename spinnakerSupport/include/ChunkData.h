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

#ifndef FLIR_SPINNAKER_CHUNKDATA_H
#define FLIR_SPINNAKER_CHUNKDATA_H

#include "Interface/IChunkData.h"

namespace Spinnaker
{
    /**
    * @defgroup SpinnakerClasses Spinnaker Classes
    */
    /**@{*/

    /**
    * @defgroup ChunkData_h ChunkData Class
    */
    /**@{*/

    /**
    *@brief The chunk data which contains additional information about an image.
    */

    class SPINNAKER_API ChunkData: public IChunkData
    {
    public:
        ChunkData();

        ChunkData(const ChunkData& src);

        virtual ~ChunkData(void);

        void SetChunks(GenApi::INodeMap & pNodeMap);

        /**
         * Description: Returns the black level used to capture the image.
         * Visibility: 
         */
        float64_t GetBlackLevel() const;

        /**
         * Description: Returns the image frame ID.
         * Visibility: 
         */
        int64_t GetFrameID() const;

        /**
         * Description: Returns the exposure time used to capture the image.
         * Visibility: 
         */
        float64_t GetExposureTime() const;

        /**
         * Description: Returns the compression mode of the last image payload.
         * Visibility: 
         */
        int64_t GetCompressionMode() const;

        /**
         * Description: Returns the compression ratio of the last image payload.
         * Visibility: 
         */
        float64_t GetCompressionRatio() const;

        /**
         * Description: Returns the Timestamp of the image.
         * Visibility: 
         */
        int64_t GetTimestamp() const;

        /**
         * Description: Returns the status of all the I/O lines at the end of exposure event.
         * Visibility: 
         */
        int64_t GetExposureEndLineStatusAll() const;

        /**
         * Description: Returns the width of the image included in the payload.
         * Visibility: 
         */
        int64_t GetWidth() const;

        /**
         * Description: Returns the image payload.
         * Visibility: 
         */
        int64_t GetImage() const;

        /**
         * Description: Returns the height of the image included in the payload.
         * Visibility: 
         */
        int64_t GetHeight() const;

        /**
         * Description: Returns the gain used to capture the image.
         * Visibility: 
         */
        float64_t GetGain() const;

        /**
         * Description: Returns the index of the active set of the running sequencer included in the payload.
         * Visibility: 
         */
        int64_t GetSequencerSetActive() const;

        /**
         * Description: Returns the CRC of the image payload.
         * Visibility: 
         */
        int64_t GetCRC() const;

        /**
         * Description: Returns the Offset X of the image included in the payload.
         * Visibility: 
         */
        int64_t GetOffsetX() const;

        /**
         * Description: Returns the Offset Y of the image included in the payload.
         * Visibility: 
         */
        int64_t GetOffsetY() const;

        /**
         * Description: Returns the length of the received serial data that was included in the payload.
         * Visibility: 
         */
        int64_t GetSerialDataLength() const;

        /**
         * Description: Selects the part to access in chunk data in a multipart transmission.
         * Visibility: Expert
         */
        int64_t GetPartSelector() const;

        /**
         * Description: Returns the minimum value of dynamic range of the image included in the payload.
         * Visibility: Expert
         */
        int64_t GetPixelDynamicRangeMin() const;

        /**
         * Description: Returns the maximum value of dynamic range of the image included in the payload.
         * Visibility: Expert
         */
        int64_t GetPixelDynamicRangeMax() const;

        /**
         * Description: Returns the last Timestamp latched with the TimestampLatch command.
         * Visibility: Expert
         */
        int64_t GetTimestampLatchValue() const;

        /**
         * Description: Returns the status of all the I/O lines at the time of the FrameStart internal event.
         * Visibility: Expert
         */
        int64_t GetLineStatusAll() const;

        /**
         * Description: Returns the value of the selected Chunk counter at the time of the FrameStart event.
         * Visibility: Expert
         */
        int64_t GetCounterValue() const;

        /**
         * Description: Returns the value of the selected Timer at the time of the FrameStart internal event.
         * Visibility: Expert
         */
        float64_t GetTimerValue() const;

        /**
         * Description: Index for vector representation of one chunk value per line in an image.
         * Visibility: Expert
         */
        int64_t GetScanLineSelector() const;

        /**
         * Description: Returns the counter's value of the selected Encoder at the time of the FrameStart in area scan mode or the counter's value at the time of the LineStart selected by ChunkScanLineSelector in LineScan mode.
         * Visibility: Expert
         */
        int64_t GetEncoderValue() const;

        /**
         * Description: Returns the LinePitch of the image included in the payload.
         * Visibility: Expert
         */
        int64_t GetLinePitch() const;

        /**
         * Description: Returns the unique identifier of the transfer block used to transport the payload.
         * Visibility: Expert
         */
        int64_t GetTransferBlockID() const;

        /**
         * Description: Returns the current number of blocks in the transfer queue.
         * Visibility: Expert
         */
        int64_t GetTransferQueueCurrentBlockCount() const;

        /**
         * Description: Returns identifier of the stream channel used to carry the block.
         * Visibility: Expert
         */
        int64_t GetStreamChannelID() const;

        /**
         * Description: Returns the Scale for the selected coordinate axis of the image included in the payload.
         * Visibility: Expert
         */
        float64_t GetScan3dCoordinateScale() const;

        /**
         * Description: Returns the Offset for the selected coordinate axis of the image included in the payload.
         * Visibility: Expert
         */
        float64_t GetScan3dCoordinateOffset() const;

        /**
         * Description: Returns the Invalid Data Value used for the image included in the payload.
         * Visibility: Expert
         */
        float64_t GetScan3dInvalidDataValue() const;

        /**
         * Description: Returns the Minimum Axis value for the selected coordinate axis of the image included in the payload.
         * Visibility: Expert
         */
        float64_t GetScan3dAxisMin() const;

        /**
         * Description: Returns the Maximum Axis value for the selected coordinate axis of the image included in the payload.
         * Visibility: Expert
         */
        float64_t GetScan3dAxisMax() const;

        /**
         * Description: Returns the transform value.
         * Visibility: Expert
         */
        float64_t GetScan3dTransformValue() const;

        /**
         * Description: Reads the value of a position or pose coordinate for the anchor or transformed coordinate systems relative to the reference point.
         * Visibility: Expert
         */
        float64_t GetScan3dCoordinateReferenceValue() const;

        /**
         * Description: Returns the frame ID associated with the most recent inference result.
         * Visibility: Expert
         */
        int64_t GetInferenceFrameId() const;

        /**
         * Description: Returns the chunk data inference result.
         * Visibility: Expert
         */
        int64_t GetInferenceResult() const;

        /**
         * Description: Returns the chunk data inference confidence percentage.
         * Visibility: Expert
         */
        float64_t GetInferenceConfidence() const;

        /**
         * Description: Returns the chunk inference bounding box result data.
         * Visibility: Expert
         */
        InferenceBoundingBoxResult GetInferenceBoundingBoxResult() const;

        friend class ChunkDataImpl;

    };
    /**@}*/

    /**@}*/

}
#endif // FLIR_SPINNAKER_CHUNKDATA_H