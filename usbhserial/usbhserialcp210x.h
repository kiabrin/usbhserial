//*****************************************************************************
//
// usbhserialcp210x.h - USBLib serial host driver for CP210x devices
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

#ifndef USBHSERIALCP210X_H_
#define USBHSERIALCP210X_H_

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

#define ADSFADFASDF

#define DECLARE_USB_SERIAL_CP210X_DRIVER                            \
{                                                                   \
    USB_CLASS_VEND_SPECIFIC,                                        \
    0x10C4,                                                         \
    0xEA60,                                                         \
    true,                                                           \
    8,                                                              \
    USBHSerialCPInit,                                               \
    USBHSerialCPSetBaud,                                            \
    USBHSerialCPGetBaud,                                            \
    USBHSerialCPSetCoding,                                          \
    USBHSerialCPGetCoding,                                          \
    USBHSerialCPSetControlLineState,                                \
    USBHSerialCPGetControlLineState,                                \
    USBHSerialCPSetFlow,                                            \
    USBHSerialCPBreakSet,                                           \
    USBHSerialCPBreakClear                                          \
}

extern uint32_t USBHSerialCPInit(tSerialInstance *psSerialInstance);
extern uint32_t USBHSerialCPSetBaud(tSerialInstance *psSerialInstance, uint32_t ui32Baud);
extern uint32_t USBHSerialCPGetBaud(tSerialInstance *psSerialInstance);
extern uint32_t USBHSerialCPSetCoding(tSerialInstance *psSerialInstance, uint32_t ui32Coding);
extern uint32_t USBHSerialCPGetCoding(tSerialInstance *psSerialInstance);
extern uint32_t USBHSerialCPSetControlLineState(tSerialInstance *psSerialInstance, uint32_t ui32Control);
extern uint32_t USBHSerialCPGetControlLineState(tSerialInstance *psSerialInstance);
extern uint32_t USBHSerialCPSetFlow(tSerialInstance *psSerialInstance, uint32_t ui32Flow);
extern uint32_t USBHSerialCPBreakSet(tSerialInstance *psSerialInstance);
extern uint32_t USBHSerialCPBreakClear(tSerialInstance *psSerialInstance);

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

#endif /* USBHSERIALCP210X_H_ */
