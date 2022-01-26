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

typedef void* CAN_HANDLE;

typedef struct __canconfig
{
	uint8_t			RecvBuff[64];
}CanConfig;

extern CAN_HANDLE  CAN_0;
extern CAN_HANDLE  CAN_1;
extern CAN_HANDLE  CAN_2;
extern CAN_HANDLE  CAN_3;
extern CAN_HANDLE  CAN_4;
extern CAN_HANDLE  CAN_5;
extern CAN_HANDLE  CAN_6;
extern CAN_HANDLE  CAN_7;
extern CAN_HANDLE  CAN_8;

bool Can_Init(CAN_HANDLE Can, CanConfig* Config);
bool Can_Send(CAN_HANDLE Can, uint8_t* Data, uint16_t DataLen);
void Can_SendComplete(CAN_HANDLE Can);
void Can_Recv(CAN_HANDLE Can, uint8_t* Data, uint32_t Len);

#ifdef __cplusplus
}
#endif

