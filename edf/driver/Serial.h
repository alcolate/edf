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

#include <memory.h>
#include "SerialDrv.h"
#include "Driver.h"
#include "Edf.h"

namespace Edf
{


class CSerialEvent : public CDeviceEvent
{
public:
	CSerialEvent(Signals Sig, UART_HANDLE UartHandle, uint32_t BuffSize, bool Dynamic = true);

	virtual ~CSerialEvent();
};


class CSerial : public CDevice
{
public:
	CSerial(char *Name, UART_HANDLE Uart, 
			UART_Baudrate Baudrate, UART_Parity Parity, UART_StopBit Stopbit,
			uint16_t MaxFrameLen, 
			MACCALLBACK MacCall, uint32_t DQSize = 2);
	~CSerial();

	virtual void Initial(CActive *Owner) override;
	virtual void PostIrqRecvEvent() override;

	virtual bool MacCall(uint8_t *Data, uint32_t Len) override;
protected:
	virtual bool Send(Event const* const e) override;

public:
	uint8_t* m_Buff4MacCall;
	uint16_t 	m_BuffSize;
	uint16_t 	m_BuffCount;
	MACCALLBACK m_MacCall;
	
	CDeviceEvent* m_IrqRecvEvent;
	uint32_t	m_IrqRecvEventIndex;

	UartConfig 	m_Config;

};



} // namespace Edf

