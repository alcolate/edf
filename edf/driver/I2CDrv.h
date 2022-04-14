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

typedef struct __I2Cconfig
{
	uint8_t			Reserve[1];
}I2CConfig;

extern DEV_HANDLE  I2C_0;
extern DEV_HANDLE  I2C_1;
extern DEV_HANDLE  I2C_2;
extern DEV_HANDLE  I2C_3;
extern DEV_HANDLE  I2C_4;
extern DEV_HANDLE  I2C_5;
extern DEV_HANDLE  I2C_6;
extern DEV_HANDLE  I2C_7;
extern DEV_HANDLE  I2C_8;

void I2C_Init(DEV_HANDLE  I2C);
uint8_t I2C_Read7(DEV_HANDLE  I2C, uint8_t Device, uint8_t Addr, uint8_t *Buf, uint8_t Count);
uint8_t I2C_Read16(DEV_HANDLE I2C, uint8_t Device, uint16_t Addr, uint8_t *Buf, uint8_t Count);
uint8_t I2C_Write7(DEV_HANDLE  I2C, uint8_t Device, uint8_t Addr, uint8_t *Buf, uint8_t Count);
uint8_t I2C_Write16(DEV_HANDLE  I2C, uint8_t Device, uint16_t Addr, uint8_t *Buf, uint8_t Count);

void I2C_SendComplete(DEV_HANDLE I2C);
void I2C_Recv(DEV_HANDLE I2C, uint8_t* Data, uint32_t Len);

#ifdef __cplusplus
}
#endif

