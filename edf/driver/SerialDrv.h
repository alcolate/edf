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

typedef void* UART_HANDLE;

typedef uint32_t UART_Baudrate;

typedef enum __uart_parity
{
	Parity_None,
	Parity_Odd,
	Parity_Even
}UART_Parity;

typedef enum __uart_stoptbits
{
	StopBit_1Bit,
	StopBit_2Bit
}UART_StopBit;

typedef struct __uartconfig
{
	UART_Baudrate	Baudrate;
	UART_Parity		Parity;
	UART_StopBit	StopBits;
	uint8_t			RecvBuff[1];
}UartConfig;

extern UART_HANDLE  UART_0;
extern UART_HANDLE  UART_1;
extern UART_HANDLE  UART_2;

bool Uart_Init(UART_HANDLE Uart, UartConfig* Config);
bool Uart_Send(UART_HANDLE Uart, uint8_t* Data, uint16_t DataLen);
void Uart_SendComplete(UART_HANDLE Uart);
void Uart_Recv(UART_HANDLE Uart, uint8_t Data);

#ifdef __cplusplus
}
#endif

