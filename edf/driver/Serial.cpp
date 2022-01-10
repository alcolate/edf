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
#include <string.h>
#include <stdio.h>
#include "Serial.h"

namespace Edf
{

CSerialEvent::CSerialEvent(Signals Sig, UART_HANDLE UartHandle, uint32_t BuffSize, bool Dynamic) 
	: CDeviceEvent(Sig, UartHandle, BuffSize,  Dynamic) 
{

}

CSerialEvent::~CSerialEvent()
{

}


CUart::CUart(char *Name, UART_HANDLE Uart, 
		UART_Baudrate Baudrate, UART_Parity Parity, UART_StopBit Stopbit,
		uint16_t MaxFrameLen,  MACCALLBACK MacCall, uint32_t DQSize) :
		CDevice(Name, Uart, MaxFrameLen, MacCall, DQSize)
{
	m_Config.Baudrate = Baudrate;
	m_Config.Parity = Parity;
	m_Config.StopBits = Stopbit;

	CDevKeeper::Instance()->RegDevice(this);
}
CUart::~CUart()
{
	
}

void CUart::Initial(CActive *Owner)
{
	bool result = Uart_Init(m_Device, &m_Config);
	ASSERT(result);

	CDevice::Initial(Owner);
}
bool CUart::Send(uint8_t *Data, uint32_t Len)
{
	return Uart_Send(m_Device, Data, Len);
}



} // namespace Edf

void Uart_SendComplete(UART_HANDLE Uart)
{
	CDevKeeper::Instance()->SendComplete(Uart);
}
void Uart_Recv(UART_HANDLE Uart, uint8_t Data)
{
	CDevKeeper::Instance()->Receive(Uart, &Data, 1);
}

