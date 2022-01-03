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
#include "Serial.h"

namespace Edf
{

CUartEvent::CUartEvent(Signals Sig, UART_HANDLE Uart_H, uint32_t BuffSize, bool Dynamic) : Event(Sig, Dynamic)
{
	this->Device = Uart_H;
	this->DataCount = BuffSize;
	this->Data = new uint8_t[BuffSize];
	ASSERT(this->Data);
	this->DataSize = BuffSize;
}

CUartEvent::CUartEvent(Signals Sig, UART_HANDLE Uart_H, const uint8_t* Data, uint16_t Len) : Event(Sig, true)
{
	this->Device = Uart_H;

	this->Data = new uint8_t[Len];
	ASSERT(this->Data);
	memcpy(this->Data, Data, Len);
	this->DataCount = Len;
	this->DataSize = Len;

}
CUartEvent::~CUartEvent()
{
	delete Data;
}

void CUartEvent::SetSig(Signals Sig)
{
	this->Sig = Sig;
}
void CUartEvent::CopyData(uint8_t* Data, uint16_t Len)
{
	if (Len)
	{
		memcpy(this->Data, Data, Len);
		this->DataCount = Len;
	}
	else
	{
		this->DataCount = 0;
	}
}

CUart::CUart(char *Name, UART_HANDLE Uart, 
		UART_Baudrate Baudrate, UART_Parity Parity, UART_StopBit Stopbit,
		uint16_t MaxFrameLen,  MACCALLBACK MacCall, uint32_t DQSize)
{
	if (Name)
	{
		strncpy_s(m_Name, Name, sizeof(m_Name) - 1);
	}
	else
	{
		m_Name[0] = 0;
	}

	m_Device = Uart;

	m_Config.Baudrate = Baudrate;
	m_Config.Parity = Parity;
	m_Config.StopBits = Stopbit;

	m_BuffSize = MaxFrameLen;
	m_BuffCount = 0;
	m_Buff4MacCall = new uint8_t [MaxFrameLen];
	ASSERT(m_Buff4MacCall);

	m_MacCall = MacCall;

	m_IrqEvent = new CUartEvent(SERIAL_IN_SIG, m_Device, MaxFrameLen, false);
	ASSERT(m_IrqEvent);

	m_DQ = new CEventQ(DQSize);
	ASSERT(m_DQ);

	CUartKeeper::Instance()->RegUart(this);
}
CUart::~CUart()
{
	delete m_Buff4MacCall;
	delete m_IrqEvent;
}

void CUart::ResetBuff()
{
	m_BuffCount = 0;
	memset(m_Buff4MacCall, 0, m_BuffSize);
}

void CUart::Dispatcher(Event const* const e)
{
	RunState(e);
}

void CUart::Initial()
{
	INIT_TRANS(&CUart::S_Run);
}

void CUart::S_Run(Event const* const e)
{
	switch (e->Sig)
	{
	case ENTRY_SIG:
		FetchDeferedEvent();
		break;

	case SERIAL_OUT_SIG:
		Uart_Send(EventCast(CUartEvent)->Device, const_cast<uint8_t *>(EventCast(CUartEvent)->Data), EventCast(CUartEvent)->DataCount);
		TRANS(&CUart::S_WaitComplete);
		break;

	default:
		break;
	}
}
void CUart::S_WaitComplete(Event const* const e)
{
	switch (e->Sig)
	{
	case SERIAL_OUT_COMPLETE_SIG:
		
		TRANS(&CUart::S_Run);
		break;
	case SERIAL_OUT_SIG:
		DeferEvent(e);
		break;
	default:
		break;
	}
}

bool CUart::DeferEvent(Event const* const e)
{
	ASSERT(e);

	return m_DQ->Defer(e);

}

void CUart::FetchDeferedEvent()
{
	const Event* e = m_DQ->Fetch();

	if (e)
	{
		m_Owner->Post(e);
	}
}

CUartKeeper* CUartKeeper::Instance()
{
	static CUartKeeper uk;
	return &uk;
}

void CUartKeeper::RegUart(CUart* Uart)
{
	bool result = Uart_Init(Uart->m_Device, &Uart->m_Config);
	ASSERT(result);

	AddUart(Uart);
	Uart->m_Owner = this;
	Uart->Initial();

}
void CUartKeeper::AddUart(CUart* Uart)
{
	m_Uart.AddHead(Uart);
}

CUart* CUartKeeper::GetUart(UART_HANDLE UartH)
{
	CUart *p = m_Uart.Head();

	for (; p && p->m_Device != UartH; p = m_Uart.Next(p));

	ASSERT(p);
	return p;

}

void CUartKeeper::Initial()
{

	Edf::Subscribe(SERIAL_OUT_SIG, this);
	Edf::Subscribe(SERIAL_OUT_COMPLETE_SIG, this);
	INIT_TRANS(&CUartKeeper::S_Run);
}

void CUartKeeper::S_Run(Event const* const e)
{
	switch (e->Sig)
	{

	case SERIAL_OUT_SIG:
	case SERIAL_OUT_COMPLETE_SIG:
		GetUart(EventCast(CUartEvent)->Device)->Dispatcher(e);		
		break;

	default:
		break;
	}
}
#define FROM_IRQ
void CUartKeeper::SendComplete(UART_HANDLE UartH)
{
	CUart* Uart = GetUart(UartH);

	if (Uart)
	{
		LOG_DEBUG("keeper send completely: %s \r\n", Uart->m_Name);
#ifdef FROM_IRQ
		Uart->m_IrqEvent->SetSig(SERIAL_OUT_COMPLETE_SIG);
		Uart->m_IrqEvent->CopyData(0, 0);
		CUartKeeper::Instance()->Post(Uart->m_IrqEvent, true);
#else
		CUartEvent* ue = new CUartEvent(SERIAL_OUT_COMPLETE_SIG, UartH, 10, true);
		Publish(ue);
#endif
	}
}

void CUartKeeper::Receive(UART_HANDLE UartH, uint8_t AByte)
{
	CUart* Uart = GetUart(UartH);

	if (Uart && Uart->m_MacCall(Uart->m_Buff4MacCall, Uart->m_BuffSize, Uart->m_BuffCount, AByte))
	{
		LOG_DEBUG("keeper get: %d \r\n", Uart->m_BuffCount);
#ifdef FROM_IRQ
		Uart->m_IrqEvent->SetSig(SERIAL_IN_SIG);
		Uart->m_IrqEvent->CopyData(Uart->m_Buff4MacCall, Uart->m_BuffCount);
		Uart->m_BuffCount = 0;
		Publish(Uart->m_IrqEvent, true);
#else
		CUartEvent* ue = new CUartEvent(SERIAL_IN_SIG, UartH, Uart->m_Buff4MacCall, Uart->m_BuffCount);
		Uart->m_BuffCount = 0;
		Publish(ue);
#endif
	}
}

} // namespace Edf

void Uart_SendComplete(UART_HANDLE Uart)
{
	CUartKeeper::Instance()->SendComplete(Uart);
}
void Uart_Recv(UART_HANDLE Uart, uint8_t Data)
{
	CUartKeeper::Instance()->Receive(Uart, Data);
}

