/*
 * flash_if.h
 *
 *  Created on: Feb 12, 2017
 *      Author: lubos koudelka
 */

#ifndef FLASH_IF_H_
#define FLASH_IF_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
typedef  void (*pFunction)(void);

/* Exported constants --------------------------------------------------------*/
/* Define the flash memory start address */
#define USER_FLASH_STARTADDRESS    ((uint32_t)0x08000000)

/* Define the address from where user application will be loaded.
Note: the 1st and the second sectors 0x08000000-0x0800BFFF are reserved
for the Firmware upgrade code */
#define APPLICATION_ADDRESS        (uint32_t)0x08008000

/* Last Page Adress */
#define USER_FLASH_LAST_PAGE_ADDRESS  0x08060000 - 4

/* Define the user application size */
#define USER_FLASH_SIZE   (USER_FLASH_LAST_PAGE_ADDRESS - APPLICATION_ADDRESS + 1)

/* Base address of the Flash sectors Bank 1 */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */

/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void FLASH_If_FlashUnlock(void);
FlagStatus FLASH_If_ReadOutProtectionStatus(void);
uint32_t FLASH_If_EraseSectors(uint32_t Address);
uint32_t FLASH_If_Write(uint32_t Address, uint32_t Data);

#endif /* FLASH_IF_H_ */
