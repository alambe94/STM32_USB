/*
 * w25qxx.h
 *
 *  Created on: 23-Mar-2020
 *      Author: gitz
 */

#ifndef W25QXX_H_
#define W25QXX_H_

#include "main.h"

#define W25QXX_TOTAL_SIZE     (8*1024*1024)
#define W25QXX_ID             0xEF4017
#define W25QXX_PAGE_SIZE      256
#define	W25QXX_PAGE_COUNT     (W25QXX_TOTAL_SIZE/W25QXX_PAGE_SIZE)
#define	W25QXX_SECTOR_SIZE    4096
#define	W25QXX_SECTOR_COUNT   (W25QXX_TOTAL_SIZE/W25QXX_SECTOR_SIZE)

#define W25QXX_BLOCK_COUNT    128
#define W25QXX_ADDRESS_WIDTH  24

extern SPI_HandleTypeDef hspi1;
#define W25QXX_SPI       hspi1

void W25QXX_Init(void);
uint8_t W25QXX_Read_Status(void);
uint32_t W25QXX_Read_ID(void);
void W25QXX_Read_Unique_ID(uint8_t *buff);
void W25QXX_Write_Enable(void);
void W25QXX_Write_Disable(void);
void W25QXX_Wait_Busy(void);
void W25QXX_Erase_Chip(void);
void W25QXX_Read(uint32_t address, uint8_t *buffer, uint16_t count);
void W25QXX_Write_Page(uint32_t address, uint8_t *buffer, uint16_t count);
void W25QXX_Erase_Sector(uint32_t sector_number);
void W25QXX_Write(uint32_t address, uint8_t *buffer, uint32_t count);

#endif /* W25QXX_H_ */
