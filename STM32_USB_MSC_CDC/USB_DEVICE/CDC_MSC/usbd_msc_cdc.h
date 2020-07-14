/**
  ******************************************************************************
  * @file    usbd_msc_cdc.h
  * @author  MCD Application Team
  * @brief   Header for the usbd_msc_cdc.c file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_MSC_CDC_H
#define __USBD_MSC_CDC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

#define USB_MSC_CDC_CONFIG_DESC_SIZ      98U

/* Structure for MSC_CDC process */
extern USBD_ClassTypeDef  USBD_MSC_CDC;
/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* __USBD_MSC_CDC_H */
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
