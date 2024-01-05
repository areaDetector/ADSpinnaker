//=============================================================================
// Copyright (c) 2001-2022 FLIR Systems, Inc. All Rights Reserved.
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

#ifndef FLIR_SPINNAKER_DEFS_H
#define FLIR_SPINNAKER_DEFS_H

#include <memory.h>
#include <stdint.h>
#include <string>
#include "SpinnakerPlatform.h"

namespace Spinnaker
{
    /**
     *  @defgroup SpinnakerHeaders Spinnaker Headers
     */

    /*@{*/

    /**
     * @defgroup SpinnakerDefs Spinnaker Definitions
     *
     * Definitions file for Spinnaker.
     */

    /**
     * Timeout values for getting next image, device, or interface event
     */
    const uint64_t EVENT_TIMEOUT_NONE = 0;                      // Do not wait.  GetNextImage will return immediately.
    const uint64_t EVENT_TIMEOUT_INFINITE = 0xFFFFFFFFFFFFFFFF; // Never timeout.  GetNextImage will wait indefinitely.

    /*@{*/

    /**
     * @brief Spinnaker enum definitions.
     */

    /**
     * The error codes used in Spinnaker.  These codes are returned as part of
     * Spinnaker::Exception.  The error codes in the range of -1000 to -1999
     * are reserved for exceptions that map directly to GenTL values.
     * The error codes in the range of -2000 to -2999 are reserved
     * for GenICam related errors.  The error codes in the range of -3000 to -3999
     * are reserved for image processing related errors.
     */
    enum Error
    {
        SPINNAKER_ERR_SUCCESS = 0,
        SPINNAKER_ERR_ERROR = -1001,
        SPINNAKER_ERR_NOT_INITIALIZED = -1002,
        SPINNAKER_ERR_NOT_IMPLEMENTED = -1003,
        SPINNAKER_ERR_RESOURCE_IN_USE = -1004,
        SPINNAKER_ERR_ACCESS_DENIED = -1005,
        SPINNAKER_ERR_INVALID_HANDLE = -1006,
        SPINNAKER_ERR_INVALID_ID = -1007,
        SPINNAKER_ERR_NO_DATA = -1008,
        SPINNAKER_ERR_INVALID_PARAMETER = -1009,
        SPINNAKER_ERR_IO = -1010,
        SPINNAKER_ERR_TIMEOUT = -1011,
        SPINNAKER_ERR_ABORT = -1012,
        SPINNAKER_ERR_INVALID_BUFFER = -1013,
        SPINNAKER_ERR_NOT_AVAILABLE = -1014,
        SPINNAKER_ERR_INVALID_ADDRESS = -1015,
        SPINNAKER_ERR_BUFFER_TOO_SMALL = -1016,
        SPINNAKER_ERR_INVALID_INDEX = -1017,
        SPINNAKER_ERR_PARSING_CHUNK_DATA = -1018,
        SPINNAKER_ERR_INVALID_VALUE = -1019,
        SPINNAKER_ERR_RESOURCE_EXHAUSTED = -1020,
        SPINNAKER_ERR_OUT_OF_MEMORY = -1021,
        SPINNAKER_ERR_BUSY = -1022,
        SPINNAKER_ERR_IP_CONFLICT = -1023,

        SPINNAKER_ERR_GENICAM_INVALID_ARGUMENT = -2001,
        SPINNAKER_ERR_GENICAM_OUT_OF_RANGE = -2002,
        SPINNAKER_ERR_GENICAM_PROPERTY = -2003,
        SPINNAKER_ERR_GENICAM_RUN_TIME = -2004,
        SPINNAKER_ERR_GENICAM_LOGICAL = -2005,
        SPINNAKER_ERR_GENICAM_ACCESS = -2006,
        SPINNAKER_ERR_GENICAM_TIMEOUT = -2007,
        SPINNAKER_ERR_GENICAM_DYNAMIC_CAST = -2008,
        SPINNAKER_ERR_GENICAM_GENERIC = -2009,
        SPINNAKER_ERR_GENICAM_BAD_ALLOCATION = -2010,

        SPINNAKER_ERR_IM_CONVERT = -3001,
        SPINNAKER_ERR_IM_COPY = -3002,
        SPINNAKER_ERR_IM_MALLOC = -3003,
        SPINNAKER_ERR_IM_NOT_SUPPORTED = -3004,
        SPINNAKER_ERR_IM_HISTOGRAM_RANGE = -3005,
        SPINNAKER_ERR_IM_HISTOGRAM_MEAN = -3006,
        SPINNAKER_ERR_IM_MIN_MAX = -3007,
        SPINNAKER_ERR_IM_COLOR_CONVERSION = -3008,
        SPINNAKER_ERR_IM_DECOMPRESSION = -3009,

        SPINNAKER_ERR_CUSTOM_ID = -10000
    };

    /**
     * Event types in Spinnaker.
     *
     * @see EventHandler::GetEventType()
     */
    enum EventType
    {
        SPINNAKER_EVENT_ARRIVAL_REMOVAL, /* Specifically, the device arrival/removal event */
        SPINNAKER_EVENT_DEVICE,
        SPINNAKER_EVENT_DEVICE_SPECIFIC,
        SPINNAKER_EVENT_NEW_BUFFER,
        SPINNAKER_EVENT_NEW_BUFFER_LIST,
        SPINNAKER_EVENT_LOGGING_EVENT,
        SPINNAKER_EVENT_INTERFACE_ARRIVAL_REMOVAL,
        SPINNAKER_EVENT_UNKNOWN,
    };

    /**
     * This enum represents the namespace in which the TL specific pixel format
     * resides.  This enum is returned from a captured image when calling
     * Image::GetTLPixelFormatNamespace().  It can be used to interpret the raw
     * pixel format returned from Image::GetTLPixelFormat().
     *
     * @see Image::GetTLPixelFormat()
     *
     * @see Image::GetTLPixelFormatNamespace()
     */
    enum TLPixelFormatNamespace
    {
        SPINNAKER_TLPIXELFORMAT_NAMESPACE_UNKNOWN = 0,    /* GenTL v1.2 */
        SPINNAKER_TLPIXELFORMAT_NAMESPACE_GEV = 1,        /* GenTL v1.2 */
        SPINNAKER_TLPIXELFORMAT_NAMESPACE_IIDC = 2,       /* GenTL v1.2 */
        SPINNAKER_TLPIXELFORMAT_NAMESPACE_PFNC_16BIT = 3, /* GenTL v1.4 */
        SPINNAKER_TLPIXELFORMAT_NAMESPACE_PFNC_32BIT = 4, /* GenTL v1.4 */

        SPINNAKER_TLPIXELFORMAT_NAMESPACE_CUSTOM_ID = 1000
    };

    /**
     * Color processing algorithms. Please refer to our knowledge base at
     * article at
     * https://www.flir.com/support-center/iis/machine-vision/knowledge-base/different-color-processing-algorithms-in-flycapture2
     * for complete details for each algorithm.
     */
    enum ColorProcessingAlgorithm
    {
        /** No color processing. */
        SPINNAKER_COLOR_PROCESSING_ALGORITHM_NONE,
        /**
         * Fastest but lowest quality. Equivalent to
         * FLYCAPTURE_NEAREST_NEIGHBOR_FAST in FlyCapture.
         */
        SPINNAKER_COLOR_PROCESSING_ALGORITHM_NEAREST_NEIGHBOR,
        /**
         * Nearest Neighbor with averaged green pixels. Higher quality but slower
         * compared to nearest neighbor without averaging.
         */
        SPINNAKER_COLOR_PROCESSING_ALGORITHM_NEAREST_NEIGHBOR_AVG,
        /** Weighted average of surrounding 4 pixels in a 2x2 neighborhood. */
        SPINNAKER_COLOR_PROCESSING_ALGORITHM_BILINEAR,
        /** Weights surrounding pixels based on localized edge orientation. */
        SPINNAKER_COLOR_PROCESSING_ALGORITHM_EDGE_SENSING,
        /** Well-balanced speed and quality. */
        SPINNAKER_COLOR_PROCESSING_ALGORITHM_HQ_LINEAR,
        /** Multi-threaded with similar results to edge sensing. */
        SPINNAKER_COLOR_PROCESSING_ALGORITHM_IPP,
        /** Best quality but much faster than rigorous. More memory intensive than other color processing algorithms. */
        SPINNAKER_COLOR_PROCESSING_ALGORITHM_DIRECTIONAL_FILTER,
        /** Slowest but produces good results. */
        SPINNAKER_COLOR_PROCESSING_ALGORITHM_RIGOROUS,
        /** Weighted pixel average from different directions. */
        SPINNAKER_COLOR_PROCESSING_ALGORITHM_WEIGHTED_DIRECTIONAL_FILTER
    };

    /** File formats to be used for saving images to disk. */
    enum ImageFileFormat
    {
        SPINNAKER_IMAGE_FILE_FORMAT_FROM_FILE_EXT = -1, /**< Determine file format from file extension. */
        SPINNAKER_IMAGE_FILE_FORMAT_PGM,                /**< Portable gray map. */
        SPINNAKER_IMAGE_FILE_FORMAT_PPM,                /**< Portable pixmap. */
        SPINNAKER_IMAGE_FILE_FORMAT_BMP,                /**< Bitmap. */
        SPINNAKER_IMAGE_FILE_FORMAT_JPEG,               /**< JPEG. */
        SPINNAKER_IMAGE_FILE_FORMAT_JPEG2000,           /**< JPEG 2000. */
        SPINNAKER_IMAGE_FILE_FORMAT_TIFF,               /**< Tagged image file format. */
        SPINNAKER_IMAGE_FILE_FORMAT_PNG,                /**< Portable network graphics. */
        SPINNAKER_IMAGE_FILE_FORMAT_RAW,                /**< Raw data. */
        SPINNAKER_IMAGE_FILE_FORMAT_JPEG12_C,           /**< 12 bit compressed JPEG data. */
        SPINNAKER_IMAGE_FILE_FORMAT_FORCE_32BITS = 0x7FFFFFFF
    };

    /** Status of images returned from GetNextImage() or Convert() calls. */
    enum ImageStatus
    {
        SPINNAKER_IMAGE_STATUS_UNKNOWN_ERROR = -1, /**< Image has an unknown error. */
        SPINNAKER_IMAGE_STATUS_NO_ERROR =
            0, /**< Image is returned from GetNextImage() or Convert() calls without any errors. */
        SPINNAKER_IMAGE_STATUS_CRC_CHECK_FAILED = 1, /**< Image failed CRC check. */
        SPINNAKER_IMAGE_STATUS_DATA_OVERFLOW = 2,    /**< Received more data than the size of the image. */
        SPINNAKER_IMAGE_STATUS_MISSING_PACKETS =
            3, /**< Image has missing packets. Potential fixes include enabling
               jumbo packets and adjusting packet size/delay. For more information see
               https://www.flir.com/support-center/iis/machine-vision/application-note/troubleshooting-image-consistency-errors/
             */
        SPINNAKER_IMAGE_STATUS_LEADER_BUFFER_SIZE_INCONSISTENT =
            4, /**< Image leader is incomplete. Could be caused by missing packet(s). See link above.*/
        SPINNAKER_IMAGE_STATUS_TRAILER_BUFFER_SIZE_INCONSISTENT =
            5, /**< Image trailer is incomplete. Could be caused by missing packet(s). See link above.*/
        SPINNAKER_IMAGE_STATUS_PACKETID_INCONSISTENT =
            6, /**< Image has an inconsistent packet id. Could be caused by missing packet(s). See link above.*/
        SPINNAKER_IMAGE_STATUS_MISSING_LEADER =
            7, /**< Image leader is missing. Could be caused by missing packet(s). See link above.*/
        SPINNAKER_IMAGE_STATUS_MISSING_TRAILER =
            8, /**< Image trailer is missing. Could be caused by missing packet(s). See link above.*/
        SPINNAKER_IMAGE_STATUS_DATA_INCOMPLETE = 9, /**< Image data is incomplete. Could be caused by missing packet(s)
                                      or decompression error. See link above.*/
        SPINNAKER_IMAGE_STATUS_INFO_INCONSISTENT =
            10, /**< Image info is corrupted. Could be caused by missing packet(s). See link above.*/
        SPINNAKER_IMAGE_STATUS_CHUNK_DATA_INVALID = 11, /**< Image chunk data is invalid */
        SPINNAKER_IMAGE_STATUS_NO_SYSTEM_RESOURCES = 12 /**< Image cannot be processed due to lack of system
                                       resources. */
    };

    /**
     * Channels that allow statistics to be calculated.
     */
    enum StatisticsChannel
    {
        SPINNAKER_STATISTICS_CHANNEL_GREY,
        SPINNAKER_STATISTICS_CHANNEL_RED,
        SPINNAKER_STATISTICS_CHANNEL_GREEN,
        SPINNAKER_STATISTICS_CHANNEL_BLUE,
        SPINNAKER_STATISTICS_CHANNEL_HUE,
        SPINNAKER_STATISTICS_CHANNEL_SATURATION,
        SPINNAKER_STATISTICS_CHANNEL_LIGHTNESS,
        SPINNAKER_STATISTICS_CHANNEL_NUM_CHANNELS
    };

    /** Log levels */
    enum SpinnakerLogLevel
    {
        SPINNAKER_LOG_LEVEL_OFF = -1,    // Logging is off.
        SPINNAKER_LOG_LEVEL_FATAL = 0,   // Failures that are non-recoverable without user intervention.
        SPINNAKER_LOG_LEVEL_ALERT = 100, // Not used by Spinnaker.
        SPINNAKER_LOG_LEVEL_CRIT = 200,  // Not used by Spinnaker.
        SPINNAKER_LOG_LEVEL_ERROR = 300, // Failures that may or may not be recoverable without user
                                         // intervention (use case dependent).
        SPINNAKER_LOG_LEVEL_WARN = 400,  // Failures that are recoverable without user intervention.
        SPINNAKER_LOG_LEVEL_NOTICE =
            500, // Events such as camera arrival and removal, initialization and deinitialization,
                 // starting and stopping image acquisition, and feature modification.
        SPINNAKER_LOG_LEVEL_INFO = 600,  // Information about recurring events that are generated regularly such as
                                         // information on individual images.
        SPINNAKER_LOG_LEVEL_DEBUG = 700, // Information that can be used to troubleshoot the system.
        SPINNAKER_LOG_LEVEL_NOTSET = 800 // Logs everything.
    };

    /* Image payload types supported by the Spinnaker Image class*/
    enum ImagePayloadType
    {
        SPINNAKER_IMAGE_PAYLOAD_TYPE_UNKNOWN = -1,
        SPINNAKER_IMAGE_PAYLOAD_TYPE_IMAGE,
        SPINNAKER_IMAGE_PAYLOAD_TYPE_EXTENDED_CHUNK,
        SPINNAKER_IMAGE_PAYLOAD_TYPE_JPEG,
        SPINNAKER_IMAGE_PAYLOAD_TYPE_LOSSLESS_COMPRESSED,
        SPINNAKER_IMAGE_PAYLOAD_TYPE_LOSSY_COMPRESSED,
        SPINNAKER_IMAGE_PAYLOAD_TYPE_JPEG_LOSSLESS_COMPRESSED
    };

    /* Enumeration of generic transport layer dependent payload types */
    enum TLPayloadType
    {
        SPINNAKER_TLPAYLOAD_TYPE_UNKNOWN = 0,         /* GenTL v1.2 */
        SPINNAKER_TLPAYLOAD_TYPE_IMAGE = 1,           /* GenTL v1.2 */
        SPINNAKER_TLPAYLOAD_TYPE_RAW_DATA = 2,        /* GenTL v1.2 */
        SPINNAKER_TLPAYLOAD_TYPE_FILE = 3,            /* GenTL v1.2 */
        SPINNAKER_TLPAYLOAD_TYPE_CHUNK_DATA = 4,      /* GenTL v1.2, Deprecated in GenTL 1.5*/
        SPINNAKER_TLPAYLOAD_TYPE_JPEG = 5,            /* GenTL v1.4 */
        SPINNAKER_TLPAYLOAD_TYPE_JPEG2000 = 6,        /* GenTL v1.4 */
        SPINNAKER_TLPAYLOAD_TYPE_H264 = 7,            /* GenTL v1.4 */
        SPINNAKER_TLPAYLOAD_TYPE_CHUNK_ONLY = 8,      /* GenTL v1.4 */
        SPINNAKER_TLPAYLOAD_TYPE_DEVICE_SPECIFIC = 9, /* GenTL v1.4 */
        SPINNAKER_TLPAYLOAD_TYPE_MULTI_PART = 10,     /* GenTL v1.5 */

        SPINNAKER_TLPAYLOAD_TYPE_CUSTOM_ID = 1000, /* Starting value for GenTL Producer custom IDs*/
        SPINNAKER_TLPAYLOAD_TYPE_LOSSLESS_COMPRESSED = SPINNAKER_TLPAYLOAD_TYPE_CUSTOM_ID + 1,
        SPINNAKER_TLPAYLOAD_TYPE_LOSSY_COMPRESSED = SPINNAKER_TLPAYLOAD_TYPE_CUSTOM_ID + 2,
        SPINNAKER_TLPAYLOAD_TYPE_JPEG_LOSSLESS_COMPRESSED = SPINNAKER_TLPAYLOAD_TYPE_CUSTOM_ID + 3
    };

    /** Possible Status Codes Returned from Action Command. */
    enum ActionCommandStatus
    {
        SPINNAKER_ACTION_COMMAND_STATUS_OK = 0, /* The device acknowledged the command.*/
        SPINNAKER_ACTION_COMMAND_STATUS_NO_REF_TIME =
            0x8013, /* The device is not synchronized to a master clock to be used as time reference. Typically used
                    when scheduled action commands cannot be scheduled for a future time since the reference time
                    coming from IEEE 1588 is not locked. */
        SPINNAKER_ACTION_COMMAND_STATUS_OVERFLOW = 0x8015, /* Returned when the scheduled action commands queue is full
                                                           and the device cannot accept the additional request. */
        SPINNAKER_ACTION_COMMAND_STATUS_ACTION_LATE =
            0x8016, /* The requested scheduled action command was requested at a point in time that is in the past. */
        SPINNAKER_ACTION_COMMAND_STATUS_ERROR =
            0x8FFF /* Generic Error. Try enabling the Extended Status Code 2.0 bit on gvcp configuration register in
                   order to receive more meaningful/detailed acknowledge messages from the device. */
    };

    /** Compression method to use for encoding TIFF images. */
    enum TIFFCompressionMethod
    {
        SPINNAKER_TIFF_COMPRESS_METHOD_NONE = 1,      /**< Save without any compression. */
        SPINNAKER_TIFF_COMPRESS_METHOD_PACKBITS,      /**< Save using PACKBITS compression. */
        SPINNAKER_TIFF_COMPRESS_METHOD_DEFLATE,       /**< Save using DEFLATE compression (ZLIB compression). */
        SPINNAKER_TIFF_COMPRESS_METHOD_ADOBE_DEFLATE, /**< Save using ADOBE DEFLATE compression */
                                                      /**
                                                       * Save using CCITT Group 3 fax encoding. This is only valid for
                                                       * 1-bit images only. Default to LZW for other bit depths.
                                                       */
        SPINNAKER_TIFF_COMPRESS_METHOD_CCITTFAX3,
        /**
         * Save using CCITT Group 4 fax encoding. This is only valid for
         * 1-bit images only. Default to LZW for other bit depths.
         */
        SPINNAKER_TIFF_COMPRESS_METHOD_CCITTFAX4,
        SPINNAKER_TIFF_COMPRESS_METHOD_LZW, /**< Save using LZW compression. */
                                            /**
                                             * Save using JPEG compression. This is only valid for 8-bit
                                             * greyscale and 24-bit only. Default to LZW for other bit depths.
                                             */
        SPINNAKER_TIFF_COMPRESS_METHOD_JPEG
    };

    /** Possible integer types and packing used in a pixel format. */
    enum PixelFormatIntType
    {
        SPINNAKER_INT_TYPE_UINT8,   /* Unsigned 8-bit integer. */
        SPINNAKER_INT_TYPE_INT8,    /* Signed 8-bit integer. */
        SPINNAKER_INT_TYPE_UINT10,  /* Unsigned 10-bit integer. */
        SPINNAKER_INT_TYPE_UINT10p, /* LSB packed unsigned 10-bit integer. */
        SPINNAKER_INT_TYPE_UINT10P, /* MSB packed unsigned 10-bit integer. */
        SPINNAKER_INT_TYPE_UINT12,  /* Unsigned 12-bit integer (unpacked). */
        SPINNAKER_INT_TYPE_UINT12p, /* LSB packed unsigned 12-bit integer. */
        SPINNAKER_INT_TYPE_UINT12P, /* MSB packed unsigned 12-bit integer. */
        SPINNAKER_INT_TYPE_UINT14,  /* Unsigned 14-bit integer (unpacked). */
        SPINNAKER_INT_TYPE_UINT16,  /* Unsigned 16-bit integer (unpacked). */
        SPINNAKER_INT_TYPE_INT16,   /* Signed 16-bit integer (unpacked). */
        SPINNAKER_INT_TYPE_FLOAT32, /* 32-bit float. */
        SPINNAKER_INT_TYPE_UNKNOWN
    };

    enum BufferOwnership
    {
        SPINNAKER_BUFFER_OWNERSHIP_SYSTEM, /* Buffers are owned and managed by the library */
        SPINNAKER_BUFFER_OWNERSHIP_USER    /* Buffers are owned and managed by the user */
    };

    /**
     * Image scaling algorithms.
     */
    enum ImageScalingAlgorithm
    {
        /*
         * Uses copies of the nearest source pixels to compute a scaled image.
         * The scaling factor used with this algorithm will be rounded to the
         * nearest integer (upscaling) or inverse integer (downscaling).
         * eg. 1.3 will be rounded to 1 and 0.3 will be rounded to (1 / 3).
         */
        SPINNAKER_IMAGE_SCALING_ALGORITHM_NEAREST_NEIGHBOR
    };

    /**
     * Image normalization source data options.
     * Options to normalize the source data based on the max and min values present in the specific
     * image (image data) or the theoretical abosolute max and min image data values for the image type (absolute
     * data). By default the abosolute max and min values for an image are the max and min values allowable for the
     * image's pixel format. An exception to this is for some computed image data formats such as AoLP, DoLP and
     * Stokes, where the absolute max and min are dependant on the algorithm used.
     *
     * For a given pixel, normalization is done by:
     * NormalizedValue = ((maxDest - minDest) * (PixelValue - minSrc) / (maxSrc - minSrc)) + minDest
     */
    enum SourceDataRange
    {
        /** Normalize based on the actual max and min values for the source image. */
        SPINNAKER_SOURCE_DATA_RANGE_IMAGE_DATA_RANGE,
        /** Normalize based on the theoretical max and min values for the source image. */
        SPINNAKER_SOURCE_DATA_RANGE_ABSOLUTE_DATA_RANGE,
        /** Normalize based on the actual min and theoretical max values for the source image. */
        SPINNAKER_SOURCE_DATA_RANGE_IMAGE_MIN_ABSOLUTE_MAX,
        /** Normalize based on the theoretical min and actual max values for the source image. */
        SPINNAKER_SOURCE_DATA_RANGE_ABSOLUTE_MIN_IMAGE_MAX
    };

    /**
     * Color specifiers for the heatmap color gradient.
     */
    enum HeatmapColor
    {
        SPINNAKER_HEATMAP_COLOR_BLACK = 1,
        SPINNAKER_HEATMAP_COLOR_BLUE = 2,
        SPINNAKER_HEATMAP_COLOR_CYAN = 3,
        SPINNAKER_HEATMAP_COLOR_GREEN = 4,
        SPINNAKER_HEATMAP_COLOR_YELLOW = 5,
        SPINNAKER_HEATMAP_COLOR_RED = 6,
        SPINNAKER_HEATMAP_COLOR_WHITE = 7
    };

    /**
     * Polarization quadrant specifiers describing the four orientations of linear polarizing
     * filters on polarized cameras
     */
    enum PolarizationQuadrant
    {
        /** The 0 degree of polarization. */
        SPINNAKER_POLARIZATION_QUADRANT_I0,
        /** The 45 degree of polarization. */
        SPINNAKER_POLARIZATION_QUADRANT_I45,
        /** The 90 degree of polarization. */
        SPINNAKER_POLARIZATION_QUADRANT_I90,
        /** The 135 degree of polarization. */
        SPINNAKER_POLARIZATION_QUADRANT_I135
    };

    enum CCMColorTemperature
    {
        SPINNAKER_CCM_COLOR_TEMP_TUNGSTEN_2800K,
        SPINNAKER_CCM_COLOR_TEMP_WARM_FLUORESCENT_3000K,
        SPINNAKER_CCM_COLOR_TEMP_COOL_FLUORESCENT_4000K,
        SPINNAKER_CCM_COLOR_TEMP_SUNNY_5000K,
        SPINNAKER_CCM_COLOR_TEMP_CLOUDY_6500K,
        SPINNAKER_CCM_COLOR_TEMP_SHADE_8000K,
        SPINNAKER_CCM_COLOR_TEMP_GENERAL
    };

    enum CCMType
    {
        SPINNAKER_CCM_TYPE_LINEAR,
        SPINNAKER_CCM_TYPE_ADVANCED
    };

    enum CCMSensor
    {
        SPINNAKER_CCM_SENSOR_IMX250 /* The only sensor that currently supports host-side color correction */
    };

    enum CCMColorSpace
    {
        SPINNAKER_CCM_COLOR_SPACE_OFF,
        SPINNAKER_CCM_COLOR_SPACE_SRGB
    };

    enum CCMApplication
    {
        SPINNAKER_CCM_APPLICATION_GENERIC,
        SPINNAKER_CCM_APPLICATION_MICROSCOPY
    };

    /*@}*/

    /*@{*/

    /**
     * @brief Spinnaker struct definitions.
     */

    /** Options for saving PNG images. */
    struct PNGOption
    {
        /** Whether to save the PNG as interlaced. */
        bool interlaced;
        /** Compression level (0-9). 0 is no compression, 9 is best compression. */
        unsigned int compressionLevel;
        /** Reserved for future use. */
        unsigned int reserved[16];

        PNGOption()
        {
            interlaced = false;
            compressionLevel = 6;
            memset(reserved, 0, sizeof(reserved));
        }
    };

    /** Options for saving PPM images. */
    struct PPMOption
    {
        /** Whether to save the PPM as a binary file. */
        bool binaryFile;
        /** Reserved for future use. */
        unsigned int reserved[16];

        PPMOption()
        {
            binaryFile = true;
            memset(reserved, 0, sizeof(reserved));
        }
    };

    /** Options for saving PGM images. */
    struct PGMOption
    {
        /** Whether to save the PPM as a binary file. */
        bool binaryFile;
        /** Reserved for future use. */
        unsigned int reserved[16];

        PGMOption()
        {
            binaryFile = true;
            memset(reserved, 0, sizeof(reserved));
        }
    };

    /** Options for saving TIFF images. */
    struct TIFFOption
    {
        /** Compression method to use for encoding TIFF images. */
        TIFFCompressionMethod compression;
        /** Reserved for future use. */
        unsigned int reserved[16];

        TIFFOption()
        {
            compression = SPINNAKER_TIFF_COMPRESS_METHOD_LZW;
            memset(reserved, 0, sizeof(reserved));
        }
    };

    /** Options for saving JPEG image. */
    struct JPEGOption
    {
        /** Whether to save as a progressive JPEG file. */
        bool progressive;
        /**
         * JPEG image quality in range (0-100).
         * - 100 - Superb quality.
         * - 75  - Good quality.
         * - 50  - Normal quality.
         * - 10  - Poor quality.
         */
        unsigned int quality;
        /** Reserved for future use. */
        unsigned int reserved[16];

        JPEGOption()
        {
            progressive = false;
            quality = 75;
            memset(reserved, 0, sizeof(reserved));
        }
    };

    /** Options for saving JPEG2000 image. */
    struct JPG2Option
    {
        /** JPEG saving quality in range (1-512). */
        unsigned int quality;
        /** Reserved for future use. */
        unsigned int reserved[16];

        JPG2Option()
        {
            quality = 16;
            memset(reserved, 0, sizeof(reserved));
        }
    };

    /** Options for saving Bitmap image. */
    struct BMPOption
    {
        bool indexedColor_8bit;
        /** Reserved for future use. */
        unsigned int reserved[16];

        BMPOption()
        {
            indexedColor_8bit = false;
            memset(reserved, 0, sizeof(reserved));
        }
    };

    /** Options for saving Spinnaker image. */
    struct SIOption
    {
        /** Reserved for future use. */
        unsigned int reserved[16];

        SIOption()
        {
            memset(reserved, 0, sizeof(reserved));
        }
    };

    /** Provides easier access to the current version of Spinnaker. **/
    struct LibraryVersion
    {
        /** Major version of the library **/
        unsigned int major;

        /** Minor version of the library **/
        unsigned int minor;

        /** Version type of the library **/
        unsigned int type;

        /** Build number of the library **/
        unsigned int build;
    };

    /** Action Command Result */
    struct ActionCommandResult
    {
        unsigned int DeviceAddress; /* IP Address of device that responded to Action Command. */
        ActionCommandStatus Status; /* Action Command status return from device. */
    };

    struct CCMSettings
    {
        CCMColorTemperature ColorTemperature; /* Determines the tone of the color correction to be applied.
                                                 Ignored when CustomCCMCode is not empty. */
        CCMType Type;                         /* The type of CCM affects the color accuracy and conversion speed.
                                                 Ignored when CustomCCMCode is not empty. */
        CCMSensor Sensor;                     /* Informs Spinnaker to use the CCM calibrated for this sensor.
                                                 Ignored when CustomCCMCode is not empty. */
        std::string CustomCCMCode;            /* Custom encrypted CCM provided by FLIR. */

        CCMColorSpace ColorSpace;

        CCMApplication Application;

        CCMSettings()
        {
            ColorTemperature = SPINNAKER_CCM_COLOR_TEMP_GENERAL;
            Type = SPINNAKER_CCM_TYPE_ADVANCED;
            Sensor = SPINNAKER_CCM_SENSOR_IMX250;
            ColorSpace = SPINNAKER_CCM_COLOR_SPACE_SRGB;
            Application = SPINNAKER_CCM_APPLICATION_GENERIC;
            CustomCCMCode = "";
        }
    };

    /**  Data fields for Device Event payload for EventInference */
    struct DeviceEventInferenceData
    {
        uint32_t result;  /* inference classification result of the Inference Event */
        float confidence; /* inference confidence result of the Inference Event */
        uint64_t frameID; /* frame ID associated with the inference result of the Inference Event */
    };

    /**  Data Fields for Device Event payload for EventExposureEnd */
    struct DeviceEventExposureEndData
    {
        uint64_t frameID; /* frame ID associated with the Exposure End Event */
    };

    /*@}*/

    /*@}*/
} // namespace Spinnaker

#endif // FLIR_SPINNAKER_DEFS_H
