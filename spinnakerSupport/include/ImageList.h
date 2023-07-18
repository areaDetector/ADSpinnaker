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

#ifndef FLIR_SPINNAKER_IMAGELIST_H
#define FLIR_SPINNAKER_IMAGELIST_H

#include "Interface/IImageList.h"

namespace Spinnaker
{
    class SPINNAKER_API ImageList : public IImageList
    {
      public:
        /**
         * Default constructor.
         */
        ImageList(void);

        /**
         * Virtual destructor.
         */
        virtual ~ImageList(void);

        /**
         * Copy constructor
         */
        ImageList(const ImageList& iface);

        /**
         * Assignment operator.
         */
        ImageList& operator=(const ImageList& iface);

        /**
         * Array subscription operators.
         */
        ImagePtr operator[](unsigned int index);
        const ImagePtr operator[](unsigned int index) const;

        /*
         * Releases all the images that were acquired calling camera->GetNextImageSync().
         *
         * @see Init()
         * @see CameraBase::GetNextImageSync( grabTimeout )
         */
        void Release();

        /**
         * Returns the size of the image list.  The size is the number
         * of Image objects stored in the list.
         *
         * @return An integer that represents the list size.
         */
        unsigned int GetSize() const;

        /**
         * Returns a pointer to an image object at the "index". This function will throw
         * a Spinnaker exception with SPINNAKER_ERR_INVALID_PARAMETER error if the input
         * index is out of range.
         *
         * @param index The index at which to retrieve the Image object
         *
         * @return A pointer to an image object.
         */
        ImagePtr GetByIndex(unsigned int index) const;

        /**
         * Returns a pointer to the first image object with the pixel format from the list.
         * This function will return a NULL ImagePtr if no matching pixel format is found.
         *
         * @param pixelFormat The pixel format of the Image object to retrieve
         *
         * @return A pointer to an image object.
         */
        ImagePtr GetByPixelFormat(PixelFormatEnums pixelFormat) const;

        /**
         * Clears the list of images and destroys their corresponding reference counted
         * objects. This is necessary in order to clean up the parent interface.
         * It is important that the camera list is destroyed or is cleared before calling
         * system->ReleaseInstance() or else the call to system->ReleaseInstance()
         * will result in an error message thrown that a reference to the camera
         * is still held.
         *
         * @see System::ReleaseInstance()
         */
        void Clear();

        /**
         * Adds a copy of an image object.
         *
         * @param image An image object to be added to this list.
         */
        void Add(ImagePtr image);

        /**
         * Removes an image at "index" and destroys its corresponding reference counted
         * object. This function will throw a Spinnaker exception with
         * SPINNAKER_ERR_INVALID_PARAMETER error if the input index is out of range.
         *
         * @param index The index at which to remove the Image object
         */
        void RemoveByIndex(unsigned int index);

        /**
         * Removes an image object with the image pixel format from the list. This
         * function will throw a Spinnaker exception with SPINNAKER_ERR_INVALID_PARAMETER
         * error if the list does not have any image with the specified pixel format.
         *
         * @param pixelFormat The pixel format of the image object to remove
         */
        void RemoveByPixelFormat(PixelFormatEnums pixelFormat);

        /**
         * Appends a copy of the image list.
         *
         * @param list Another ImageList object, whose elements are added to this list.
         */
        void Append(const ImageList& list);

        /**
         * Saves an image list as an object to a file.
         *
         * @param filename Name of the file to save the current image list object to.
         *                 It is recommended to use the file extension 'sil'.
         *
         * @see Load()
         */
        void Save(const char* filename);

        /**
         * Creates an image list object from file.
         *
         * @param filename Name of the file to load an image object from.
         *
         * @see Save()
         */
        static ImageList Load(const char* filename);
    };

    /*@}*/

    /*@}*/
} // namespace Spinnaker

#endif // FLIR_SPINNAKER_IMAGELIST_H