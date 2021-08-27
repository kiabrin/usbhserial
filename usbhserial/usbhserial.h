//*****************************************************************************
//
// usbhserial.h - USBLib support for serial devices on host
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

#ifndef USBHSERIAL_H_
#define USBHSERIAL_H_

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
//! This is the structure that holds all of the data for a given instance of
//! a serial device.
//
//*****************************************************************************
typedef struct
{
    //
    // Save the device instance.
    //
    tUSBHostDevice *psDevice;

    //
    // Used to save the callback.
    //
    tUSBCallback pfnCallback;

    //
    // Callback data provided by caller.
    //
    void *pvCBData;

    //
    // Used to remember what type of device was registered.
    //
    uint8_t ui8Driver;

    bool bConnected;

    //
    // Bulk IN pipe.
    //
    uint32_t ui32BulkInPipe;
    uint16_t ui16PipeSizeIn;
    tUSBCallback pfnInCallback;
    void *pvInBuffer;

    //
    // Bulk OUT pipe.
    //
    uint32_t ui32BulkOutPipe;
    uint16_t ui16PipeSizeOut;
    tUSBCallback pfnOutCallback;
    void *pvOutBuffer;

    //
    // Interrupt IN pipe.
    //
    uint32_t ui32IntInPipe;
    uint16_t ui16PipeSizeIntIn;
    tUSBCallback pfnIntInCallback;
    void *pvIntInBuffer;
} tSerialInstance;

#define USB_TRANSFER_SIZE       64

//*****************************************************************************
//
//! Constants for uiConfig param for USBHostSerialSetLineConfig()
//
//*****************************************************************************

#define USBHS_CONF_STOP_M       0x00000003
#define USBHS_CONF_STOP_1       0x00000000
#define USBHS_CONF_STOP_1_5     0x00000001
#define USBHS_CONF_STOP_2       0x00000002

#define USBHS_CONF_PAR_M        0x00000070
#define USBHS_CONF_PAR_NONE     0x00000000
#define USBHS_CONF_PAR_EVEN     0x00000010
#define USBHS_CONF_PAR_ODD      0x00000020
#define USBHS_CONF_PAR_MARK     0x00000030
#define USBHS_CONF_PAR_SPACE    0x00000040

#define USBHS_CONF_DATA_M       0x00000F00
#define USBHS_CONF_DATA_5       0x00000500
#define USBHS_CONF_DATA_6       0x00000600
#define USBHS_CONF_DATA_7       0x00000700
#define USBHS_CONF_DATA_8       0x00000800

//*****************************************************************************
//
//! Constants for control line state bits
//
//*****************************************************************************

#define USBHS_CONTROL_CTS       0x00000001
#define USBHS_CONTROL_DSR       0x00000002
#define USBHS_CONTROL_RI        0x00000004
#define USBHS_CONTROL_DCD       0x00000008
#define USBHS_CONTROL_DTR       0x00000010
#define USBHS_CONTROL_RTS       0x00000020

//*****************************************************************************
//
//! Constants for flow control values
//
//*****************************************************************************

#define USBHS_FLOW_DTR_LOW      0x00000000
#define USBHS_FLOW_DTR_HIGH     0x00000001
#define USBHS_FLOW_DTR_AUTO     0x00000002

#define USBHS_FLOW_CTS_READ     0x00000000
#define USBHS_FLOW_CTS_HS       0x00000008

#define USBHS_FLOW_DSR_READ     0x00000000
#define USBHS_FLOW_DSR_HS       0x00000010

#define USBHS_FLOW_DCD_READ     0x00000000
#define USBHS_FLOW_DCD_HS       0x00000020

#define USBHS_FLOW_DSR_SENS     0x00000040

// TODO: XON/XOFF

//*****************************************************************************
//
// Prototypes.
//
//*****************************************************************************
extern uint32_t USBHostSerialInit(tUSBCallback pfnCallback);

extern uint32_t USBHostSerialSetupInstance(tSerialInstance *psSerialInstance,
                                           tUSBCallback pfnCallback, void *pvRxBuffer);

extern void USBHostSerialScheduleWrite(tSerialInstance *psSerialInstance, uint8_t *pui8Data,
                                 uint32_t ui32Size);

extern uint16_t USBHostSerialReadDataCount(tSerialInstance *psSerialInstance);

extern uint32_t USBHostSerialInitNewDevice(tSerialInstance *psSerialInstance);

extern uint32_t USBHostSerialSetLineConfig(tSerialInstance *psSerialInstance, uint32_t ui32Baud, uint32_t ui32Coding);
extern uint32_t USBHostSerialGetBaud(tSerialInstance *psSerialInstance);
extern uint32_t USBHostSerialGetCoding(tSerialInstance *psSerialInstance);
extern uint32_t USBHostSerialSetControlLineState(tSerialInstance *psSerialInstance, uint32_t ui32Control);
extern uint32_t USBHostSerialGetControlLineState(tSerialInstance *psSerialInstance);
extern uint32_t USBHostSerialSetFlow(tSerialInstance *psSerialInstance, uint32_t ui32Flow);
extern uint32_t USBHostSerialBreakSet(tSerialInstance *psSerialInstance);
extern uint32_t USBHostSerialBreakClear(tSerialInstance *psSerialInstance);


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


#endif /* USBHSERIAL_H_ */
