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

#ifndef FLIR_SPINNAKER_IIMAGEPROCESSOR_H
#define FLIR_SPINNAKER_IIMAGEPROCESSOR_H

#include "CameraDefs.h"
#include "SpinnakerDefs.h"

namespace Spinnaker
{
    class ImagePtr;
    class ImageList;

    /**
     *  @defgroup SpinnakerClasses Spinnaker Classes
     */

    /*@{*/

    /**
     *  @defgroup ImageProcessor_h Image Processor Class
     */

    /*@{*/

    /**
     * @brief Used to hold a list of image objects.
     */
    class SPINNAKER_API IImageProcessor
    {
      public:
        virtual ~IImageProcessor(void){};

        virtual void SetColorProcessing(ColorProcessingAlgorithm colorAlgorithm) = 0;
        virtual ColorProcessingAlgorithm GetColorProcessing() = 0;

        virtual void SetNumDecompressionThreads(unsigned int numThreads) = 0;
        virtual unsigned int GetNumDecompressionThreads() = 0;

        virtual ImagePtr Convert(const ImagePtr& srcImage, PixelFormatEnums destFormat) const = 0;
        virtual void Convert(const ImagePtr& srcImage, ImagePtr& destImage, PixelFormatEnums destFormat) const = 0;
        virtual ImagePtr Convert(const ImageList& srcImageList, PixelFormatEnums destFormat) const = 0;
        virtual void Convert(const ImageList& srcImageList, ImagePtr& destImage, PixelFormatEnums destFormat) const = 0;

        virtual ImagePtr ApplyGamma(const ImagePtr& srcImage, float gamma, bool isGammaInverse = false) const = 0;
        virtual void ApplyGamma(const ImagePtr& srcImage, ImagePtr& destImage, float gamma, bool isGammaInverse = false)
            const = 0;

      protected:
        struct ImageProcessorData; // Forward declaration
        ImageProcessorData* m_pImageProcessorData;

        IImageProcessor(){};
        IImageProcessor(const IImageProcessor&){};
        IImageProcessor& operator=(const IImageProcessor&);
    };

    /*@}*/

    /*@}*/

} // namespace Spinnaker

#endif // FLIR_SPINNAKER_IIMAGEPROCESSOR_H