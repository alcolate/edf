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
#include "Edf.h"

namespace Edf
{

class CUartEvent : public Event
{
public:
	CUartEvent(Signal Sig, UARTDEV_H Uart_H, uint8_t* Data, uint16_t Len) : Event(Sig, true)
	{
		this->Uart_H = Uart_H;
		this->Data = new uint8_t[Len];
		memcpy(this->Data, Data, Len);
		this->DataLen = Len;
		this->Status = 0;
	}
	~CUartEvent()
	{
		if (Data)
			delete [] Data;
	}
	UARTDEV_H  Uart_H;
	uint8_t* Data;
	uint16_t DataLen;
	uint16_t Status;

};

class CUart
{
public:
	CUart() 
	{
		m_Sibling = 0;
	}
	CUart(UARTDEV_H UartH, UartConfig* Config, uint8_t* Buff4MacCall, uint16_t BuffSize)
	{
		m_Uart_H = UartH;
		m_Config = *Config;
		this->m_Buff4MacCall = Buff4MacCall;
		this->m_BuffSize = BuffSize;
		this->m_BuffCount = 0;
		m_Sibling = 0;
	}
	UARTDEV_H  m_Uart_H;			// the no. of uart

	UartConfig m_Config;

	uint8_t* m_Buff4MacCall;

	uint16_t m_BuffSize;

	uint16_t m_BuffCount;

	CUart* m_Sibling;

	// the method is used to get a frame
	virtual bool  MacCall(uint8_t Abyte) = 0;
};

class CUartKeeper : public CActive
{
public:

	static CUartKeeper* Instance();

	void RegUart(CUart* Uart);

	virtual void Initial();

	void Receive(UARTDEV_H UartH, uint8_t AByte);

private:
	void S_Run(Event const* const e);

	CUart* GetUart(UARTDEV_H UartH);

	void AddUart(CUart* Uart);

	CUartKeeper() : CActive((char*)"UartKeeper")
	{
		m_Uart = 0;
	}

	CUart* m_Uart;

public:
	DEF_STATEMACHINE(CUartKeeper);
};

} // namespace Edf

