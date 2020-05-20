/*
 * w25qxx.c
 *
 *  Created on: 23-Mar-2020
 *      Author: gitz
 */

#include "w25qxx.h"

/**
 * spi configured in cube @see spi.c
 * gpio configured in cube @see gpio.c
 * cs pin defined in @see main.h
 */
void W25QXX_Init(void)
    {
    W25QXX_Read_Status();
    }

uint8_t W25QXX_Read_Status(void)
    {
    uint8_t cmd = 0x05;
    uint8_t result;

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&W25QXX_SPI, &cmd, 1, 100);

    HAL_SPI_Receive(&W25QXX_SPI, &result, 1, 100);

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);

    return result;
    }

uint32_t W25QXX_Read_ID(void)
    {
    uint8_t cmd = 0x9F;
    uint8_t ret;
    uint32_t result;

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&W25QXX_SPI, &cmd, 1, 100);
    cmd = 0x00;

    HAL_SPI_Receive(&W25QXX_SPI, &ret, 1, 100);
    result = ret << 16;

    HAL_SPI_Receive(&W25QXX_SPI, &ret, 1, 100);
    result |= ret << 8;

    HAL_SPI_Receive(&W25QXX_SPI, &ret, 1, 100);
    result |= ret;

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);

    return result;
    }


void W25QXX_Read_Unique_ID(uint8_t *buff)
    {
    uint8_t cmd = 0x9F;
    uint8_t ret;

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&W25QXX_SPI, &cmd, 1, 100);
    cmd = 0x00;
    HAL_SPI_Transmit(&W25QXX_SPI, &cmd, 1, 100);
    HAL_SPI_Transmit(&W25QXX_SPI, &cmd, 1, 100);
    HAL_SPI_Transmit(&W25QXX_SPI, &cmd, 1, 100);
    HAL_SPI_Transmit(&W25QXX_SPI, &cmd, 1, 100);

    for (uint8_t i = 0; i < 8; i++)
	{
	HAL_SPI_Receive(&W25QXX_SPI, &ret, 1, 100);
	buff[i] = ret;
	}
    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);
    }


void W25QXX_Write_Enable(void)
    {
    uint8_t cmd = 0x06;

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&W25QXX_SPI, &cmd, 1, 100);

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);
    }

void W25QXX_Write_Disable(void)
    {
    uint8_t cmd = 0x04;

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&W25QXX_SPI, &cmd, 1, 100);

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);
    }

void W25QXX_Wait_Busy(void)
    {
    while ((W25QXX_Read_Status() & 0x01) == 0x01);
    }

void W25QXX_Erase_Chip(void)
    {
    uint8_t cmd = 0xC7;

    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&W25QXX_SPI, &cmd, 1, 100);

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);

    W25QXX_Wait_Busy();
    }

void W25QXX_Read(uint32_t address, uint8_t *buffer, uint16_t count)
    {
    uint8_t cmd = 0x03;
    uint8_t temp;

    if (!count)
	{
	return;
	}

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&W25QXX_SPI, &cmd, 1, 100);

    temp = address>>16;
    HAL_SPI_Transmit(&W25QXX_SPI, &temp, 1, 100);

    temp = address>>8;
    HAL_SPI_Transmit(&W25QXX_SPI, &temp, 1, 100);

    temp = address;
    HAL_SPI_Transmit(&W25QXX_SPI, &temp, 1, 100);

    HAL_SPI_Receive(&W25QXX_SPI, buffer, count, 5000);

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);
    }

void W25QXX_Erase_Sector(uint32_t sector_number)
    {
    uint8_t cmd = 0x20;
    uint8_t temp;
    uint32_t address = sector_number*W25QXX_SECTOR_SIZE;

    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&W25QXX_SPI, &cmd, 1, 100);

    temp = address>>16;
    HAL_SPI_Transmit(&W25QXX_SPI, &temp, 1, 100);

    temp = address>>8;
    HAL_SPI_Transmit(&W25QXX_SPI, &temp, 1, 100);

    temp = address;
    HAL_SPI_Transmit(&W25QXX_SPI, &temp, 1, 100);

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);

    W25QXX_Wait_Busy();
    }

void W25QXX_Write_Page(uint32_t address, uint8_t *buffer, uint16_t count)
    {
    uint8_t cmd = 0x02;
    uint8_t temp;

    if (!count || count > W25QXX_PAGE_SIZE)
	{
	return;
	}

    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&W25QXX_SPI, &cmd, 1, 100);

    temp = address>>16;
    HAL_SPI_Transmit(&W25QXX_SPI, &temp, 1, 100);

    temp = address>>8;
    HAL_SPI_Transmit(&W25QXX_SPI, &temp, 1, 100);

    temp = address;
    HAL_SPI_Transmit(&W25QXX_SPI, &temp, 1, 100);

    HAL_SPI_Transmit(&W25QXX_SPI, buffer, count, 5000);

    HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);

    W25QXX_Wait_Busy();
    }

void W25QXX_Write(uint32_t address, uint8_t *buffer, uint32_t count)
    {

    if (address % W25QXX_SECTOR_SIZE == 0)
	{
	W25QXX_Erase_Sector(address / W25QXX_SECTOR_SIZE);
	}

    uint16_t bytes_to_write = W25QXX_PAGE_SIZE - address % W25QXX_PAGE_SIZE;

    if (count < bytes_to_write)
	{
	bytes_to_write = count;
	}

    W25QXX_Write_Page(address, buffer, bytes_to_write);

    address += bytes_to_write;
    buffer += bytes_to_write;
    count -= bytes_to_write;

    uint16_t pages = count / W25QXX_PAGE_SIZE;

    while (pages--)
	{
	W25QXX_Write_Page(address, buffer, W25QXX_PAGE_SIZE);
	address += W25QXX_PAGE_SIZE;
	buffer += W25QXX_PAGE_SIZE;
	count -= W25QXX_PAGE_SIZE;
	}

    W25QXX_Write_Page(address, buffer, count);
    }
