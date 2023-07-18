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

#ifndef FLIR_SPINNAKER_IMAGE_UTILITY_CCM_H
#define FLIR_SPINNAKER_IMAGE_UTILITY_CCM_H

#include "SpinnakerPlatform.h"
#include "SpinnakerDefs.h"

namespace Spinnaker
{
    class ImagePtr;

    /**
     *  @defgroup SpinnakerClasses Spinnaker Classes
     */
    /*@{*/

    /**
     *  @defgroup ImageUtilityCCM_h Image Utility CCM Class
     */
    /*@{*/

    /**
     * @brief Static function to create color corrected images from an image object
     */

    class SPINNAKER_API ImageUtilityCCM
    {
      public:
        /**
         * Create a color corrected image from the source image by applying
         * a color correction matrix calibrated according to the settings specified.
         * When using ImageUtilityCCM, users are advised to disable CCM on the camera before
         * capturing source images. This can be done through the camera node "ColorTransformationEnable".
         *
         * Color correction is currently supported for the following pixel formats:
         * - PixelFormat_BGR8
         * - PixelFormat_BGRa8
         * - PixelFormat_RGBa8
         * - PixelFormat_RGB8
         * - PixelFormat_BGR16
         * - PixelFormat_BGRa16
         * - PixelFormat_RGBa16
         * - PixelFormat_RGB16
         * The output image will have the same pixel format as the source image.
         *
         * @param srcImage The source image to which the CCM is applied
         * @param settings Selected CCM settings including CCMColorTemperature, CCMType, CCMSensor, etc
         *
         * @return The color corrected image
         *
         * @see CCMSettings
         */
        static ImagePtr CreateColorCorrected(const ImagePtr& srcImage, const CCMSettings& settings);

        /**
         * Create a color corrected image from the source image by applying
         * a color correction matrix calibrated according to the settings specified.
         * When using ImageUtilityCCM, users are advised to disable CCM on the camera before
         * capturing source images. This can be done through the camera node "ColorTransformationEnable".
         *
         * Color correction is currently supported for the following pixel formats:
         * - PixelFormat_BGR8
         * - PixelFormat_BGRa8
         * - PixelFormat_RGBa8
         * - PixelFormat_RGB8
         * - PixelFormat_BGR16
         * - PixelFormat_BGRa16
         * - PixelFormat_RGBa16
         * - PixelFormat_RGB16
         *
         * The destination image height and width must be the same as the source image.
         *
         * @param srcImage The source image to which the CCM is applied
         * @param destImage The destination image in which to store the color corrected image
         * @param settings Selected CCM settings including CCMColorTemperature, CCMType, CCMSensor, etc
         *
         * @see CCMSettings
         */
        static void CreateColorCorrected(const ImagePtr& srcImage, ImagePtr& destImage, const CCMSettings& settings);
    };

    /*@}*/

    /*@}*/
} // namespace Spinnaker

#endif // FLIR_SPINNAKER_IMAGE_UTILITY_CCM_H