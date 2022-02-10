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

typedef struct __canconfig
{
	uint8_t			RecvBuff[64];
}CanConfig;

extern DEV_HANDLE  CAN_0;
extern DEV_HANDLE  CAN_1;
extern DEV_HANDLE  CAN_2;
extern DEV_HANDLE  CAN_3;
extern DEV_HANDLE  CAN_4;
extern DEV_HANDLE  CAN_5;
extern DEV_HANDLE  CAN_6;
extern DEV_HANDLE  CAN_7;
extern DEV_HANDLE  CAN_8;

bool Can_Init(DEV_HANDLE Can, CanConfig* Config);
bool Can_Send(DEV_HANDLE Can, uint8_t* Data, uint16_t DataLen);
void Can_SendComplete(DEV_HANDLE Can);
void Can_Recv(DEV_HANDLE Can, uint8_t* Data, uint32_t Len);

#ifdef __cplusplus
}
#endif

