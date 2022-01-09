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

CSerialEvent::CSerialEvent(Signals Sig, UART_HANDLE Uart_H, uint32_t BuffSize, bool Dynamic) : Event(Sig, Dynamic)
{
	this->m_Device = Uart_H;
	this->m_DataCount = BuffSize;
	this->m_Data = new uint8_t[BuffSize];
	ASSERT(this->m_Data);
	this->m_DataSize = BuffSize;
}

CSerialEvent::CSerialEvent(Signals Sig, UART_HANDLE Uart_H, const uint8_t* Data, uint16_t Len) : Event(Sig, true)
{
	this->m_Device = Uart_H;

	this->m_Data = new uint8_t[Len];
	ASSERT(this->m_Data);
	memcpy(this->m_Data, Data, Len);
	this->m_DataCount = Len;
	this->m_DataSize = Len;

}
CSerialEvent::~CSerialEvent()
{
	delete m_Data;
}

void CSerialEvent::SetSig(Signals Sig)
{
	this->Sig = Sig;
}
void CSerialEvent::CopyData(uint8_t* Data, uint16_t Len)
{
	if (Len)
	{
		memcpy(this->m_Data, Data, Len);
		this->m_DataCount = Len;
	}
	else
	{
		this->m_DataCount = 0;
	}
}

CUart::CUart(char *Name, UART_HANDLE Uart, 
		UART_Baudrate Baudrate, UART_Parity Parity, UART_StopBit Stopbit,
		uint16_t MaxFrameLen,  MACCALLBACK MacCall, uint32_t DQSize)
{
	if (Name)
	{
		snprintf(m_Name, sizeof(m_Name), "%s", Name);
	}
	else
	{
		m_Name[0] = 0;
	}

	m_Device = Uart;

	m_Config.Baudrate = Baudrate;
	m_Config.Parity = Parity;
	m_Config.StopBits = Stopbit;

	m_IrqEvent = new  CSerialEvent[2]{
		CSerialEvent(SERIAL_RSP_SIG, m_Device, MaxFrameLen, false),
		CSerialEvent(SERIAL_RSP_SIG, m_Device, MaxFrameLen, false)
	};
	ASSERT(m_IrqEvent);
	m_IrqEventIndex = 0;

	m_BuffSize = MaxFrameLen;
	m_BuffCount = 0;
	m_Buff4MacCall = m_IrqEvent[m_IrqEventIndex].m_Data;

	m_MacCall = MacCall;

	m_DQ = new CEventQ(DQSize);
	ASSERT(m_DQ);

	CUartKeeper::Instance()->RegUart(this);
}
CUart::~CUart()
{
	delete [] m_IrqEvent;
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
	INIT_TRANS(&CUart::S_Idle);
}

void CUart::S_Idle(Event const* const e)
{
	switch (e->Sig)
	{
	case ENTRY_SIG:
		FetchDeferedEvent();
		break;

	case MAC_REQ_SIG:
		Uart_Send(EventCast(CSerialEvent)->m_Device, const_cast<uint8_t *>(EventCast(CSerialEvent)->m_Data), EventCast(CSerialEvent)->m_DataCount);
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
	case SERIAL_HW_OUT_COMPLETE_SIG:
		
		TRANS(&CUart::S_Idle);
		break;
	case MAC_REQ_SIG:
		DeferEvent(e);
		break;
	default:
		break;
	}
}

// call from uart driver
void CUart::PostIrqEvent(Signals Sig)
{
	m_IrqEvent[m_IrqEventIndex].SetSig(Sig);
	Publish(&m_IrqEvent[m_IrqEventIndex], true);
	m_IrqEventIndex ^= 0x01;
	m_BuffCount = 0;
	m_Buff4MacCall = m_IrqEvent[m_IrqEventIndex].m_Data;
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

	Edf::Subscribe(MAC_REQ_SIG, this);
	Edf::Subscribe(SERIAL_HW_OUT_COMPLETE_SIG, this);
	INIT_TRANS(&CUartKeeper::S_Run);
}

void CUartKeeper::S_Run(Event const* const e)
{
	switch (e->Sig)
	{

	case MAC_REQ_SIG:
	case SERIAL_HW_OUT_COMPLETE_SIG:
		GetUart(EventCast(CSerialEvent)->m_Device)->Dispatcher(e);
		break;

	default:
		break;
	}
}


void CUartKeeper::SendComplete(UART_HANDLE UartH)
{
	CUart* Uart = GetUart(UartH);

	if (Uart)
	{
		LOG_DEBUG("keeper send completely: %s \r\n", Uart->m_Name);

		Uart->PostIrqEvent(SERIAL_HW_OUT_COMPLETE_SIG);
	}
}

void CUartKeeper::Receive(UART_HANDLE UartH, uint8_t AByte)
{
	CUart* Uart = GetUart(UartH);

	if (Uart && Uart->m_MacCall(Uart->m_Buff4MacCall, Uart->m_BuffSize, Uart->m_BuffCount, AByte))
	{
		LOG_DEBUG("keeper get: %d \r\n", Uart->m_BuffCount);

		Uart->PostIrqEvent(SERIAL_RSP_SIG);
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

