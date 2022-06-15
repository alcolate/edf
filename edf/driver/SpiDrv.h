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

typedef struct __Spiconfig
{
	uint8_t			Reserve[1];
}SpiConfig;

extern DEV_HANDLE  Spi_0;
extern DEV_HANDLE  Spi_1;
extern DEV_HANDLE  Spi_2;
extern DEV_HANDLE  Spi_3;
extern DEV_HANDLE  Spi_4;
extern DEV_HANDLE  Spi_5;
extern DEV_HANDLE  Spi_6;
extern DEV_HANDLE  Spi_7;
extern DEV_HANDLE  Spi_8;

bool Spi_Init(DEV_HANDLE Spi, SpiConfig* Config);
bool Spi_TransmitReceive(DEV_HANDLE Spi, uint8_t* TxData, uint8_t* RxData, uint16_t DataLen);
void Spi_SendComplete(DEV_HANDLE Spi);
void Spi_Recv(DEV_HANDLE Spi);

#ifdef __cplusplus
}
#endif

