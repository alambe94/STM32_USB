#include <stdint.h>

#include "main.h"
#include "i2c.h"


void AUDIO_IO_Init(void)
{
  /** gpio, i2c, i2s configured in cube @see gpio.c, i2c.c, i2s.c*/

  /* Power Down the codec */
  HAL_GPIO_WritePin(Audio_RST_GPIO_Port, Audio_RST_Pin, GPIO_PIN_RESET);

  /* Wait for a delay to insure registers erasing */
  HAL_Delay(5);

  /* Power on the codec */
  HAL_GPIO_WritePin(Audio_RST_GPIO_Port, Audio_RST_Pin, GPIO_PIN_SET);

  /* Wait for a delay to insure registers erasing */
  HAL_Delay(5);
}
void AUDIO_IO_DeInit(void)
{

}
void AUDIO_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
	HAL_I2C_Mem_Write(&hi2c1, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, 100);
}
uint8_t AUDIO_IO_Read(uint8_t Addr, uint8_t Reg)
{
	uint8_t value = 0;
	HAL_I2C_Mem_Read(&hi2c1, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
	return value;
}


