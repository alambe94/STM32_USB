#ifndef INC_PCM_BUFFER_POOL_H_
#define INC_PCM_BUFFER_POOL_H_

#include <stdint.h>
#include <stddef.h>

#define AUDIO_IN_SAMPLING_FREQ         48000
#define AUDIO_IN_BIT_RESOLUTION        16
#define AUDIO_IN_PDM_DECIMATION_FACTOR 64
#define AUDIO_IN_CHANNELS              1
#define AUDIO_IN_PCM_SAMPLES_IN_MS     (AUDIO_IN_SAMPLING_FREQ/1000)
#define AUDIO_IN_PDM_FREQ              (AUDIO_IN_SAMPLING_FREQ*AUDIO_IN_PDM_DECIMATION_FACTOR*AUDIO_IN_CHANNELS)
#define AUDIO_IN_PCM_SAMPLES_RB_SIZE   32

void Check_PDM();
uint8_t PCM_Pool_Is_Full();
uint8_t PCM_Pool_Is_Empty();
uint8_t PCM_Pool_Get_Count();
uint16_t *PCM_Pool_Next_Filled();
uint16_t *PCM_Pool_Next_Empty();
uint16_t *PDM_Get_Buffer();

#endif /* INC_PCM_BUFFER_POOL_H_ */
