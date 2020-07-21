#include <string.h>

#include "pcm_buffer_pool.h"
#include "cs43l22.h"
#include "i2s.h"

#define PCM_POOL_SIZE 100

static uint16_t PCM_Buffer_Pool[PCM_POOL_SIZE][AUDIO_OUT_PCM_SAMPLES_IN_MS];

static uint8_t PCM_Read_Index;
static uint8_t PCM_Write_Index;
static uint8_t PCM_Full_Flag;

uint8_t CS43L22_CMPLT_Flag = 0;

uint8_t Ping_Flag = 0;
uint8_t Pong_Flag = 0;
uint16_t CS43L22_Buffer[AUDIO_OUT_PCM_SAMPLES_IN_MS];

extern int16_t Sine_Wave[];

/** return 1 if entire pool is full*/
uint8_t PCM_Pool_Is_Full()
{
	return PCM_Full_Flag;
}

/** return 1 if entire pool is empty*/
uint8_t PCM_Pool_Is_Empty()
{
	return (PCM_Pool_Get_Count() == 0);
}

/** return number of filled buffers */
uint8_t PCM_Pool_Get_Count()
{
	if (PCM_Full_Flag)
		return PCM_POOL_SIZE;
	if (PCM_Write_Index >= PCM_Read_Index)
		return (PCM_Write_Index - PCM_Read_Index);
	return (PCM_POOL_SIZE - (PCM_Read_Index - PCM_Write_Index));
}

/** return pointer to next filled buffer */
uint16_t *PCM_Pool_Next_Filled()
{
	if(PCM_Pool_Is_Empty())
	{
		return NULL;
	}

	uint16_t *temp = PCM_Buffer_Pool[PCM_Read_Index++];
	PCM_Full_Flag = 0;
	if (PCM_Read_Index == PCM_POOL_SIZE)
		PCM_Read_Index = 0;

	return temp;
}

/** return pointer to next filled buffer */
uint16_t *PCM_Pool_Next_Peek()
{
	if(PCM_Pool_Is_Empty())
	{
		return NULL;
	}

	uint16_t *temp = PCM_Buffer_Pool[PCM_Read_Index];

	return temp;
}

/** return pointer to next empty buffer */
uint16_t *PCM_Pool_Next_Empty()
{
	if(PCM_Pool_Is_Full())
	{
		//return NULL;
	}

	uint16_t *temp = PCM_Buffer_Pool[PCM_Write_Index++];
	PCM_Full_Flag = 0;
	if (PCM_Write_Index == PCM_POOL_SIZE)
		PCM_Write_Index = 0;
	if (PCM_Write_Index == PCM_Read_Index)
		PCM_Full_Flag = 1;

	return temp;
}

void App_Loop()
{
	if(PCM_Pool_Get_Count())
	{
		uint16_t *data = PCM_Pool_Next_Peek();
		if(Ping_Flag)
		{
			Ping_Flag = 0;
			memcpy(CS43L22_Buffer, data, AUDIO_OUT_PCM_SAMPLES_IN_MS/2);
		}
		if(Pong_Flag)
		{
			Pong_Flag = 0;
			memcpy(CS43L22_Buffer + AUDIO_OUT_PCM_SAMPLES_IN_MS/2,
				   data + AUDIO_OUT_PCM_SAMPLES_IN_MS/2,
				   AUDIO_OUT_PCM_SAMPLES_IN_MS/2);

			PCM_Pool_Next_Filled();
		}
	}
}
