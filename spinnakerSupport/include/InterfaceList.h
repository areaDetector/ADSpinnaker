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

#ifndef FLIR_SPINNAKER_INTERFACELIST_H
#define FLIR_SPINNAKER_INTERFACELIST_H

#include "Interface/IInterfaceList.h"

namespace Spinnaker
{
    /**
     * @defgroup SpinnakerClasses Spinnaker Classes
     */

    /*@{*/

    /**
     * @defgroup InterfaceList_h InterfaceList Class
     */

    /*@{*/

    /**
     * @brief A list of the available interfaces on the system.
     */

    class SPINNAKER_API InterfaceList : public IInterfaceList
    {
      public:
        /*
         * Default Constructor
         */
        InterfaceList(void);

        /*
         * Virtual Destructor
         */
        virtual ~InterfaceList(void);

        /*
         * Copy Constructor
         */
        InterfaceList(const InterfaceList& iface);

        /**
         * Assignment operator.
         */
        InterfaceList& operator=(const InterfaceList& iface);

        /**
         * Array subscription operators
         */
        InterfacePtr operator[](unsigned int index);

        /**
         * Returns the size of the interface list.  The size is the number
         * of Interface objects stored in the list.
         *
         * @return An integer that represents the list size.
         */
        unsigned int GetSize() const;

        /**
         * Returns a pointer to an Interface object at the "index".
         *
         * @param index The index at which to retrieve the Interface object
         *
         * @return A pointer to an Interface object.
         */
        InterfacePtr GetByIndex(unsigned int index) const;

        /**
         * Returns a pointer to a interface object with the specified interface identifier. This
         * function will return a NULL InterfacePtr if no matching interface identifier is found.
         *
         * @param interfaceID The unique interface identifier of the interface object to retrieve
         *
         * @return A pointer to an Interface object.
         */
        InterfacePtr GetByInterfaceID(std::string interfaceID) const;

        /**
         * Clears the list of interfaces and destroys their corresponding objects.
         * It is important to first make sure there are no referenced cameras still
         * in use before calling Clear().  If a camera on any of the interfaces is still
         * in use this function will throw an exception.
         *
         */
        void Clear();

        /**
         * Adds a copy of an interface object.
         *
         * @param iface An interface object to be added to this list.
         */
        void Add(InterfacePtr iface);

        /**
         * Removes all occurences of an interface that is pointed to by the input interface
         * pointer and destroys its corresponding reference counted object. This function will
         * throw a Spinnaker exception with SPINNAKER_ERR_NOT_AVAILABLEerror if no matching 
         * interface is found.
         *
         * @param iface Pointer to the interface that is to be removed
         */
        void Remove(InterfacePtr iface);

        /**
         * Appends a copy of the interfact list.
         *
         * @param list Another InterfaceList object, whose elements are added to this list.
         */
        void Append(const InterfaceList* list);

      protected:
        friend class SystemImpl;
        friend class ProducerImpl;
    };

    /*@}*/

    /*@}*/
} // namespace Spinnaker

#endif // FLIR_SPINNAKER_INTERFACELIST_H