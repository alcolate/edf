/*****************************************************************************
* MIT License:
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*
* Contact information:
* <9183399@qq.com>
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
			delete Data;
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

