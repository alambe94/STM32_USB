#ifndef INC_PCM_BUFFER_POOL_H_
#define INC_PCM_BUFFER_POOL_H_

#include <stdint.h>
#include <stddef.h>

#define AUDIO_OUT_SAMPLING_FREQ        48000
#define AUDIO_OUT_BIT_RESOLUTION       16
#define AUDIO_OUT_CHANNELS             2
#define AUDIO_OUT_PCM_SAMPLES_IN_MS    (AUDIO_OUT_CHANNELS*AUDIO_OUT_SAMPLING_FREQ/1000)

void App_Loop();
uint8_t PCM_Pool_Is_Full();
uint8_t PCM_Pool_Is_Empty();
uint8_t PCM_Pool_Get_Count();
uint16_t *PCM_Pool_Next_Filled();
uint16_t *PCM_Pool_Next_Empty();

#endif /* INC_PCM_BUFFER_POOL_H_ */
