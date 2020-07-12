/**
  ******************************************************************************
  * @file    usbd_dual_CDC.c
  *
  * @verbatim
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
#include "usbd_dual_cdc.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup Dual_CDC_CORE
  * @brief Mass storage core module
  * @{
  */

/** @defgroup Dual_CDC_CORE_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */

/** @defgroup Dual_CDC_CORE_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup Dual_CDC_CORE_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup Dual_CDC_CORE_Private_FunctionPrototypes
  * @{
  */
uint8_t USBD_Dual_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t USBD_Dual_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t USBD_Dual_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
uint8_t USBD_Dual_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
uint8_t USBD_Dual_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
uint8_t USBD_Dual_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev);

uint8_t *USBD_Dual_CDC_GetHSCfgDesc(uint16_t *length);
uint8_t *USBD_Dual_CDC_GetFSCfgDesc(uint16_t *length);
uint8_t *USBD_Dual_CDC_GetOtherSpeedCfgDesc(uint16_t *length);
uint8_t *USBD_Dual_CDC_GetDeviceQualifierDescriptor(uint16_t *length);

/**
  * @}
  */

/** @defgroup Dual_CDC_CORE_Private_Variables
  * @{
  */

USBD_ClassTypeDef USBD_Dual_CDC =
    {
        USBD_Dual_CDC_Init,
        USBD_Dual_CDC_DeInit,
        USBD_Dual_CDC_Setup,
        NULL,                      /*EP0_TxSent*/
        USBD_Dual_CDC_EP0_RxReady, /*EP0_RxReady*/
        USBD_Dual_CDC_DataIn,
        USBD_Dual_CDC_DataOut,
        NULL, /*SOF */
        NULL,
        NULL,
        USBD_Dual_CDC_GetFSCfgDesc,
        USBD_Dual_CDC_GetFSCfgDesc,
        USBD_Dual_CDC_GetFSCfgDesc,
        USBD_Dual_CDC_GetDeviceQualifierDescriptor,
};

/* USB Mass storage device Configuration Descriptor */
/* All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
__ALIGN_BEGIN static uint8_t USBD_Dual_CDC_CfgFSDesc[USB_DUAL_CDC_CONFIG_DESC_SIZ] __ALIGN_END =
    {
        0x09,                        /* bLength: Configuation Descriptor size */
        USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
        USB_DUAL_CDC_CONFIG_DESC_SIZ,
        0x00,
        0x04, /* bNumInterfaces: */
        0x01, /* bConfigurationValue: */
        0x04, /* iConfiguration: */
        0xC0, /* bmAttributes: */
        0x32, /* MaxPower 100 mA */

        /********************  CDC0 block ********************/
        /******** IAD to associate the two CDC interfaces */
        0x08, /* bLength */
        0x0B, /* bDescriptorType */
        0x00, /* bFirstInterface */
        0x02, /* bInterfaceCount */
        0x02, /* bFunctionClass */
        0x02, /* bFunctionSubClass */
        0x01, /* bFunctionProtocol */
        0x00, /* iFunction (Index of string descriptor describing this function) */

        /* Interface Descriptor */
        0x09,                    /* bLength: Interface Descriptor size */
        USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface */
        0x00,                    /* bInterfaceNumber: Number of Interface */
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
        0x01, /* bDataInterface: 1 */

        /* ACM Functional Descriptor */
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */

        /* Union Functional Descriptor */
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x06, /* bDescriptorSubtype: Union func desc */
        0x00, /* bMasterInterface: Communication class interface */
        0x01, /* bSlaveInterface0: Data Class Interface */

        /* Endpoint 2 Descriptor */
        0x07,                        /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
        CDC0_CMD_EP,                 /* bEndpointAddress */
        0x03,                        /* bmAttributes: Interrupt */
        LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_CMD_PACKET_SIZE),
        CDC_FS_BINTERVAL, /* bInterval: */
        /*---------------------------------------------------------------------------*/

        /* Data class interface descriptor */
        0x09,                    /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
        0x01,                    /* bInterfaceNumber: Number of Interface */
        0x00,                    /* bAlternateSetting: Alternate setting */
        0x02,                    /* bNumEndpoints: Two endpoints used */
        0x0A,                    /* bInterfaceClass: CDC */
        0x00,                    /* bInterfaceSubClass: */
        0x00,                    /* bInterfaceProtocol: */
        0x00,                    /* iInterface: */

        /* Endpoint OUT Descriptor */
        0x07,                                /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,              /* bDescriptorType: Endpoint */
        CDC0_OUT_EP,                         /* bEndpointAddress */
        0x02,                                /* bmAttributes: Bulk */
        LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
        0x00, /* bInterval: ignore for Bulk transfer */

        /* Endpoint IN Descriptor */
        0x07,                                /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,              /* bDescriptorType: Endpoint */
        CDC0_IN_EP,                          /* bEndpointAddress */
        0x02,                                /* bmAttributes: Bulk */
        LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
        0x00,

        /********************  CDC1 block ********************/
        /******** IAD to associate the two CDC interfaces */
        0x08, /* bLength */
        0x0B, /* bDescriptorType */
        0x03, /* bFirstInterface */
        0x02, /* bInterfaceCount */
        0x02, /* bFunctionClass */
        0x02, /* bFunctionSubClass */
        0x01, /* bFunctionProtocol */
        0x00, /* iFunction (Index of string descriptor describing this function) */

        /* Interface Descriptor */
        0x09,                    /* bLength: Interface Descriptor size */
        USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface */
        0x03,                    /* bInterfaceNumber: Number of Interface */
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
        0x04, /* bDataInterface: 1 */

        /* ACM Functional Descriptor */
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */

        /* Union Functional Descriptor */
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x06, /* bDescriptorSubtype: Union func desc */
        0x03, /* bMasterInterface: Communication class interface */
        0x04, /* bSlaveInterface0: Data Class Interface */

        /* Endpoint 2 Descriptor */
        0x07,                        /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
        CDC1_CMD_EP,                 /* bEndpointAddress */
        0x03,                        /* bmAttributes: Interrupt */
        LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_CMD_PACKET_SIZE),
        CDC_FS_BINTERVAL, /* bInterval: */
        /*---------------------------------------------------------------------------*/

        /* Data class interface descriptor */
        0x09,                    /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
        0x04,                    /* bInterfaceNumber: Number of Interface */
        0x00,                    /* bAlternateSetting: Alternate setting */
        0x02,                    /* bNumEndpoints: Two endpoints used */
        0x0A,                    /* bInterfaceClass: CDC */
        0x00,                    /* bInterfaceSubClass: */
        0x00,                    /* bInterfaceProtocol: */
        0x00,                    /* iInterface: */

        /* Endpoint OUT Descriptor */
        0x07,                                /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,              /* bDescriptorType: Endpoint */
        CDC1_OUT_EP,                         /* bEndpointAddress */
        0x02,                                /* bmAttributes: Bulk */
        LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
        0x00, /* bInterval: ignore for Bulk transfer */

        /* Endpoint IN Descriptor */
        0x07,                                /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,              /* bDescriptorType: Endpoint */
        CDC1_IN_EP,                          /* bEndpointAddress */
        0x02,                                /* bmAttributes: Bulk */
        LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
        0x00,
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_Dual_CDC_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
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

/** @defgroup Dual_CDC_CORE_Private_Functions
  * @{
  */

/**
  * @brief  USBD_Dual_CDC_Init
  *         Initialize  the mass storage configuration
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status
  */
uint8_t USBD_Dual_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_CDC_HandleTypeDef *hcdc0;
  USBD_CDC_HandleTypeDef *hcdc1;

  hcdc0 = USBD_malloc(sizeof(USBD_CDC_HandleTypeDef));
  hcdc1 = USBD_malloc(sizeof(USBD_CDC_HandleTypeDef));

  if (hcdc0 == NULL || hcdc1 == NULL)
  {
    pdev->pClassDataCDC[0] = NULL;
    pdev->pClassDataCDC[1] = NULL;
    return (uint8_t)USBD_EMEM;
  }

  pdev->pClassDataCDC[0] = (void *)hcdc0;
  pdev->pClassDataCDC[1] = (void *)hcdc1;

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Open EP IN */
    (void)USBD_LL_OpenEP(pdev, CDC0_IN_EP, USBD_EP_TYPE_BULK, CDC_DATA_HS_IN_PACKET_SIZE);
    (void)USBD_LL_OpenEP(pdev, CDC1_IN_EP, USBD_EP_TYPE_BULK, CDC_DATA_HS_IN_PACKET_SIZE);

    pdev->ep_in[CDC0_IN_EP & 0xFU].is_used = 1U;
    pdev->ep_in[CDC1_IN_EP & 0xFU].is_used = 1U;

    /* Open EP OUT */
    (void)USBD_LL_OpenEP(pdev, CDC0_OUT_EP, USBD_EP_TYPE_BULK, CDC_DATA_HS_OUT_PACKET_SIZE);
    (void)USBD_LL_OpenEP(pdev, CDC1_OUT_EP, USBD_EP_TYPE_BULK, CDC_DATA_HS_OUT_PACKET_SIZE);

    pdev->ep_out[CDC0_OUT_EP & 0xFU].is_used = 1U;
    pdev->ep_out[CDC1_OUT_EP & 0xFU].is_used = 1U;

    /* Set bInterval for CDC CMD Endpoint */
    pdev->ep_in[CDC0_CMD_EP & 0xFU].bInterval = CDC_HS_BINTERVAL;
    pdev->ep_in[CDC1_CMD_EP & 0xFU].bInterval = CDC_HS_BINTERVAL;
  }
  else
  {
    /* Open EP IN */
    (void)USBD_LL_OpenEP(pdev, CDC0_IN_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_IN_PACKET_SIZE);
    (void)USBD_LL_OpenEP(pdev, CDC1_IN_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_IN_PACKET_SIZE);

    pdev->ep_in[CDC0_IN_EP & 0xFU].is_used = 1U;
    pdev->ep_in[CDC1_IN_EP & 0xFU].is_used = 1U;

    /* Open EP OUT */
    (void)USBD_LL_OpenEP(pdev, CDC0_OUT_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_OUT_PACKET_SIZE);
    (void)USBD_LL_OpenEP(pdev, CDC1_OUT_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_OUT_PACKET_SIZE);

    pdev->ep_out[CDC0_OUT_EP & 0xFU].is_used = 1U;
    pdev->ep_out[CDC1_OUT_EP & 0xFU].is_used = 1U;

    /* Set bInterval for CMD Endpoint */
    pdev->ep_in[CDC0_CMD_EP & 0xFU].bInterval = CDC_FS_BINTERVAL;
    pdev->ep_in[CDC1_CMD_EP & 0xFU].bInterval = CDC_FS_BINTERVAL;
  }

  /* Open Command IN EP */
  (void)USBD_LL_OpenEP(pdev, CDC0_CMD_EP, USBD_EP_TYPE_INTR, CDC_CMD_PACKET_SIZE);
  (void)USBD_LL_OpenEP(pdev, CDC1_CMD_EP, USBD_EP_TYPE_INTR, CDC_CMD_PACKET_SIZE);

  pdev->ep_in[CDC0_CMD_EP & 0xFU].is_used = 1U;
  pdev->ep_in[CDC1_CMD_EP & 0xFU].is_used = 1U;

  /* Init  physical Interface components */
  ((USBD_CDC_ItfTypeDef *)pdev->pUserDataCDC)->Init(0);
  ((USBD_CDC_ItfTypeDef *)pdev->pUserDataCDC)->Init(1);

  /* Init Xfer states */
  hcdc0->TxState = 0U;
  hcdc1->TxState = 0U;

  hcdc0->RxState = 0U;
  hcdc1->RxState = 0U;

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Prepare Out endpoint to receive next packet */
    (void)USBD_LL_PrepareReceive(pdev, CDC0_OUT_EP, hcdc0->RxBuffer, CDC_DATA_HS_OUT_PACKET_SIZE);
    (void)USBD_LL_PrepareReceive(pdev, CDC1_OUT_EP, hcdc1->RxBuffer, CDC_DATA_HS_OUT_PACKET_SIZE);
  }
  else
  {
    /* Prepare Out endpoint to receive next packet */
    (void)USBD_LL_PrepareReceive(pdev, CDC0_OUT_EP, hcdc0->RxBuffer, CDC_DATA_FS_OUT_PACKET_SIZE);
    (void)USBD_LL_PrepareReceive(pdev, CDC1_OUT_EP, hcdc1->RxBuffer, CDC_DATA_FS_OUT_PACKET_SIZE);
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_Dual_CDC_DeInit
  *         DeInitilaize  the mass storage configuration
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status
  */
uint8_t USBD_Dual_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  uint8_t ret = 0U;

  /* Close EP IN */
  (void)USBD_LL_CloseEP(pdev, CDC0_IN_EP);
  (void)USBD_LL_CloseEP(pdev, CDC1_IN_EP);
  pdev->ep_in[CDC0_IN_EP & 0xFU].is_used = 0U;
  pdev->ep_in[CDC1_IN_EP & 0xFU].is_used = 0U;

  /* Close EP OUT */
  (void)USBD_LL_CloseEP(pdev, CDC0_OUT_EP);
  (void)USBD_LL_CloseEP(pdev, CDC1_OUT_EP);
  pdev->ep_out[CDC0_OUT_EP & 0xFU].is_used = 0U;
  pdev->ep_out[CDC1_OUT_EP & 0xFU].is_used = 0U;

  /* Close Command IN EP */
  (void)USBD_LL_CloseEP(pdev, CDC0_CMD_EP);
  (void)USBD_LL_CloseEP(pdev, CDC1_CMD_EP);
  pdev->ep_in[CDC0_CMD_EP & 0xFU].is_used = 0U;
  pdev->ep_in[CDC1_CMD_EP & 0xFU].is_used = 0U;
  pdev->ep_in[CDC0_CMD_EP & 0xFU].bInterval = 0U;
  pdev->ep_in[CDC1_CMD_EP & 0xFU].bInterval = 0U;

  /* DeInit  physical Interface components */
  if (pdev->pClassDataCDC[0] != NULL)
  {
    ((USBD_CDC_ItfTypeDef *)pdev->pUserDataCDC)->DeInit(0);
    (void)USBD_free(pdev->pClassDataCDC[0]);
    pdev->pClassDataCDC[0] = NULL;
  }
  if (pdev->pClassDataCDC[1] != NULL)
  {
    ((USBD_CDC_ItfTypeDef *)pdev->pUserDataCDC)->DeInit(1);
    (void)USBD_free(pdev->pClassDataCDC[1]);
    pdev->pClassDataCDC[1] = NULL;
  }

  return ret;
}
/**
* @brief  USBD_Dual_CDC_Setup
*         Handle the MSC specific requests
* @param  pdev: device instance
* @param  req: USB request
* @retval status
*/
uint8_t USBD_Dual_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  uint8_t ifalt = 0U;
  uint16_t status_info = 0U;
  USBD_StatusTypeDef ret = USBD_OK;

  uint8_t cdc_index = 1;
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)pdev->pClassDataCDC[cdc_index];

  if (((req->bmRequest & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_INTERFACE && req->wIndex == 0x00) ||
      ((req->bmRequest & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_ENDPOINT && ((req->wIndex & 0x7F) == 0x02)))
  {
    cdc_index = 0;
    hcdc = (USBD_CDC_HandleTypeDef *)pdev->pClassDataCDC[cdc_index];
  }

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS:
    if (req->wLength != 0U)
    {
      if ((req->bmRequest & 0x80U) != 0U)
      {
        ((USBD_CDC_ItfTypeDef *)pdev->pUserDataCDC)->Control(cdc_index, req->bRequest, (uint8_t *)hcdc->data, req->wLength);

        (void)USBD_CtlSendData(pdev, (uint8_t *)hcdc->data, req->wLength);
      }
      else
      {
        hcdc->CmdOpCode = req->bRequest;
        hcdc->CmdLength = (uint8_t)req->wLength;

        (void)USBD_CtlPrepareRx(pdev, (uint8_t *)hcdc->data, req->wLength);
      }
    }
    else
    {
      ((USBD_CDC_ItfTypeDef *)pdev->pUserDataCDC)->Control(cdc_index, req->bRequest, (uint8_t *)req, 0U);
    }
    break;

  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_STATUS:
      if (pdev->dev_state == USBD_STATE_CONFIGURED)
      {
        (void)USBD_CtlSendData(pdev, (uint8_t *)&status_info, 2U);
      }
      else
      {
        USBD_CtlError(pdev, req);
        ret = USBD_FAIL;
      }
      break;

    case USB_REQ_GET_INTERFACE:
      if (pdev->dev_state == USBD_STATE_CONFIGURED)
      {
        (void)USBD_CtlSendData(pdev, &ifalt, 1U);
      }
      else
      {
        USBD_CtlError(pdev, req);
        ret = USBD_FAIL;
      }
      break;

    case USB_REQ_SET_INTERFACE:
      if (pdev->dev_state != USBD_STATE_CONFIGURED)
      {
        USBD_CtlError(pdev, req);
        ret = USBD_FAIL;
      }
      break;

    case USB_REQ_CLEAR_FEATURE:
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
    }
    break;

  default:
    USBD_CtlError(pdev, req);
    ret = USBD_FAIL;
    break;
  }

  return (uint8_t)ret;
}

/**
* @brief  USBD_Dual_CDC_DataIn
*         handle data IN Stage
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval status
*/
uint8_t USBD_Dual_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_CDC_HandleTypeDef *hcdc;
  PCD_HandleTypeDef *hpcd = pdev->pData;

  uint8_t cdc_index = 1;
  if (epnum == 0x01 || epnum == 0x02)
  {
    cdc_index = 0;
  }

  if (pdev->pClassDataCDC[cdc_index] == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hcdc = (USBD_CDC_HandleTypeDef *)pdev->pClassDataCDC[cdc_index];

  if ((pdev->ep_in[epnum].total_length > 0U) &&
      ((pdev->ep_in[epnum].total_length % hpcd->IN_ep[epnum].maxpacket) == 0U))
  {
    /* Update the packet total length */
    pdev->ep_in[epnum].total_length = 0U;

    /* Send ZLP */
    (void)USBD_LL_Transmit(pdev, epnum, NULL, 0U);
  }
  else
  {
    hcdc->TxState = 0U;
    ((USBD_CDC_ItfTypeDef *)pdev->pUserDataCDC)->TransmitCplt(cdc_index, hcdc->TxBuffer, &hcdc->TxLength, epnum);
  }

  return (uint8_t)USBD_OK;
}

/**
* @brief  USBD_Dual_CDC_DataOut
*         handle data OUT Stage
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval status
*/
uint8_t USBD_Dual_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  uint8_t cdc_index = 1;
  if (epnum == 0x01 || epnum == 0x02)
  {
    cdc_index = 0;
  }

  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)pdev->pClassDataCDC[cdc_index];

  if (pdev->pClassDataCDC[cdc_index] == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  /* Get the received data length */
  hcdc->RxLength = USBD_LL_GetRxDataSize(pdev, epnum);

  /* USB data will be immediately processed, this allow next USB traffic being
    NAKed till the end of the application Xfer */

  ((USBD_CDC_ItfTypeDef *)pdev->pUserDataCDC)->Receive(cdc_index, hcdc->RxBuffer, &hcdc->RxLength);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_Dual_CDC_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_Dual_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  USBD_CDC_HandleTypeDef *hcdc0 = (USBD_CDC_HandleTypeDef *)pdev->pClassDataCDC[0];
  USBD_CDC_HandleTypeDef *hcdc1 = (USBD_CDC_HandleTypeDef *)pdev->pClassDataCDC[1];

  if ((pdev->pUserDataCDC != NULL))
  {
    if ((hcdc0->CmdOpCode != 0xFFU))
    {
      ((USBD_CDC_ItfTypeDef *)pdev->pUserDataCDC)->Control(0, hcdc0->CmdOpCode, (uint8_t *)hcdc0->data, (uint16_t)hcdc0->CmdLength);
      hcdc0->CmdOpCode = 0xFFU;
    }

    if ((hcdc1->CmdOpCode != 0xFFU))
    {
      ((USBD_CDC_ItfTypeDef *)pdev->pUserDataCDC)->Control(1, hcdc1->CmdOpCode, (uint8_t *)hcdc1->data, (uint16_t)hcdc1->CmdLength);
      hcdc1->CmdOpCode = 0xFFU;
    }
  }

  return (uint8_t)USBD_OK;
}

/**
* @brief  USBD_Dual_CDC_GetFSCfgDesc
*         return configuration descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_Dual_CDC_GetFSCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_Dual_CDC_CfgFSDesc);

  return USBD_Dual_CDC_CfgFSDesc;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_Dual_CDC_GetDeviceQualifierDescriptor(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_Dual_CDC_DeviceQualifierDesc);

  return USBD_Dual_CDC_DeviceQualifierDesc;
}

/**
* @brief  USBD_CDC_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CD  Interface callback
  * @retval status
  */
uint8_t USBD_CDC_RegisterInterface(USBD_HandleTypeDef *pdev, USBD_CDC_ItfTypeDef *fops)
{
  if (fops == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  pdev->pUserDataCDC = fops;

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_SetTxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Tx Buffer
  * @retval status
  */
uint8_t USBD_CDC_SetTxBuffer(uint8_t cdc_index, USBD_HandleTypeDef *pdev, uint8_t *pbuff, uint32_t length)
{
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)pdev->pClassDataCDC[cdc_index];

  hcdc->TxBuffer = pbuff;
  hcdc->TxLength = length;

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_SetRxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Rx Buffer
  * @retval status
  */
uint8_t USBD_CDC_SetRxBuffer(uint8_t cdc_index, USBD_HandleTypeDef *pdev, uint8_t *pbuff)
{
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)pdev->pClassDataCDC[cdc_index];

  hcdc->RxBuffer = pbuff;

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_TransmitPacket
  *         Transmit packet on IN endpoint
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_CDC_TransmitPacket(uint8_t cdc_index, USBD_HandleTypeDef *pdev)
{
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)pdev->pClassDataCDC[cdc_index];
  USBD_StatusTypeDef ret = USBD_BUSY;

  if (pdev->pClassDataCDC[cdc_index] == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  if (hcdc->TxState == 0U)
  {
    /* Tx Transfer in progress */
    hcdc->TxState = 1U;

    if (cdc_index)
    {
      /* Update the packet total length */
      pdev->ep_in[CDC1_IN_EP & 0xFU].total_length = hcdc->TxLength;

      /* Transmit next packet */
      (void)USBD_LL_Transmit(pdev, CDC1_IN_EP, hcdc->TxBuffer, hcdc->TxLength);
    }
    else
    {
      /* Update the packet total length */
      pdev->ep_in[CDC0_IN_EP & 0xFU].total_length = hcdc->TxLength;

      /* Transmit next packet */
      (void)USBD_LL_Transmit(pdev, CDC0_IN_EP, hcdc->TxBuffer, hcdc->TxLength);
    }

    ret = USBD_OK;
  }

  return (uint8_t)ret;
}

/**
  * @brief  USBD_CDC_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_CDC_ReceivePacket(uint8_t cdc_index, USBD_HandleTypeDef *pdev)
{
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)pdev->pClassDataCDC[cdc_index];

  if (pdev->pClassDataCDC[cdc_index] == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Prepare Out endpoint to receive next packet */
    if (cdc_index)
    {
      (void)USBD_LL_PrepareReceive(pdev, CDC1_OUT_EP, hcdc->RxBuffer, CDC_DATA_HS_OUT_PACKET_SIZE);
    }
    else
    {
      (void)USBD_LL_PrepareReceive(pdev, CDC0_OUT_EP, hcdc->RxBuffer, CDC_DATA_HS_OUT_PACKET_SIZE);
    }
  }
  else
  {
    /* Prepare Out endpoint to receive next packet */
    if (cdc_index)
    {
      (void)USBD_LL_PrepareReceive(pdev, CDC1_OUT_EP, hcdc->RxBuffer, CDC_DATA_FS_OUT_PACKET_SIZE);
    }
    else
    {
      (void)USBD_LL_PrepareReceive(pdev, CDC0_OUT_EP, hcdc->RxBuffer, CDC_DATA_FS_OUT_PACKET_SIZE);
    }
  }

  return (uint8_t)USBD_OK;
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
