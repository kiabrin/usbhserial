//*****************************************************************************
//
// usbhserialcp210x.c - USBLib serial host driver for CP210x devices
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
#include "usbhserialcp210x.h"

#define CPCDC_IFC_ENABLE    0x00
#define CPCDC_SET_BAUDDIV   0x01
#define CPCDC_GET_BAUDDIV   0x02
#define CPCDC_SET_BAUDRATE  0x1E
#define CPCDC_GET_BAUDRATE  0x1D
#define CPCDC_SET_LINE_CTL  0x03
#define CPCDC_GET_LINE_CTL  0x04
#define CPCDC_SET_MHS       0x07
#define CPCDC_GET_MDMSTS    0x08
#define CPCDC_SET_FLOW      0x13
#define CPCDC_GET_FLOW      0x14
#define CPCDC_GET_COMM_STATUS   0x10
#define CPCDC_PURGE         0x12


uint32_t USBHSerialCPInit(tSerialInstance *psSerialInstance)
{
    tUSBRequest sSetupPacket;

    //
    // This is a Class specific interface OUT request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_VENDOR |
                                 USB_RTYPE_INTERFACE;
    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = CPCDC_IFC_ENABLE;
    sSetupPacket.wValue = 1;

    //
    // Set this on interface 0.
    //
    sSetupPacket.wIndex = 0;

    //
    // This is always 7 for this request.
    //
    sSetupPacket.wLength = 0;

    USBHCDControlTransfer(0, &sSetupPacket, psSerialInstance->psDevice,
                                 0, 0, MAX_PACKET_SIZE_EP0);

    return (0);
}

uint32_t USBHSerialCPSetBaud(tSerialInstance *psSerialInstance, uint32_t ui32Baud)
{
    tUSBRequest sSetupPacket;

    //
    // This is a Class specific interface OUT request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_VENDOR |
                                 USB_RTYPE_INTERFACE;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = CPCDC_SET_BAUDRATE;
    sSetupPacket.wValue = 0;

    //
    // Set this on interface 0.
    //
    sSetupPacket.wIndex = 0;

    //
    // This is always 7 for this request.
    //
    sSetupPacket.wLength = 0x04;

    //
    // Put the setup packet in the buffer.
    // This request includes an OUT transaction and an IN transaction.
    // The OUT transaction is the line coding structure of length 7 bytes.
    //
    USBHCDControlTransfer(0, &sSetupPacket, psSerialInstance->psDevice,
                                 (uint8_t *)&ui32Baud, 0x04, MAX_PACKET_SIZE_EP0);
    return 0;
}

uint32_t USBHSerialCPGetBaud(tSerialInstance *psSerialInstance)
{
    tUSBRequest sSetupPacket;
    uint32_t ui32Bytes;
    uint32_t ui32Baud;

    sSetupPacket.bmRequestType = USB_RTYPE_DIR_IN | USB_RTYPE_VENDOR |
                                 USB_RTYPE_INTERFACE;
    sSetupPacket.wValue = 0;
    sSetupPacket.wIndex = 0;
    sSetupPacket.bRequest = CPCDC_GET_BAUDRATE;
    sSetupPacket.wLength = 4;
    ui32Bytes = (USBHCDControlTransfer(0, &sSetupPacket,
                                       psSerialInstance->psDevice,
                                       (uint8_t *)&ui32Baud, 0x04, MAX_PACKET_SIZE_EP0));
    return ui32Baud;
}

uint32_t USBHSerialCPSetCoding(tSerialInstance *psSerialInstance, uint32_t ui32Coding)
{
    tUSBRequest sSetupPacket;

    //
    // This is a Class specific interface OUT request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_VENDOR |
                                 USB_RTYPE_INTERFACE;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = CPCDC_SET_LINE_CTL;
    sSetupPacket.wValue = 0;

    //
    // Set this on interface 0.
    //
    sSetupPacket.wIndex = 0;

    //
    // This is always 7 for this request.
    //
    sSetupPacket.bRequest = CPCDC_SET_LINE_CTL;
    sSetupPacket.wValue = 0x0800 + (uint16_t)(ui32Coding & (USBHS_CONF_STOP_M + USBHS_CONF_PAR_M + USBHS_CONF_DATA_M));
    sSetupPacket.wLength = 0;
    USBHCDControlTransfer(0, &sSetupPacket, psSerialInstance->psDevice,
                                 0, 0, MAX_PACKET_SIZE_EP0);

    return (0);

}

uint32_t USBHSerialCPGetCoding(tSerialInstance *psSerialInstance)
{
    tUSBRequest sSetupPacket;
    uint32_t ui32Bytes;
    uint16_t ui16Coding;

    sSetupPacket.bmRequestType = USB_RTYPE_DIR_IN | USB_RTYPE_VENDOR |
                                 USB_RTYPE_INTERFACE;
    sSetupPacket.bRequest = CPCDC_GET_BAUDDIV;
    sSetupPacket.wValue = 0;
    sSetupPacket.wIndex = 0;
    sSetupPacket.bRequest = CPCDC_GET_LINE_CTL;
    sSetupPacket.wLength = 2;
    ui32Bytes = (USBHCDControlTransfer(0, &sSetupPacket,
                                       psSerialInstance->psDevice,
                                       (uint8_t *)&ui16Coding, 0x02, MAX_PACKET_SIZE_EP0));

    return (uint32_t)ui16Coding;

}

uint32_t USBHSerialCPSetControlLineState(tSerialInstance *psSerialInstance, uint32_t ui32Control)
{
    // TODO:
    return 0;
}

uint32_t USBHSerialCPGetControlLineState(tSerialInstance *psSerialInstance)
{
    // TODO:
    return 0;
}

uint32_t USBHSerialCPSetFlow(tSerialInstance *psSerialInstance, uint32_t ui32Flow)
{
    tUSBRequest sSetupPacket;
    uint8_t pui8Bytes[0x10];
    sSetupPacket.bRequest = CPCDC_SET_FLOW;
    sSetupPacket.wValue = 0;
    sSetupPacket.wIndex = 0;
    sSetupPacket.wLength = 0x10;
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_VENDOR |
                                 USB_RTYPE_INTERFACE;
    memset(pui8Bytes, 0, 0x10);
    pui8Bytes[0] = (uint8_t)(ui32Flow & 0xFF);

    USBHCDControlTransfer(0, &sSetupPacket, psSerialInstance->psDevice,
                                 pui8Bytes, 0x10, MAX_PACKET_SIZE_EP0);

    return 0;

}

uint32_t USBHSerialCPBreakSet(tSerialInstance *psSerialInstance)
{
    // TODO:
    return 0;
}

uint32_t USBHSerialCPBreakClear(tSerialInstance *psSerialInstance)
{
    // TODO:
    return 0;
}



