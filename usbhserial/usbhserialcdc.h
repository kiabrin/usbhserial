//*****************************************************************************
//
// usbhserialcdc.h - USBLib serial host driver for CDC devices
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

#ifndef USBHSERIALCDC_H_
#define USBHSERIALCDC_H_

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

#define DECLARE_USB_SERIAL_CDC_DRIVER                               \
{                                                                   \
    USB_CLASS_CDC,                                                  \
    0,                                                              \
    0,                                                              \
    false,                                                          \
    0,                                                              \
    USBHSerialCDCInit,                                              \
    USBHSerialCDCSetBaud,                                           \
    USBHSerialCDCGetBaud,                                           \
    USBHSerialCDCSetCoding,                                         \
    USBHSerialCDCGetCoding,                                         \
    USBHSerialCDCSetControlLineState,                               \
    USBHSerialCDCGetControlLineState,                               \
    USBHSerialCDCSetFlow,                                           \
    USBHSerialCDCBreakSet,                                          \
    USBHSerialCDCBreakClear                                         \
}

extern uint32_t USBHSerialCDCInit(tSerialInstance *psSerialInstance);
extern uint32_t USBHSerialCDCSetBaud(tSerialInstance *psSerialInstance, uint32_t ui32Baud);
extern uint32_t USBHSerialCDCGetBaud(tSerialInstance *psSerialInstance);
extern uint32_t USBHSerialCDCSetCoding(tSerialInstance *psSerialInstance, uint32_t ui32Coding);
extern uint32_t USBHSerialCDCGetCoding(tSerialInstance *psSerialInstance);
extern uint32_t USBHSerialCDCSetControlLineState(tSerialInstance *psSerialInstance, uint32_t ui32Control);
extern uint32_t USBHSerialCDCGetControlLineState(tSerialInstance *psSerialInstance);
extern uint32_t USBHSerialCDCSetFlow(tSerialInstance *psSerialInstance, uint32_t ui32Flow);
extern uint32_t USBHSerialCDCBreakSet(tSerialInstance *psSerialInstance);
extern uint32_t USBHSerialCDCBreakClear(tSerialInstance *psSerialInstance);

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

#endif /* USBHSERIALCDC_H_ */
