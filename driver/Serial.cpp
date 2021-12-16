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
	bool result = Uart_Init(Uart->m_Uart_H, Uart->m_Config);
	ASSERT(result);
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
		LOG_DEBUG("keeper get: %s \r\n", Uart->Buff4MacCall);
		CUartEvent* ue = new CUartEvent(SERIAL_IN_SIG, UartH, Uart->Buff4MacCall, Uart->BuffCount);
		Uart->BuffCount = 0;
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
