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

#ifndef FLIR_SPINNAKER_IMAGE_PROCESSOR_H
#define FLIR_SPINNAKER_IMAGE_PROCESSOR_H

#include "Interface/IImageProcessor.h"
#include "SpinnakerPlatform.h"
#include "CameraDefs.h"
#include "SpinnakerDefs.h"

namespace Spinnaker
{
    class ImagePtr;
    class ImageList;
    class Image;

    /**
     *  @defgroup SpinnakerClasses Spinnaker Classes
     */
    /*@{*/

    /**
     *  @defgroup ImageProcessor_h Image Processor Class
     */
    /*@{*/

    /**
     * @class ImageProcessor
     * @brief Image post processing class for converting a source image to
     * another pixel format
     *
     * All supported input image pixel formats can be converted to supported
     * output image pixel formats. If the input pixel format is a compressed
     * format, the decompression will occur before converting to the output
     * pixel format.
     *
     * List of supported input image pixel formats:
     *
     * <ul>
     * <li> PixelFormat_Mono8
     * <li> PixelFormat_Mono16
     * <li> PixelFormat_BayerGR8
     * <li> PixelFormat_BayerRG8
     * <li> PixelFormat_BayerGB8
     * <li> PixelFormat_BayerBG8
     * <li> PixelFormat_BayerGR16
     * <li> PixelFormat_BayerRG16
     * <li> PixelFormat_BayerGB16
     * <li> PixelFormat_BayerBG16
     * <li> PixelFormat_Mono12Packed
     * <li> PixelFormat_BayerGR12Packed
     * <li> PixelFormat_BayerRG12Packed
     * <li> PixelFormat_BayerGB12Packed
     * <li> PixelFormat_BayerBG12Packed
     * <li> PixelFormat_YUV411Packed
     * <li> PixelFormat_YUV422Packed
     * <li> PixelFormat_YUV444Packed
     * <li> PixelFormat_Mono12p
     * <li> PixelFormat_BayerGR12p
     * <li> PixelFormat_BayerRG12p
     * <li> PixelFormat_BayerGB12p
     * <li> PixelFormat_BayerBG12p
     * <li> PixelFormat_YCbCr8
     * <li> PixelFormat_YCbCr422_8
     * <li> PixelFormat_YCbCr411_8
     * <li> PixelFormat_BGR8
     * <li> PixelFormat_BGRa8
     * <li> PixelFormat_Mono10Packed
     * <li> PixelFormat_BayerGR10Packed
     * <li> PixelFormat_BayerRG10Packed
     * <li> PixelFormat_BayerGB10Packed
     * <li> PixelFormat_BayerBG10Packed
     * <li> PixelFormat_Mono10p
     * <li> PixelFormat_BayerGR10p
     * <li> PixelFormat_BayerRG10p
     * <li> PixelFormat_BayerGB10p
     * <li> PixelFormat_BayerBG10p
     * <li> PixelFormat_Mono10
     * <li> PixelFormat_Mono12
     * <li> PixelFormat_Mono14
     * <li> PixelFormat_BayerBG10
     * <li> PixelFormat_BayerBG12
     * <li> PixelFormat_BayerGB10
     * <li> PixelFormat_BayerGB12
     * <li> PixelFormat_BayerGR10
     * <li> PixelFormat_BayerGR12
     * <li> PixelFormat_BayerRG10
     * <li> PixelFormat_BayerRG12
     * <li> PixelFormat_RGBa8
     * <li> PixelFormat_RGB8
     * <li> PixelFormat_BGR16
     * <li> PixelFormat_R12
     * <li> PixelFormat_G12
     * <li> PixelFormat_B12
     * <li> PixelFormat_YUV8_UYV
     * <li> PixelFormat_YUV411_8_UYYVYY
     * <li> PixelFormat_YUV422_8
     * <li> PixelFormat_Polarized8
     * <li> PixelFormat_Polarized10p
     * <li> PixelFormat_Polarized12p
     * <li> PixelFormat_Polarized16
     * <li> PixelFormat_BayerRGPolarized8
     * <li> PixelFormat_BayerRGPolarized10p
     * <li> PixelFormat_BayerRGPolarized12p
     * <li> PixelFormat_BayerRGPolarized16
     * <li> PixelFormat_LLCMono8
     * <li> PixelFormat_LLCBayerRG8
     * <li> PixelFormat_JPEGMono8
     * <li> PixelFormat_JPEGColor8
     * <li> PixelFormat_Raw16
     * <li> PixelFormat_Raw8
     * <li> PixelFormat_R12_Jpeg
     * <li> PixelFormat_GR12_Jpeg
     * <li> PixelFormat_GB12_Jpeg
     * <li> PixelFormat_B12_Jpeg
     * </ul>
     *
     * List of supported output image pixel formats
     *
     * <ul>
     * <li> PixelFormat_Mono8
     * <li> PixelFormat_Mono16
     * <li> PixelFormat_BayerBG8
     * <li> PixelFormat_BayerGB8
     * <li> PixelFormat_BayerRG8
     * <li> PixelFormat_BayerGR8
     * <li> PixelFormat_BayerBG16
     * <li> PixelFormat_BayerGB16
     * <li> PixelFormat_BayerRG16
     * <li> PixelFormat_BayerGR16
     * <li> PixelFormat_BGR8
     * <li> PixelFormat_BGRa8
     * <li> PixelFormat_RGB8
     * <li> PixelFormat_RGBa8
     * <li> PixelFormat_BGR16
     * <li> PixelFormat_RGB16
     * <li> PixelFormat_R12
     * <li> PixelFormat_G12
     * <li> PixelFormat_B12
     * </ul>
     */

    class SPINNAKER_API ImageProcessor : public IImageProcessor
    {
      public:
        /**
         * Default constructor.
         */
        ImageProcessor();

        /**
         * Default destructor.
         */
        virtual ~ImageProcessor();

        /**
         * Copy constructor
         */
        ImageProcessor(const ImageProcessor& iface);

        /**
         * Assignment operator.
         */
        ImageProcessor& operator=(const ImageProcessor& iface);

        /**
         * Sets the color processing algorithm used at the time of the Convert()
         * call, therefore the most recent execution of this function will take
         * precedence. The DEFAULT algorithm is deprecated and should not be used
         * in the ImageProcessor class.
         *
         * @param colorAlgorithm The color processing algorithm to set.
         *
         * @see GetColorProcessing()
         */
        void SetColorProcessing(ColorProcessingAlgorithm colorAlgorithm);

        /**
         * Gets the default color processing algorithm.
         *
         * @see SetColorProcessing()
         *
         * @return The default color processing algorithm.
         */
        ColorProcessingAlgorithm GetColorProcessing();

        /**
         * Sets the default number of threads used for image decompression during
         * Convert(). The number of threads used is defaulted to be equal to one
         * less than the number of concurrent threads supported by the
         * system.
         *
         * @param numThreads Number of parallel image decompression threads set to run
         *
         * @see Convert()
         */
        void SetNumDecompressionThreads(unsigned int numThreads);

        /**
         * Gets the number of threads used for image decompression during Convert().
         *
         * @see SetNumDecompressionThreads()
         *
         * @return Number of parallel image decompression threads set to run.
         */
        unsigned int GetNumDecompressionThreads();

        /**
         * Converts the source image buffer to the specified destination pixel format
         * and returns the result in a new image. The destination image does not
         * need to be configured in any way before the call is made.
         *
         * Note that compressed images are decompressed before any further color processing
         * or conversion during this call. Decompression is multi-threaded and defaults to
         * utilizing one less than the number of concurrent threads supported by the
         * system. The default number of decompression threads can be set with
         * SetNumDecompressionThreads().
         *
         * @see PixelFormatEnums
         * @see SetNumDecompressionThreads
         *
         * @param srcImage The source image from which to convert the image from.
         * @param destFormat Output format of the converted image.
         *
         * @return The converted image.
         */
        ImagePtr Convert(const ImagePtr& srcImage, PixelFormatEnums destFormat) const;

        /**
         * Converts the source image buffer to the specified destination pixel format and
         * stores the result in the destination image. The destination image needs to be
         * configured to have the correct buffer size before calling this function. See
         * ResetImage() to setup the correct buffer size according to specified pixel format.
         *
         * Note that compressed images are decompressed before any further color processing
         * or conversion during this call. Decompression is multi-threaded and defaults to
         * utilizing one less than the number of concurrent threads supported by the
         * system. The default number of decompression threads can be set with
         * SetNumDecompressionThreads().
         *
         * @see PixelFormatEnums
         * @see ResetImage
         * @see SetNumDecompressionThreads
         *
         * @param srcImage The source image from which to convert the image from.
         * @param destImage The destination image in which the converted image data will be stored.
         * @param destFormat Output format of the converted image.
         */
        void Convert(const ImagePtr& srcImage, ImagePtr& destImage, PixelFormatEnums destFormat) const;

        /**
         * Converts the source list of image buffers to the specified output pixel format
         * and returns the result in a new image. The conversion could encompasses decompression,
         * interleaving and conversion of image data depending on the source pixel format of
         * images in the source image list. The destination image does not need to be configured
         * in any way before the call is made.
         *
         * Note that compressed images are decompressed before any further color processing,
         * interleaving or conversion is performed. Decompression is multi-threaded and
         * defaults to utilizing one less than the number of concurrent threads supported by
         * the system. The default number of decompression threads can be set with
         * SetNumDecompressionThreads().
         *
         * Note not all the supported image pixel formats described in the class description
         * are supported in this function.
         *
         * List of supported image pixel formats for the source image list:
         *
         * <ul>
         * <li> PixelFormat_R12
         * <li> PixelFormat_GR12
         * <li> PixelFormat_GB12
         * <li> PixelFormat_B12
         * <li> PixelFormat_R12_Jpeg
         * <li> PixelFormat_GR12_Jpeg
         * <li> PixelFormat_GB12_Jpeg
         * <li> PixelFormat_B12_Jpeg
         * </ul>
         *
         * @see PixelFormatEnums
         * @see SetNumDecompressionThreads
         *
         * @param srcImageList List of images from which to convert the images from.
         * @param destFormat Output format of the converted image.
         *
         * @return The converted image.
         */
        ImagePtr Convert(const ImageList& srcImageList, PixelFormatEnums destFormat) const;

        /**
         * Converts the source list of image buffers to the specified output pixel format
         * and returns the result in a new image. The conversion could encompasses decompression,
         * interleaving and conversion of image data depending on the source pixel format of
         * images in the source image list. The destination image needs to be configured to
         * have the correct buffer size before calling this function. See ResetImage() to
         * setup the correct buffer size according to specified pixel format.
         *
         * Note that compressed images are decompressed before any further color processing,
         * interleaving or conversion is performed. Decompression is multi-threaded and
         * defaults to utilizing one less than the number of concurrent threads supported by
         * the system. The default number of decompression threads can be set with
         * SetNumDecompressionThreads().
         *
         * Note not all the supported image pixel formats described in the class description
         * are supported in this function.
         *
         * List of supported image pixel formats for the source image list:
         *
         * <ul>
         * <li> PixelFormat_R12
         * <li> PixelFormat_GR12
         * <li> PixelFormat_GB12
         * <li> PixelFormat_B12
         * <li> PixelFormat_R12_Jpeg
         * <li> PixelFormat_GR12_Jpeg
         * <li> PixelFormat_GB12_Jpeg
         * <li> PixelFormat_B12_Jpeg
         * </ul>
         *
         * @see PixelFormatEnums
         * @see ResetImage
         * @see SetNumDecompressionThreads
         *
         * @param srcImageList List of images from which to convert the images from.
         * @param destImage The destination image in which the converted image data will be stored.
         * @param destFormat Output format of the converted image.
         */
        void Convert(const ImageList& srcImageList, ImagePtr& destImage, PixelFormatEnums destFormat) const;

        /**
         * Applies gamma correction to the source image and returns a new image.
         *
         * @param srcImage The source image from which to apply gamma on.
         * @param gamma Gamma value to apply. A value between 0.5 and 4 is acceptable. (Default assuming
         * image-to-screen)
         * @param applyGammaInverse Converts a gamma corrected source image back to the original image using the inverse
         * of the gamma value (used for applying screen-to-image gamma)
         */
        ImagePtr ApplyGamma(const ImagePtr& srcImage, float gamma, bool applyGammaInverse = false) const;

        /**
         * Applies gamma correction to the source image and stores the result in the destination image.
         *
         * @param srcImage The source image from which to apply gamma on.
         * @param destImage The destination image in which the gamma applied image data will be stored.
         * @param gamma Gamma value to apply. A value between 0.5 and 4 is acceptable. (Default assuming
         * image-to-screen)
         * @param applyGammaInverse Converts a gamma corrected source image back to the original image using the inverse
         * of the gamma value (used for applying screen-to-image gamma)
         */
        void ApplyGamma(const ImagePtr& srcImage, ImagePtr& destImage, float gamma, bool applyGammaInverse = false)
            const;

      protected:
        friend class ImageFiler;

        void Convert(const Image* pSrcImage, Image* pDestImage, PixelFormatEnums destFormat) const;
    };

    /*@}*/

    /*@}*/
} // namespace Spinnaker

#endif // FLIR_SPINNAKER_IMAGE_PROCESSOR_H