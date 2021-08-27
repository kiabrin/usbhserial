//*****************************************************************************
//
// usbhserialcdc.c - USBLib serial host driver for CDC devices
//
// Copyright (c) 2008-2019 Texas Instruments Incorporated.  All rights reserved.
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

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/usbcdc.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhostpriv.h"
#include "usbhserial.h"
#include "usbhserialdriver.h"
#include "usbhserialcdc.h"

#define USBREQ_GET_LINE_CODING  0x21
#define USBREQ_SET_LINE_CODING  0x20
#define USBREQ_SET_CONTROL_LINE_STATE  0x22


uint32_t USBHSerialCDCInit(tSerialInstance *psSerialInstance)
{
    return(0);
}

uint32_t USBHSerialCDCGetBaudCoding(tSerialInstance *psSerialInstance, uint8_t *pui8Buffer)
{
    tUSBRequest sSetupPacket;
    uint32_t ui32Bytes;

    //
    // This is a Class specific interface OUT request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_IN | USB_RTYPE_CLASS |
                                 USB_RTYPE_INTERFACE;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_GET_LINE_CODING;
    sSetupPacket.wValue = 0;

    //
    // Set this on interface 0.
    //
    sSetupPacket.wIndex = 0;

    //
    // This is always 7 for this request.
    //
    sSetupPacket.wLength = 0x07;

    //
    // Put the setup packet in the buffer.
    //
    ui32Bytes = (USBHCDControlTransfer(0, &sSetupPacket,
                                       psSerialInstance->psDevice,
                                       pui8Buffer, 0x07, MAX_PACKET_SIZE_EP0));

    return ui32Bytes;
}

uint32_t USBHSerialCDCSetBaud(tSerialInstance *psSerialInstance, uint32_t ui32Baud)
{
    uint8_t pui8Buffer[7];

    USBHSerialCDCGetBaudCoding(psSerialInstance, pui8Buffer);
    *((uint32_t *)pui8Buffer) = ui32Baud;

    tUSBRequest sSetupPacket;

    //
    // This is a Class specific interface OUT request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS |
                                 USB_RTYPE_INTERFACE;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_SET_LINE_CODING;
    sSetupPacket.wValue = 0;

    //
    // Set this on interface 0.
    //
    sSetupPacket.wIndex = 0;

    //
    // This is always 7 for this request.
    //
    sSetupPacket.wLength = 0x07;

    //
    // Put the setup packet in the buffer.
    // This request includes an OUT transaction and an IN transaction.
    // The OUT transaction is the line coding structure of length 7 bytes.
    //
    USBHCDControlTransfer(0, &sSetupPacket, psSerialInstance->psDevice,
                          pui8Buffer, 0x07, MAX_PACKET_SIZE_EP0);

    return (0);

}

uint32_t USBHSerialCDCGetBaud(tSerialInstance *psSerialInstance)
{
    uint8_t pui8Buffer[7];

    USBHSerialCDCGetBaudCoding(psSerialInstance, pui8Buffer);
    return *((uint32_t *)pui8Buffer);

}

uint32_t USBHSerialCDCSetCoding(tSerialInstance *psSerialInstance, uint32_t ui32Coding)
{
    uint8_t pui8Buffer[7];

    USBHSerialCDCGetBaudCoding(psSerialInstance, pui8Buffer);
    pui8Buffer[4] = (ui32Coding & USBHS_CONF_STOP_M);
    pui8Buffer[5] = ((ui32Coding & USBHS_CONF_PAR_M) >> 4);
    pui8Buffer[6] = ((ui32Coding & USBHS_CONF_DATA_M) >> 8);

    tUSBRequest sSetupPacket;

    //
    // This is a Class specific interface OUT request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS |
                                 USB_RTYPE_INTERFACE;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_SET_LINE_CODING;
    sSetupPacket.wValue = 0;

    //
    // Set this on interface 0.
    //
    sSetupPacket.wIndex = 0;

    //
    // This is always 7 for this request.
    //
    sSetupPacket.wLength = 0x07;

    //
    // Put the setup packet in the buffer.
    // This request includes an OUT transaction and an IN transaction.
    // The OUT transaction is the line coding structure of length 7 bytes.
    //
    USBHCDControlTransfer(0, &sSetupPacket, psSerialInstance->psDevice,
                          pui8Buffer, 0x07, MAX_PACKET_SIZE_EP0);

    return (0);
}

uint32_t USBHSerialCDCGetCoding(tSerialInstance *psSerialInstance)
{
    uint8_t pui8Buffer[7];

    USBHSerialCDCGetBaudCoding(psSerialInstance, pui8Buffer);
    uint32_t ui32Ret = 0;
    ui32Ret |= pui8Buffer[6]; ui32Ret <<= 4;
    ui32Ret |= pui8Buffer[5]; ui32Ret <<= 4;
    ui32Ret |= pui8Buffer[4];
    return ui32Ret;
}

uint32_t USBHSerialCDCSetControlLineState(tSerialInstance *psSerialInstance, uint32_t ui32Control)
{
    // TODO: full control
    tUSBRequest sSetupPacket;

    //
    // This is a Class specific interface OUT request (from host to device).
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS |
                                 USB_RTYPE_INTERFACE;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_SET_CONTROL_LINE_STATE;
    if(ui32Control & USBHS_CONTROL_RTS)
    {
        sSetupPacket.wValue = 0x03; // Activate carrier, DTE present.
    }
    else
    {
        sSetupPacket.wValue = 0x00; // Deactivate carrier, DTE not present.
    }

    //
    // Set this on interface 0.
    //
    sSetupPacket.wIndex = 0;

    //
    // This is always 0 for this request.
    //
    sSetupPacket.wLength = 0x0;

    //
    // Put the setup packet in the buffer.
    // This request includes an OUT transaction and an IN transaction.
    // The OUT transaction is the line coding structure of length 7 bytes.
    //
    USBHCDControlTransfer(0, &sSetupPacket, psSerialInstance->psDevice,
                                 0, 0, MAX_PACKET_SIZE_EP0);

    return (0);

}

uint32_t USBHSerialCDCGetControlLineState(tSerialInstance *psSerialInstance)
{
    // TODO:
    return 0;
}

uint32_t USBHSerialCDCSetFlow(tSerialInstance *psSerialInstance, uint32_t ui32Flow)
{
    // TODO:
    return 0;
}

uint32_t USBHSerialCDCBreakSet(tSerialInstance *psSerialInstance)
{
    // TODO:
    return 0;

}

uint32_t USBHSerialCDCBreakClear(tSerialInstance *psSerialInstance)
{
    // TODO:
    return 0;
}

