#include "pcm_buffer_pool.h"

#define PCM_POOL_SIZE 10

uint16_t PCM_Buffer_Pool[PCM_POOL_SIZE][AUDIO_IN_PCM_SAMPLES_IN_MS];
uint8_t PCM_Read_Index;
uint8_t PCM_Write_Index;
uint8_t PCM_Full_Flag;

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
uint16_t *PCM_Pool_Next_Read()
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

/** return pointer to next empty buffer */
uint16_t *PCM_Pool_Next_Write()
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
