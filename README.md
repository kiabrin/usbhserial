# usbhserial

usbhserial is a library which can be used on Tiva C microcontrollers to connect to USB serial controllers in USB host mode.

Currently Communication Device Class (CDC) and CP210x based controllers are supported, but adding new chips is quite easy.

# Requirements

usbhserial is a Code Composer Studio library project built on CCS v 9.3. It uses TivaWare 2.2.0.295 driverlib and usblib libraries.

# Building

Import usblib project in CCS, check out include and library paths and build library.

# Adding usbhserial to your project

Your project should use driverlib and usblib as well. Include headers in your main c file:

#include "usbhserial.h"
#include "usbhserialdriver.h"
#include "usbhserialcdc.h"
#include "usbhserialcp210x.h"

Declare the list of used drivers:
{
tUSBSerialDriver g_psDrivers[] =
 {
  DECLARE_USB_SERIAL_CDC_DRIVER,
  DECLARE_USB_SERIAL_CP210X_DRIVER
 };
uint8_t g_ui8NumDrivers = 2;
}

Declare callback functions. First is global callback function receiving events for connected devices and system events^
{
 uint32_t
 CDCSerialGlobalCallback(void *pvCBData, uint32_t ui32Event,
                   uint32_t ui32MsgParam, void *pvMsgData)
 {
     if(ui32Event == USB_EVENT_CONNECTED)
     {
         psInstance = (tSerialInstance *)pvCBData;
         USBHostSerialSetupInstance(psInstance, CDCSerialCallback, g_pUSBPipeBuffer);
         USBHostSerialInitNewDevice(psInstance);
         USBHostSerialSetLineConfig(psInstance, 19200, USBHS_CONF_STOP_1 | USBHS_CONF_PAR_NONE | USBHS_CONF_DATA_8);

         return 0;
     }
     if(ui32Event == USB_EVENT_UNKNOWN_CONNECTED)
     {
         return 0;
     }
 }
}

Second is callback function for device instance events:
{
 uint32_t
 CDCSerialCallback(void *pvCBData, uint32_t ui32Event,
                   uint32_t ui32MsgParam, void *pvMsgData)
 {
     tSerialInstance *psInstance = (tSerialInstance *)pvCBData;
     uint32_t ui32Count;
     uint32_t ui16BytesReceived;

     switch(ui32Event)
     {
         //
         // Data was detected.
         //
         case USB_EVENT_RX_AVAILABLE:
         {
             ui16BytesReceived = USBHostSerialReadDataCount(psInstance);

             printf("Data received: ");
             for (ui32Count = 0; ui32Count < ui16BytesReceived; ui32Count++)
             {
                 printf("%02X ", g_pUSBPipeBuffer[ui32Count]);
             }
             printf("\n");
             break;
         }
         case USB_EVENT_TX_COMPLETE:
         {
             break;
         }
     }
     return 0;
 }
}

In main function  initialize library with 

{
 USBHostSerialInit(CDCSerialGlobalCallback);
}

call, initialize USB controller and run main loop.
