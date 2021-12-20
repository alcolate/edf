/*****************************************************************************
Copyright 2021 The Edf Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Contact information:
<9183399@qq.com>
*****************************************************************************/
#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "Driver.h"

enum ADC_MODE
{
	INTERRUPT,
	DMA,
};
typedef struct __ADCconfig
{
	uint8_t			Reserve[1];
}ADCConfig;

extern DEV_HANDLE  ADC_0;
extern DEV_HANDLE  ADC_1;
extern DEV_HANDLE  ADC_2;
extern DEV_HANDLE  ADC_3;
extern DEV_HANDLE  ADC_4;
extern DEV_HANDLE  ADC_5;
extern DEV_HANDLE  ADC_6;
extern DEV_HANDLE  ADC_7;
extern DEV_HANDLE  ADC_8;


bool ADC_Init(DEV_HANDLE ADC, ADCConfig* Config);
bool ADC_Start(DEV_HANDLE ADC);
bool ADC_Start_DMA(DEV_HANDLE ADC, uint16_t *Data, uint32_t DataLen);
bool ADC_Stop(DEV_HANDLE ADC);
uint32_t ADC_GetChannels(DEV_HANDLE ADC);
ADC_MODE ADC_GetMode(DEV_HANDLE ADC);

void ADC_Complete(DEV_HANDLE ADC, uint16_t Data);


#ifdef __cplusplus
}
#endif

