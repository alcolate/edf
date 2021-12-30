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
	CUartEvent(UARTDEV_H Uart_H) : Event(0, false)
	{
		this->Uart_H = Uart_H;
		this->DataLen = 0;
		this->Status = 0;
	}

	CUartEvent(Signals Sig, UARTDEV_H Uart_H, uint8_t* Data, uint16_t Len) : Event(Sig, true)
	{
		this->Uart_H = Uart_H;
		if (Len)
		{
			memcpy(this->Data, Data, Len);
			this->DataLen = Len;
		}
		else
		{
			this->DataLen = 0;
		}
		this->Status = 0;
	}
	~CUartEvent()
	{
	}

	void SetSig(Signals Sig)
	{
		this->Sig = Sig;
	}
	void CopyData(uint8_t* Data, uint16_t Len)
	{
		if (Len)
		{
			memcpy(this->Data, Data, Len);
			this->DataLen = Len;
		}
		else
		{
			this->DataLen = 0;
		}
	}
	enum {FRAME_MAX_LEN = 32};
	UARTDEV_H  Uart_H;
	uint8_t Data[FRAME_MAX_LEN];
	uint16_t DataLen;
	uint16_t Status;

};

class CUart
{
public:
	CUart(char *Name)
	{
		m_Sibling = 0;
	}

	UARTDEV_H  m_Uart_H;			// the no. of uart

	UartConfig m_Config;

	uint8_t* m_Buff4MacCall;

	uint16_t m_BuffSize;

	uint16_t m_BuffCount;

	CUart* m_Sibling;

	char *m_Name;

	CUartEvent *m_IrqEvent;

	// the method is used to get a frame
	virtual bool  MacCall(uint8_t Abyte) = 0;
};

class CUartKeeper : public CActive
{
public:

	static CUartKeeper* Instance();

	void RegUart(CUart* Uart);

	virtual void Initial();

	void SendComplete(UARTDEV_H UartH);

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

