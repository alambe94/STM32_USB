/**
  ******************************************************************************
  * @file    usbd_msc_cdc.c
  *
  * @verbatim
  *
  *          ===================================================================
  *                                MSC Class  Description
  *          ===================================================================
  *           This module manages the MSC class V1.0 following the "Universal
  *           Serial Bus Mass Storage Class (MSC) Bulk-Only Transport (BOT) Version 1.0
  *           Sep. 31, 1999".
  *           This driver implements the following aspects of the specification:
  *             - Bulk-Only Transport protocol
  *             - Subclass : SCSI transparent command set (ref. SCSI Primary Commands - 3 (SPC-3))
  *
  *          ===================================================================
  *                                CDC Class Description
  *          ===================================================================
  *           This driver manages the "Universal Serial Bus Class Definitions for Communications Devices
  *           Revision 1.2 November 16, 2007" and the sub-protocol specification of "Universal Serial Bus
  *           Communications Class Subclass Specification for PSTN Devices Revision 1.2 February 9, 2007"
  *           This driver implements the following aspects of the specification:
  *             - Device descriptor management
  *             - Configuration descriptor management
  *             - Enumeration as CDC device with 2 data endpoints (IN and OUT) and 1 command endpoint (IN)
  *             - Requests management (as described in section 6.2 in specification)
  *             - Abstract Control Model compliant
  *             - Union Functional collection (using 1 IN endpoint for control)
  *             - Data interface class
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_cdc.h"
#include "usbd_msc.h"
#include "usbd_cdc.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup MSC_CDC_CORE
  * @brief Mass storage core module
  * @{
  */

/** @defgroup MSC_CDC_CORE_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */

/** @defgroup MSC_CDC_CORE_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup MSC_CDC_CORE_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup MSC_CDC_CORE_Private_FunctionPrototypes
  * @{
  */
uint8_t USBD_MSC_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t USBD_MSC_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t USBD_MSC_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
uint8_t USBD_MSC_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
uint8_t USBD_MSC_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
uint8_t USBD_MSC_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev);

uint8_t *USBD_MSC_CDC_GetHSCfgDesc(uint16_t *length);
uint8_t *USBD_MSC_CDC_GetFSCfgDesc(uint16_t *length);
uint8_t *USBD_MSC_CDC_GetOtherSpeedCfgDesc(uint16_t *length);
uint8_t *USBD_MSC_CDC_GetDeviceQualifierDescriptor(uint16_t *length);

/**
  * @}
  */

/** @defgroup MSC_CDC_CORE_Private_Variables
  * @{
  */

USBD_ClassTypeDef USBD_MSC_CDC =
    {
        USBD_MSC_CDC_Init,
        USBD_MSC_CDC_DeInit,
        USBD_MSC_CDC_Setup,
        NULL,                     /*EP0_TxSent*/
        USBD_MSC_CDC_EP0_RxReady, /*EP0_RxReady*/
        USBD_MSC_CDC_DataIn,
        USBD_MSC_CDC_DataOut,
        NULL, /*SOF */
        NULL,
        NULL,
        USBD_MSC_CDC_GetFSCfgDesc,
        USBD_MSC_CDC_GetFSCfgDesc,
        USBD_MSC_CDC_GetFSCfgDesc,
        USBD_MSC_CDC_GetDeviceQualifierDescriptor,
};

/* USB Mass storage device Configuration Descriptor */
/* All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
__ALIGN_BEGIN static uint8_t USBD_MSC_CDC_CfgFSDesc[USB_MSC_CDC_CONFIG_DESC_SIZ] __ALIGN_END =
    {
        0x09,                        /* bLength: Configuation Descriptor size */
        USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
        USB_MSC_CDC_CONFIG_DESC_SIZ,
        0x00,
        0x03, /* bNumInterfaces: 1 interface */
        0x01, /* bConfigurationValue: */
        0x04, /* iConfiguration: */
        0xC0, /* bmAttributes: */
        0x32, /* MaxPower 100 mA */

        /********************  Mass Storage interface ********************/
        0x09, /* bLength: Interface Descriptor size */
        0x04, /* bDescriptorType: */
        0x00, /* bInterfaceNumber: Number of Interface */
        0x00, /* bAlternateSetting: Alternate setting */
        0x02, /* bNumEndpoints*/
        0x08, /* bInterfaceClass: MSC Class */
        0x06, /* bInterfaceSubClass : SCSI transparent*/
        0x50, /* nInterfaceProtocol */
        0x05, /* iInterface: */

        /********************  Mass Storage Endpoints ********************/
        0x07,          /* Endpoint descriptor length = 7 */
        0x05,          /* Endpoint descriptor type */
        MSC_EPIN_ADDR, /* Endpoint address (IN, address 1) */
        0x02,          /* Bulk endpoint type */
        LOBYTE(MSC_MAX_FS_PACKET),
        HIBYTE(MSC_MAX_FS_PACKET),
        0x00, /* Polling interval in milliseconds */

        0x07,           /* Endpoint descriptor length = 7 */
        0x05,           /* Endpoint descriptor type */
        MSC_EPOUT_ADDR, /* Endpoint address (OUT, address 1) */
        0x02,           /* Bulk endpoint type */
        LOBYTE(MSC_MAX_FS_PACKET),
        HIBYTE(MSC_MAX_FS_PACKET),
        0x00, /* Polling interval in milliseconds */

        /********************  CDC block ********************/
        /******** IAD to associate the two CDC interfaces */
        0x08, /* bLength */
        0x0B, /* bDescriptorType */
        0x01, /* bFirstInterface */
        0x02, /* bInterfaceCount */
        0x02, /* bFunctionClass */
        0x02, /* bFunctionSubClass */
        0x01, /* bFunctionProtocol */
        0x00, /* iFunction (Index of string descriptor describing this function) */

        /* Interface Descriptor */
        0x09,                    /* bLength: Interface Descriptor size */
        USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface */
        0x01,                    /* bInterfaceNumber: Number of Interface */
        0x00,                    /* bAlternateSetting: Alternate setting */
        0x01,                    /* bNumEndpoints: One endpoints used */
        0x02,                    /* bInterfaceClass: Communication Interface Class */
        0x02,                    /* bInterfaceSubClass: Abstract Control Model */
        0x01,                    /* bInterfaceProtocol: Common AT commands */
        0x00,                    /* iInterface: */

        /* Header Functional Descriptor */
        0x05, /* bLength: Endpoint Descriptor size */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x00, /* bDescriptorSubtype: Header Func Desc */
        0x10, /* bcdCDC: spec release number */
        0x01,

        /* Call Management Functional Descriptor */
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x01, /* bDescriptorSubtype: Call Management Func Desc */
        0x00, /* bmCapabilities: D0+D1 */
        0x02, /* bDataInterface: 1 */

        /* ACM Functional Descriptor */
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */

        /* Union Functional Descriptor */
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x06, /* bDescriptorSubtype: Union func desc */
        0x01, /* bMasterInterface: Communication class interface */
        0x02, /* bSlaveInterface0: Data Class Interface */

        /* Endpoint 2 Descriptor */
        0x07,                        /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
        CDC_CMD_EP,                  /* bEndpointAddress */
        0x03,                        /* bmAttributes: Interrupt */
        LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_CMD_PACKET_SIZE),
        CDC_FS_BINTERVAL, /* bInterval: */
        /*---------------------------------------------------------------------------*/

        /* Data class interface descriptor */
        0x09,                    /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
        0x02,                    /* bInterfaceNumber: Number of Interface */
        0x00,                    /* bAlternateSetting: Alternate setting */
        0x02,                    /* bNumEndpoints: Two endpoints used */
        0x0A,                    /* bInterfaceClass: CDC */
        0x00,                    /* bInterfaceSubClass: */
        0x00,                    /* bInterfaceProtocol: */
        0x00,                    /* iInterface: */

        /* Endpoint OUT Descriptor */
        0x07,                                /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,              /* bDescriptorType: Endpoint */
        CDC_OUT_EP,                          /* bEndpointAddress */
        0x02,                                /* bmAttributes: Bulk */
        LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
        0x00, /* bInterval: ignore for Bulk transfer */

        /* Endpoint IN Descriptor */
        0x07,                                /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,              /* bDescriptorType: Endpoint */
        CDC_IN_EP,                           /* bEndpointAddress */
        0x02,                                /* bmAttributes: Bulk */
        LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
        0x00};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_MSC_CDC_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
    {
        USB_LEN_DEV_QUALIFIER_DESC,
        USB_DESC_TYPE_DEVICE_QUALIFIER,
        0x00,
        0x02,
        0x00,
        0x00,
        0x00,
        0x40,
        0x01,
        0x00,
};
/**
  * @}
  */

/** @defgroup MSC_CDC_CORE_Private_Functions
  * @{
  */

/**
  * @brief  USBD_MSC_CDC_Init
  *         Initialize  the mass storage configuration
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status
  */
uint8_t USBD_MSC_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  /* MSC initialization */
  uint8_t ret = USBD_MSC_Init(pdev, cfgidx);
  if (ret != USBD_OK)
    return ret;

  return USBD_CDC_Init(pdev, cfgidx);
  return USBD_OK;
}

/**
  * @brief  USBD_MSC_CDC_DeInit
  *         DeInitilaize  the mass storage configuration
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status
  */
uint8_t USBD_MSC_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  USBD_MSC_DeInit(pdev, cfgidx);
  USBD_CDC_DeInit(pdev, cfgidx);

  return USBD_OK;
}
/**
* @brief  USBD_MSC_CDC_Setup
*         Handle the MSC specific requests
* @param  pdev: device instance
* @param  req: USB request
* @retval status
*/
uint8_t USBD_MSC_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  /** if interface is msc or endpoint belong to msc*/

  if (((req->bmRequest & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_INTERFACE && req->wIndex == 0x00) ||
      ((req->bmRequest & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_ENDPOINT && ((req->wIndex & 0x7F) == 0x01)))
  {
    return USBD_MSC_Setup(pdev, req);
  }

  return USBD_CDC_Setup(pdev, req);
}

/**
* @brief  USBD_MSC_CDC_DataIn
*         handle data IN Stage
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval status
*/
uint8_t USBD_MSC_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  if (epnum == 0x01)
  {
    return USBD_MSC_DataIn(pdev, epnum);
  }

  return USBD_CDC_DataIn(pdev, epnum);
}

/**
* @brief  USBD_MSC_CDC_DataOut
*         handle data OUT Stage
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval status
*/
uint8_t USBD_MSC_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  if (epnum == 0x01)
  {
    return USBD_MSC_DataOut(pdev, epnum);
  }
  return USBD_MSC_DataOut(pdev, epnum);
}

/**
  * @brief  USBD_MSC_CDC_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_MSC_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  return USBD_CDC_EP0_RxReady(pdev);
}

/**
* @brief  USBD_MSC_CDC_GetFSCfgDesc
*         return configuration descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_MSC_CDC_GetFSCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_MSC_CDC_CfgFSDesc);

  return USBD_MSC_CDC_CfgFSDesc;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_MSC_CDC_GetDeviceQualifierDescriptor(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_MSC_CDC_DeviceQualifierDesc);

  return USBD_MSC_CDC_DeviceQualifierDesc;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
