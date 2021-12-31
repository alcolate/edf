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
#include "Serial.h"

namespace Edf
{
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

}
void CUartKeeper::AddUart(CUart* Uart)
{
	m_Uart.AddHead(Uart);
}

CUart* CUartKeeper::GetUart(UART_HANDLE UartH)
{
	CUart *p = m_Uart.Head();

	while (p)
	{
		if (p->m_Device == UartH)
		{
			return p;
		}

		p = m_Uart.Next(p);
	}

	ASSERT(false);
	return NULL;

}

void CUartKeeper::Initial()
{

	Edf::Subscribe(SERIAL_OUT_SIG, this);
	INIT_TRANS(&CUartKeeper::S_Run);
}

void CUartKeeper::SendComplete(UART_HANDLE UartH)
{
	CUart* Uart = GetUart(UartH);

	if (Uart)
	{
		LOG_DEBUG("keeper send completely: %s \r\n", Uart->m_Name);
		Uart->m_IrqEvent->SetSig(SERIAL_OUT_COMPLETE_SIG);
		Uart->m_IrqEvent->CopyData(0, 0);
		Publish(Uart->m_IrqEvent);
	}
}

void CUartKeeper::Receive(UART_HANDLE UartH, uint8_t AByte)
{
	CUart* Uart = GetUart(UartH);

	if (Uart && Uart->MacCall(AByte))
	{
		LOG_DEBUG("keeper get: %s \r\n", Uart->m_Buff4MacCall);
		Uart->m_IrqEvent->SetSig(SERIAL_IN_SIG);
		Uart->m_IrqEvent->CopyData(Uart->m_Buff4MacCall, Uart->m_BuffCount);
		Publish(Uart->m_IrqEvent);
		Uart->m_BuffCount = 0;
	}
}

void CUartKeeper::S_Run(Event const* const e)
{
	switch (e->Sig)
	{
	case ENTRY_SIG:
		break;
	case SERIAL_OUT_SIG:
	{
		CUartEvent const* ev = static_cast<CUartEvent const*>(e);
		LOG_DEBUG("keeper send: %s \r\n", ev->Data);
		Uart_Send(ev->UartDevice, const_cast<uint8_t *>(ev->Data), ev->DataLen);
		break;
	}
	case TEST_SIG:

		break;

	default:
		break;
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

