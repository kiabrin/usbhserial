//*****************************************************************************
//
// usbhserialdriver.h - USBLib serial host driver definitions
//
// Copyright (c) 2008-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 2.2.0.295 of the Tiva USB Library.
//
//*****************************************************************************

#ifndef USBHSERIALDRIVER_H_
#define USBHSERIALDRIVER_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! This is the structure that holds all of the data for a given driver of
//! a serial device.
//
//*****************************************************************************
typedef struct {

    //
    //! The interface class that this device class driver supports.
    //
    uint32_t ui32InterfaceClass;

    //
    //! Vendor ID of device (0 - do not care)
    //
    uint16_t ui16VID;

    //
    //! Product ID of device (0 - do not care)
    //
    uint16_t ui16PID;

    //
    //! Polling mode flag
    //
    bool bPolling;

    //
    //! Polling interval
    //
    uint32_t ui32Interval;

    //
    //! Init function pointer
    //
    uint32_t (* pfnInit)(tSerialInstance *psSerialInstance);

    //
    //! Baud set function pointer
    //
    uint32_t (* pfnSetBaud)(tSerialInstance *psSerialInstance, uint32_t ui32Baud);

    //
    //! Baud get function pointer
    //
    uint32_t (* pfnGetBaud)(tSerialInstance *psSerialInstance);

    //
    //! Line coding set function pointer
    //
    uint32_t (* pfnSetCoding)(tSerialInstance *psSerialInstance, uint32_t ui32Coding);

    //
    //! Line coding get function pointer
    //
    uint32_t (* pfnGetCoding)(tSerialInstance *psSerialInstance);

    //
    //! Control line set function pointer
    //
    uint32_t (* pfnSetControlLineState)(tSerialInstance *psSerialInstance, uint32_t ui32Control);

    //
    //! Control line set function pointer
    //
    uint32_t (* pfnGetControlLineState)(tSerialInstance *psSerialInstance);

    //
    //! Set flow control function pointer
    //
    uint32_t (* pfnSetFlow)(tSerialInstance *psSerialInstance, uint32_t ui32Flow);

    //
    //! Break set function pointer
    //
    uint32_t (* pfnBreakSet)(tSerialInstance *psSerialInstance);

    //
    //! Break clear function pointer
    //
    uint32_t (* pfnBreakClear)(tSerialInstance *psSerialInstance);

} tUSBSerialDriver;

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif /* USBHSERIALDRIVER_H_ */
