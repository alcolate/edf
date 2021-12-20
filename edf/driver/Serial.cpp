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
	bool result = Uart_Init(Uart->m_Uart_H, &Uart->m_Config);
	ASSERT(result);

	AddUart(Uart);

}
void CUartKeeper::AddUart(CUart* Uart)
{
	Uart->m_Sibling = m_Uart;
	m_Uart = Uart;
}

CUart* CUartKeeper::GetUart(UARTDEV_H UartH)
{
	CUart* p = m_Uart;

	for (; p; p = p->m_Sibling)
	{
		if (p->m_Uart_H == UartH)
		{
			return p;
		}
	}

	ASSERT(false);
	return NULL;

}

void CUartKeeper::Initial()
{

	Edf::Subscribe(SERIAL_OUT_SIG, this);
	INIT_TRANS(&CUartKeeper::S_Run);
}

void CUartKeeper::Receive(UARTDEV_H UartH, uint8_t AByte)
{
	CUart* Uart = GetUart(UartH);

	if (Uart->MacCall(AByte))
	{
		LOG_DEBUG("keeper get: %s \r\n", Uart->m_Buff4MacCall);
		CUartEvent* ue = new CUartEvent(SERIAL_IN_SIG, UartH, Uart->m_Buff4MacCall, Uart->m_BuffCount);
		Uart->m_BuffCount = 0;
		Publish(ue);
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
		Uart_Send(ev->Uart_H, ev->Data, ev->DataLen);
		break;
	}
	case TEST_SIG:

		break;

	default:
		break;
	}
}
} // namespace Edf
