//*****************************************************************************
//
// usbhcdc.c - This file contains the USB CDC host class driver.
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

void *SerialDriverOpen(tUSBHostDevice *psDevice);
void SerialDriverClose(void *pvInstance);

//*****************************************************************************
//
//! This constant global structure defines the CDC Class Driver that is
//! provided with the USB library.
//
//*****************************************************************************
const tUSBHostClassDriver g_sUSBSerialCDCClassDriver =
{
    USB_CLASS_CDC,
    SerialDriverOpen,
    SerialDriverClose,
    0
};

const tUSBHostClassDriver g_sUSBSerialVendorClassDriver =
{
    USB_CLASS_VEND_SPECIFIC,
    SerialDriverOpen,
    SerialDriverClose,
    0
};

//*****************************************************************************
//
// Declare the USB Events driver interface.
//
//*****************************************************************************
DECLARE_EVENT_DRIVER(g_sUSBEventDriver, 0, 0, USBHCDEvents);

//*****************************************************************************
//
// The global that holds all of the host drivers in use in the application.
// In this case, only the CDC class is loaded.
//
//*****************************************************************************
static tUSBHostClassDriver const * const g_ppHostClassDrivers[] =
{
    &g_sUSBSerialCDCClassDriver,
    &g_sUSBSerialVendorClassDriver,
    &g_sUSBEventDriver
};

//*****************************************************************************
//
// This global holds the number of class drivers in the g_ppHostClassDrivers
// list.
//
//*****************************************************************************
static const uint32_t g_ui32NumHostClassDrivers =
    sizeof(g_ppHostClassDrivers) / sizeof(tUSBHostClassDriver *);

tUSBCallback g_pfnGlobalAppCB = 0;

//*****************************************************************************
//
// This is the generic callback from host stack.
//
// \param pvData is actually a pointer to a tEventInfo structure.
//
// This function will be called to inform the application when a USB event has
// occurred that is outside those related to the CDC device.  At this
// point this is used to detect unsupported devices being inserted and removed.
// It is also used to inform the application when a power fault has occurred.
// This function is required when the g_USBGenericEventDriver is included in
// the host controller driver array that is passed in to the
// USBHCDRegisterDrivers() function.
//
// \return None.
//
//*****************************************************************************
void
USBHCDEvents(void *pvData)
{
    tEventInfo *pEventInfo;

    //
    // Cast this pointer to its actual type.
    //
    pEventInfo = (tEventInfo *)pvData;

    switch(pEventInfo->ui32Event)
    {
        //
        // New CDC device detected.
        //
        case USB_EVENT_CONNECTED:
        {

            break;
        }
        //
        // Unsupported device detected.
        //
        case USB_EVENT_UNKNOWN_CONNECTED:
        {
            if(g_pfnGlobalAppCB != 0)
            {
                g_pfnGlobalAppCB(0, USB_EVENT_UNKNOWN_CONNECTED, 0, 0);
            }
            break;
        }
        //
        // Device has been unplugged.
        //
        case USB_EVENT_DISCONNECTED:
        {
            break;
        }
        //
        // Power Fault has occurred.
        //
        case USB_EVENT_POWER_FAULT:
        {
            if(g_pfnGlobalAppCB != 0)
            {
                g_pfnGlobalAppCB(0, USB_EVENT_POWER_FAULT, 0, 0);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

extern tUSBSerialDriver g_psDrivers[];
extern uint8_t g_ui8NumDrivers;

tSerialInstance g_psInstances[10];
uint8_t g_ui8NumInstances = 0;

uint8_t g_pui8TmpBuf[USB_TRANSFER_SIZE];

//*****************************************************************************
//
//! This function handles event callbacks from the USB serial driver layer.
//!
//! \param pvCdc is the pointer that was passed in to the USBHCDCOpen()
//! call.
//! \param ui32Event is the event that has been passed up from the CDC driver.
//! \param ui32MsgParam has meaning related to the \e ui32Event that occurred.
//! \param pvMsgData has meaning related to the \e ui32Event that occurred.
//!
//! This function will receive all event updates from the CDC driver layer.
//!
//! \return Non-zero values should be assumed to indicate an error condition.
//
//*****************************************************************************
void USBHSerialCallback(uint32_t ui32Pipe, uint32_t ui32Event)
{
    tSerialInstance *psInstance = 0;
    int i;

    switch (ui32Event)
    {
        //
        // Called when new data is available on the interrupt IN pipe.
        //
        case USB_EVENT_RX_AVAILABLE:
        {
            //
            // Find instance using this pipe for IN endpoint
            //
            for(i = 0; i < g_ui8NumInstances; i++)
            {
                if(g_psInstances[i].bConnected && g_psInstances[i].ui32BulkInPipe == ui32Pipe)
                {
                    psInstance = g_psInstances + i;
                    break;
                }
            }
            //
            // Check for how much data has been received.
            //
            uint16_t ui16Size = USBHCDPipeCurrentSizeGet(ui32Pipe);

            //
            // Read out the data into the USB IN buffer.
            // Call this even if read size is 0 to reset pipe state
            // Read to temporary buffer if application did not provide buffer
            //
            USBHCDPipeReadNonBlocking(ui32Pipe, (psInstance && psInstance->pvInBuffer) ? psInstance->pvInBuffer : g_pui8TmpBuf, (uint32_t)ui16Size);

            //
            // If the callback exists then call it.
            //
            if(psInstance && psInstance->pfnCallback != 0 && ui16Size != 0)
            {
                psInstance->ui16PipeSizeIn = ui16Size;
                //
                // Notify the application about received data.
                //
                psInstance->pfnCallback(psInstance, ui32Event,
                                          0,
                                          psInstance->pvCBData);
            }

            break;
        }
        //
        // Called when data has finished transferring on the interrupt OUT pipe.
        //
        case USB_EVENT_TX_COMPLETE:
        {
            //
            // Find instance using this pipe for OUT endpoint
            //
            for(i = 0; i < g_ui8NumInstances; i++)
            {
                if(g_psInstances[i].bConnected && g_psInstances[i].ui32BulkOutPipe == ui32Pipe)
                {
                    psInstance = g_psInstances + i;
                    break;
                }
            }
            //
            // If the callback exists then call it.
            //
            if(psInstance && psInstance->pfnCallback != 0)
            {
                //
                // Notify the application that the TX Complete occurred.
                //
                psInstance->pfnCallback(psInstance, ui32Event,
                                          0,
                                          psInstance->pvCBData);
            }

            break;
        }

        //
        // Called for polling if pipe is not busy.
        //
        case USB_EVENT_SCHEDULER:
        {
            //
            // Find instance using this pipe for IN endpoint
            //
            for(i = 0; i < g_ui8NumInstances; i++)
            {
                if(g_psInstances[i].bConnected && g_psInstances[i].ui32BulkInPipe == ui32Pipe)
                {
                    psInstance = g_psInstances + i;
                    break;
                }
            }
            //
            // Schedule TX request
            //
            USBHCDPipeSchedule(psInstance->ui32BulkInPipe, 0, 1);

            break;
        }

    }
}

//*****************************************************************************
//
// This function handles callbacks for the Interrupt IN endpoint.
//
//*****************************************************************************
void USBHSerialIntINCallback(uint32_t ulPipe, uint32_t ulEvent)
{
    tSerialInstance *psInstance = 0;
    int i;
    //
    // Handles a request to schedule a new request on the interrupt IN
    // pipe.
    //
    if(ulEvent == USB_EVENT_SCHEDULER)
    {
        //
        // Find instance using this pipe for IN endpoint
        //
        for(i = 0; i < g_ui8NumInstances; i++)
        {
            if(g_psInstances[i].bConnected && g_psInstances[i].ui32BulkInPipe == ulPipe)
            {
                psInstance = g_psInstances + i;
                break;
            }
        }
        //
        // Schedule TX request
        //
        USBHCDPipeSchedule(psInstance->ui32BulkInPipe, 0, 1);
    }

    //
    // Called when new data is available on the interrupt IN pipe.
    //
    if(ulEvent == USB_EVENT_RX_AVAILABLE)
    {
        //
        // Find instance using this pipe for IN endpoint
        //
        for(i = 0; i < g_ui8NumInstances; i++)
        {
            if(g_psInstances[i].bConnected && g_psInstances[i].ui32BulkInPipe == ulPipe)
            {
                psInstance = g_psInstances + i;
                break;
            }
        }
        //
        // Check for how much data has been received.
        //
        uint16_t ui16Size = USBHCDPipeCurrentSizeGet(ulPipe);

        //
        // Read out the data into the USB IN buffer.
        // Call this even if read size is 0 to reset pipe state
        // Read to temporary buffer if application did not provide buffer
        //
        USBHCDPipeReadNonBlocking(ulPipe, (psInstance && psInstance->pvInBuffer) ? psInstance->pvInBuffer : g_pui8TmpBuf, (uint32_t)ui16Size);

        //
        // If the callback exists then call it.
        //
        if(psInstance && psInstance->pfnCallback != 0)
        {
            //
            // Notify the application about received data.
            //
            psInstance->pfnCallback(psInstance, ulEvent,
                                      0,
                                      psInstance->pvCBData);
        }

    }
}

//*****************************************************************************
//
//! This function is used to open an instance of the serial driver.
//!
//! \param psDevice is a pointer to the device information structure.
//!
//! This function will attempt to open an instance of the serial driver based on
//! the information contained in the \e psDevice structure.  This call can fail
//! if there are not sufficient resources to open the device.  The function
//! returns a value that should be passed back into USBSerialClose() when the
//! driver is no longer needed.
//!
//! \return The function will return a pointer to a serial driver instance.
//
//*****************************************************************************
void *SerialDriverOpen(tUSBHostDevice *psDevice)
{
    int32_t i32Idx;
    uint8_t NumOfInterfaces, i, j;
    tEndpointDescriptor *psEndpointDescriptor;
    tInterfaceDescriptor *psInterface;

    NumOfInterfaces = psDevice->psConfigDescriptor->bNumInterfaces;

    psInterface = USBDescGetInterface(psDevice->psConfigDescriptor, 0, 0);

    for(i = 0; i < g_ui8NumDrivers; i++)
    {
        if(g_psDrivers[i].ui32InterfaceClass == psInterface->bInterfaceClass)
        {
            if(((g_psDrivers[i].ui16VID == 0) || (g_psDrivers[i].ui16VID == psDevice->sDeviceDescriptor.idVendor)) &&
               ((g_psDrivers[i].ui16PID == 0) || (g_psDrivers[i].ui16PID == psDevice->sDeviceDescriptor.idProduct)))
            {
                // Consider device is supported by the driver
                tSerialInstance *psInstance = g_psInstances + g_ui8NumInstances;
                //
                // Save the device pointer.
                //
                psInstance->psDevice = psDevice;
                psInstance->bConnected = true;
                psInstance->pfnCallback = 0;
                psInstance->pvInBuffer = 0;
                psInstance->ui8Driver = (uint8_t)i;

                for (j = 0; j < NumOfInterfaces; j++)
                {
                    //
                    // Get the interface descriptor for each interface
                    //
                    psInterface = USBDescGetInterface(psDevice->psConfigDescriptor, j, 0);

                    //
                    // Loop through the endpoints of the device.
                    //
                    for(i32Idx = 0; i32Idx < 3; i32Idx++)
                    {
                        //
                        // Get the interrupt endpoint descriptor
                        //
                        psEndpointDescriptor =
                                USBDescGetInterfaceEndpoint(psInterface, i32Idx,
                                                            psDevice->ui32ConfigDescriptorSize);

                        //
                        // If no more endpoints then break out.
                        //
                        if(psEndpointDescriptor == 0)
                        {
                            break;
                        }

                        //
                        // See if this is a bulk endpoint.
                        //
                        if((psEndpointDescriptor->bmAttributes & USB_EP_ATTR_TYPE_M) ==
                                USB_EP_ATTR_BULK)
                        {
                            //
                            // See if this is bulk IN or bulk OUT.
                            //
                            if(psEndpointDescriptor->bEndpointAddress & USB_EP_DESC_IN)
                            {
                                //
                                // Allocate the USB Pipe for this Bulk IN endpoint.
                                //
                                psInstance->ui32BulkInPipe =
                                        USBHCDPipeAllocSize(0, USBHCD_PIPE_BULK_IN,
                                                            psDevice,
                                                            psEndpointDescriptor->wMaxPacketSize,
                                                            USBHSerialCallback);
                                //
                                // Configure the USB pipe as a Bulk IN endpoint.
                                //
                                USBHCDPipeConfig(psInstance->ui32BulkInPipe,
                                                 psEndpointDescriptor->wMaxPacketSize,
                                                 g_psDrivers[i].bPolling ? g_psDrivers[i].ui32Interval : 0,
                                                 ((psEndpointDescriptor->bEndpointAddress) &
                                                         USB_EP_DESC_NUM_M));
                            }
                            else
                            {
                                //
                                // Allocate the USB Pipe for this Bulk OUT endpoint.
                                //
                                psInstance->ui32BulkOutPipe =
                                        USBHCDPipeAllocSize(0, USBHCD_PIPE_BULK_OUT,
                                                            psDevice,
                                                            psEndpointDescriptor->wMaxPacketSize,
                                                            USBHSerialCallback);
                                //
                                // Configure the USB pipe as a Bulk OUT endpoint.
                                //
                                USBHCDPipeConfig(psInstance->ui32BulkOutPipe,
                                                 psEndpointDescriptor->wMaxPacketSize,
                                                 0, (psEndpointDescriptor->bEndpointAddress &
                                                         USB_EP_DESC_NUM_M));
                            }
                        }

                        if(!g_psDrivers[i].bPolling)
                        {
                            //
                            // See if this is an interrupt endpoint.
                            //
                            if((psEndpointDescriptor->bmAttributes & USB_EP_ATTR_TYPE_M) ==
                                    USB_EP_ATTR_INT)
                            {
                                //
                                // See if this is interrupt IN endpoint.
                                //
                                if(psEndpointDescriptor->bEndpointAddress & USB_EP_DESC_IN)
                                {
                                    //
                                    // Allocate the USB Pipe for this Interrupt IN endpoint.
                                    //
                                    psInstance->ui32IntInPipe =
                                            USBHCDPipeAlloc(0, USBHCD_PIPE_INTR_IN,
                                                            psDevice, USBHSerialIntINCallback);

                                    //
                                    // Configure the USB pipe as a Interrupt IN endpoint.
                                    //
                                    USBHCDPipeConfig(psInstance->ui32IntInPipe,
                                                     psEndpointDescriptor->wMaxPacketSize,
                                                     psEndpointDescriptor->bInterval,
                                                     (psEndpointDescriptor->bEndpointAddress &
                                                             USB_EP_DESC_NUM_M));
                                }
                            }
                        }
                    }
                    //
                    // If global callback exist, call it
                    //
                    if(g_pfnGlobalAppCB != 0)
                    {
                        g_pfnGlobalAppCB(
                                psInstance,
                                USB_EVENT_CONNECTED,
                                0, 0);
                    }

                    //
                    // Increment number of open instances
                    //
                    g_ui8NumInstances++;

                    return (void *)psInstance;
                }
            }
        }
    }

    return 0;
}

//*****************************************************************************
//
//! This function is used to release an instance of the CDC driver.
//!
//! \param pvInstance is an instance pointer that needs to be released.
//!
//! This function will free up any resources in use by the CDC driver instance
//! that is passed in.  The \e pvInstance pointer should be a valid value that
//! was returned from a call to USBCDCOpen().
//!
//! \return None.
//
//*****************************************************************************
void
SerialDriverClose(void *pvInstance)
{
    tSerialInstance *psInst;

    //
    // Get our instance pointer.
    //
    psInst = (tSerialInstance *)pvInstance;

    //
    // Reset the device pointer.
    //
    psInst->psDevice = 0;
    psInst->bConnected = false;

    //
    // Free the Interrupt IN pipe.
    //
    if(psInst->ui32IntInPipe != 0)
    {
        USBHCDPipeFree(psInst->ui32IntInPipe);
    }

    //
    // Free the Bulk IN pipe.
    //
    if(psInst->ui32BulkInPipe != 0)
    {
        USBHCDPipeFree(psInst->ui32BulkInPipe);
    }

    //
    // Free the Bulk OUT pipe.
    //
    if(psInst->ui32BulkOutPipe != 0)
    {
        USBHCDPipeFree(psInst->ui32BulkOutPipe);
    }

    //
    // If the callback exists, call it with a DISCONNECTED event.
    //
    if(psInst->pfnCallback != 0)
    {
        psInst->pfnCallback(psInst, USB_EVENT_DISCONNECTED,
                            0, 0);
    }
}

//*****************************************************************************
//
//! This function is used to initialize serial driver layer.
//!
//! \param pfnCallback is callback function used to handle USB_EVENT_CONNECTED
//!
//! \return None.
//
//*****************************************************************************
uint32_t USBHostSerialInit(tUSBCallback pfnCallback)
{
    //
    // Register the host class drivers.
    //
    USBHCDRegisterDrivers(0, g_ppHostClassDrivers, g_ui32NumHostClassDrivers);

    g_pfnGlobalAppCB = pfnCallback;

    return 0;
}

//*****************************************************************************
//
//! This function is used to initialize serial device instance.
//!
//! \param psSerialInstance is an instance pointer, received by global callback
//! function.
//! \param pfnCallback is callback function for an instance.
//! \param pvRxBuffer is a pointer to receive buffer provided by application.
//!
//! Application should call this function from global callback then received
//! USB_EVENT_CONNECTED event.
//!
//! \return None.
//
//*****************************************************************************
uint32_t USBHostSerialSetupInstance(tSerialInstance *psSerialInstance,
                                           tUSBCallback pfnCallback, void *pvRxBuffer)
{
    psSerialInstance->pfnCallback = pfnCallback;
    psSerialInstance->pvInBuffer = pvRxBuffer;

    return 0;
}

uint32_t USBHostSerialInitNewDevice(tSerialInstance *psSerialInstance)
{
    return g_psDrivers[psSerialInstance->ui8Driver].pfnInit(psSerialInstance);
}

uint32_t USBHostSerialSetLineConfig(tSerialInstance *psSerialInstance, uint32_t ui32Baud, uint32_t ui32Coding)
{
    return g_psDrivers[psSerialInstance->ui8Driver].pfnSetBaud(psSerialInstance, ui32Baud) &&
           g_psDrivers[psSerialInstance->ui8Driver].pfnSetCoding(psSerialInstance, ui32Coding);
}

uint32_t USBHostSerialGetBaud(tSerialInstance *psSerialInstance)
{
    return g_psDrivers[psSerialInstance->ui8Driver].pfnGetBaud(psSerialInstance);
}

uint32_t USBHostSerialGetCoding(tSerialInstance *psSerialInstance)
{
    return g_psDrivers[psSerialInstance->ui8Driver].pfnGetCoding(psSerialInstance);
}

uint32_t USBHostSerialSetControlLineState(tSerialInstance *psSerialInstance, uint32_t ui32Control)
{
    return g_psDrivers[psSerialInstance->ui8Driver].pfnSetControlLineState(psSerialInstance, ui32Control);
}

uint32_t USBHostSerialGetControlLineState(tSerialInstance *psSerialInstance)
{
    return g_psDrivers[psSerialInstance->ui8Driver].pfnGetControlLineState(psSerialInstance);
}

uint32_t USBHostSerialBreakSet(tSerialInstance *psSerialInstance)
{
    return g_psDrivers[psSerialInstance->ui8Driver].pfnBreakSet(psSerialInstance);
}

uint32_t USBHostSerialBreakClear(tSerialInstance *psSerialInstance)
{
    return g_psDrivers[psSerialInstance->ui8Driver].pfnBreakClear(psSerialInstance);
}

//*****************************************************************************
//
//! This function schedules the bulk OUT endpoint to send data.
//!
//! \param psSerialInstance is the value that was returned from the call to
//! USBHCDCOpen().
//! \param pui8Data is the memory buffer storing the data.
//! \param ui32Size is how many bytes of data from \e pui8Buffer should be
//! written to the endpoint.
//!
//! This function will not block for sending but will load the USB FIFO with
//! the provided data.  The amount of data loaded will be limited to what will
//! fit in the FIFO for provided Pipe which in this case is the CDC Bulk Out
//! pipe.
//!
//! \return None
//
//*****************************************************************************
void USBHostSerialScheduleWrite(tSerialInstance *psSerialInstance, uint8_t *pui8Data,
                                 uint32_t ui32Size)
{
    //
    // Schedule the next OUT Pipe transaction.
    //
    USBHCDPipeSchedule(psSerialInstance->ui32BulkOutPipe, pui8Data, ui32Size);
}

uint16_t USBHostSerialReadDataCount(tSerialInstance *psSerialInstance)
{
    return(psSerialInstance->ui16PipeSizeIn);
}





