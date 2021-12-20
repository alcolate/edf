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


typedef struct __pwmconfig
{
	int dump;
}PWMConfig;

extern DEV_HANDLE  PWM_0;
extern DEV_HANDLE  PWM_1;
extern DEV_HANDLE  PWM_2;
extern DEV_HANDLE  PWM_3;
extern DEV_HANDLE  PWM_4;
extern DEV_HANDLE  PWM_5;
extern DEV_HANDLE  PWM_6;
extern DEV_HANDLE  PWM_7;
extern DEV_HANDLE  PWM_8;

#define PWM_CHANNEL_0	0
#define PWM_CHANNEL_1	1
#define PWM_CHANNEL_2	2
#define PWM_CHANNEL_3	3

bool PWM_Init(DEV_HANDLE PWM, PWMConfig* Config);
bool PWM_Start(DEV_HANDLE PWM, uint32_t Channel);
bool PWM_Stop(DEV_HANDLE PWM, uint32_t Channel);
bool PWM_SetPeriod(DEV_HANDLE PWM, uint32_t Channel, uint16_t Period, uint16_t Duty);
void PWM_SendComplete(DEV_HANDLE PWM);
void PWM_Complete(DEV_HANDLE PWM);

#ifdef __cplusplus
}
#endif

