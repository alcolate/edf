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

#define GPIO_HIGH  1
#define GPIO_LOW   0

typedef struct __GPIOconfig
{
	int dump;
}GPIOConfig;

extern DEV_HANDLE  GPIO_0;
extern DEV_HANDLE  GPIO_1;
extern DEV_HANDLE  GPIO_2;
extern DEV_HANDLE  GPIO_3;
extern DEV_HANDLE  GPIO_4;
extern DEV_HANDLE  GPIO_5;
extern DEV_HANDLE  GPIO_6;
extern DEV_HANDLE  GPIO_7;
extern DEV_HANDLE  GPIO_8;
extern DEV_HANDLE  GPIO_9;
extern DEV_HANDLE  GPIO_10;
extern DEV_HANDLE  GPIO_11;
extern DEV_HANDLE  GPIO_12;
extern DEV_HANDLE  GPIO_13;
extern DEV_HANDLE  GPIO_14;
extern DEV_HANDLE  GPIO_15;
extern DEV_HANDLE  GPIO_16;
extern DEV_HANDLE  GPIO_17;
extern DEV_HANDLE  GPIO_18;
extern DEV_HANDLE  GPIO_19;
extern DEV_HANDLE  GPIO_20;
extern DEV_HANDLE  GPIO_21;
extern DEV_HANDLE  GPIO_22;
extern DEV_HANDLE  GPIO_23;
extern DEV_HANDLE  GPIO_24;
extern DEV_HANDLE  GPIO_25;
extern DEV_HANDLE  GPIO_26;
extern DEV_HANDLE  GPIO_27;
extern DEV_HANDLE  GPIO_28;
extern DEV_HANDLE  GPIO_29;
extern DEV_HANDLE  GPIO_30;
extern DEV_HANDLE  GPIO_31;
extern DEV_HANDLE  GPIO_32;
extern DEV_HANDLE  GPIO_33;
extern DEV_HANDLE  GPIO_34;
extern DEV_HANDLE  GPIO_35;
extern DEV_HANDLE  GPIO_36;
extern DEV_HANDLE  GPIO_37;
extern DEV_HANDLE  GPIO_38;
extern DEV_HANDLE  GPIO_39;
extern DEV_HANDLE  GPIO_40;
extern DEV_HANDLE  GPIO_41;
extern DEV_HANDLE  GPIO_42;
extern DEV_HANDLE  GPIO_43;
extern DEV_HANDLE  GPIO_44;
extern DEV_HANDLE  GPIO_45;
extern DEV_HANDLE  GPIO_46;
extern DEV_HANDLE  GPIO_47;
extern DEV_HANDLE  GPIO_48;
extern DEV_HANDLE  GPIO_49;

bool GPIO_Init(DEV_HANDLE Gpio, GPIOConfig* Config);
void GPIO_Set(DEV_HANDLE Gpio, uint32_t Value);
void GPIO_Toggle(DEV_HANDLE Gpio);
void GPIO_SetInputMode(DEV_HANDLE Gpio);
void GPIO_SetOutputMode(DEV_HANDLE Gpio);
uint32_t GPIO_Get(DEV_HANDLE Gpio);

void GPIO_Complete(DEV_HANDLE Gpio);

#ifdef __cplusplus
}
#endif

