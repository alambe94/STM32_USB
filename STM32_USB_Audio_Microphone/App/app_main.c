#include <stdint.h>
#include <stddef.h>
#include <math.h>

#include "app_main.h"
#include "pdm2pcm.h"
#include "i2s.h"

uint16_t PDM_Buffer[AUDIO_IN_PDM_BUFFER_SIZE];

uint16_t PCM_Buffer[AUDIO_IN_PCM_BUFFER_SIZE];

int16_t Sine_Wave[48];

volatile uint32_t PCM_Read_Index_USB;
volatile uint32_t PCM_Read_Index_SPKR;
volatile uint32_t PCM_Write_Index;
volatile uint32_t PCM_Full_Flag_USB;
volatile uint32_t PCM_Full_Flag_SPKR;

#define PCM_SPKR_SIZE (AUDIO_IN_PCM_SAMPLES_IN_MS * 2) // sterio speaker
uint16_t SPKR_Samples[PCM_SPKR_SIZE];

uint32_t PCM_Get_Count_USB(void)
{
    if (PCM_Full_Flag_USB)
        return AUDIO_IN_PCM_BUFFER_SIZE;
    if (PCM_Write_Index > PCM_Read_Index_USB)
        return PCM_Write_Index - PCM_Read_Index_USB;
    return AUDIO_IN_PCM_BUFFER_SIZE - (PCM_Write_Index - PCM_Read_Index_USB);
}

uint32_t PCM_Get_Count_SPKR(void)
{
    if (PCM_Full_Flag_SPKR)
        return AUDIO_IN_PCM_BUFFER_SIZE;
    if (PCM_Write_Index > PCM_Read_Index_SPKR)
        return PCM_Write_Index - PCM_Read_Index_SPKR;
    return AUDIO_IN_PCM_BUFFER_SIZE - (PCM_Write_Index - PCM_Read_Index_SPKR);
}

void App_Main(void)
{
    for (uint16_t i = 0; i < 48; i++)
    {
        Sine_Wave[i] = 0xFFFF * sinf(i * 2 * 3.1416f / 48.0) / 2;
    }

    /** start streaming to speaker */
    HAL_I2S_Transmit_DMA(&hi2s3, SPKR_Samples, AUDIO_IN_PCM_SAMPLES_IN_MS * 2);

    while (1)
    {
    }
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s == &hi2s3)
    {
        if (PCM_Get_Count_SPKR() > (AUDIO_IN_PCM_BUFFER_SIZE - 36))
        {
            PCM_Read_Index_SPKR++;
        }

        if (PCM_Get_Count_SPKR() < 36)
        {
            PCM_Read_Index_SPKR--;
        }

        if (PCM_Get_Count_SPKR() > AUDIO_IN_PCM_SAMPLES_IN_MS / 2)
        {
            for (uint32_t i = 0; i < AUDIO_IN_PCM_SAMPLES_IN_MS / 2;)
            {
                SPKR_Samples[PCM_SPKR_SIZE/2 + i++] = PCM_Buffer[PCM_Read_Index_SPKR];
                SPKR_Samples[PCM_SPKR_SIZE/2 + i++] = PCM_Buffer[PCM_Read_Index_SPKR++];
                if (PCM_Read_Index_SPKR == AUDIO_IN_PCM_BUFFER_SIZE)
                {
                    PCM_Read_Index_SPKR = 0;
                }
            }
        }
    }
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s == &hi2s3)
    {
        if (PCM_Get_Count_SPKR() > (AUDIO_IN_PCM_SAMPLES_IN_MS / 2 - 36))
        {
            PCM_Read_Index_SPKR++;
        }

        if (PCM_Get_Count_SPKR() < 36)
        {
            PCM_Read_Index_SPKR--;
        }

        if (PCM_Get_Count_SPKR() > AUDIO_IN_PCM_SAMPLES_IN_MS)
        {
            for (uint32_t i = 0; i < AUDIO_IN_PCM_SAMPLES_IN_MS; i++)
            {
                SPKR_Samples[i] = PCM_Buffer[PCM_Read_Index_SPKR++];
                if (PCM_Read_Index_SPKR == AUDIO_IN_PCM_BUFFER_SIZE)
                {
                    PCM_Read_Index_SPKR = 0;
                }
            }
        }
    }
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s == &hi2s2)
    {
        PDM_Filter(PDM_Buffer, &PCM_Buffer[PCM_Write_Index], &PDM1_filter_handler);
        PCM_Write_Index += AUDIO_IN_PCM_SAMPLES_IN_MS/2;
        if (PCM_Write_Index >= AUDIO_IN_PCM_BUFFER_SIZE)
        {
            PCM_Write_Index = 0;
        }
        if (PCM_Write_Index == PCM_Read_Index_USB)
        {
            PCM_Full_Flag_USB = 1;
        }
        if (PCM_Write_Index == PCM_Read_Index_SPKR)
        {
            PCM_Full_Flag_SPKR = 1;
        }
    }
}
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s == &hi2s2)
    {
        PDM_Filter(PDM_Buffer, &PCM_Buffer[PCM_Write_Index], &PDM1_filter_handler);
        PCM_Write_Index += AUDIO_IN_PCM_SAMPLES_IN_MS/2;
        if (PCM_Write_Index >= AUDIO_IN_PCM_BUFFER_SIZE)
        {
            PCM_Write_Index = 0;
        }
        if (PCM_Write_Index == PCM_Read_Index_USB)
        {
            PCM_Full_Flag_USB = 1;
        }
        if (PCM_Write_Index == PCM_Read_Index_SPKR)
        {
            PCM_Full_Flag_SPKR = 1;
        }
    }
}
