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
#include <stdlib.h>
#include <string.h>
#include <memory>
#include "CanDrv.h"

DEV_HANDLE  UART_0 = (DEV_HANDLE)1;
DEV_HANDLE  UART_1 = (DEV_HANDLE)2;
DEV_HANDLE  UART_2 = (DEV_HANDLE)3;



bool Can_Init(DEV_HANDLE Can, CanConfig* Config)
{

	return true;
}

bool Can_Send(DEV_HANDLE Can, uint8_t* Data, uint16_t DataLen)
{
	return true;
}
